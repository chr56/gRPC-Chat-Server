#include <iostream>

#include "absl/strings/str_format.h"

#include "chat_api.h"

using namespace api::chat;


class ChatApiService::MessageStreamReactor : public grpc::ServerWriteReactor<MessageList>, ChatApiService::Client {
public:
    MessageStreamReactor(ChatApiService *service, grpc::CallbackServerContext *context, const FetchRequest *request)
            : _service(service), _context(context), _request(request) {}

    static std::string request_target_name(const FetchRequest *_request) {
        std::string str(_request->is_user() ? "User Chat" : "Group Chat");
        return str + " (id: " + std::to_string(_request->target()) + ")";
    }

    void Start() {

        // Authenticate user
        auto metadata = _context->client_metadata();
        auto user = _service->valid_user_credentials(metadata);
        if (!user) {
            this->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
            delete this;
            return;
        }

        // Register user
        _username = user.value().name();
        _id = user.value().id();
        _connected = true;
        _service->_clients.push_back(this);
        absl::PrintF("User %s connected to %s\n", _username, request_target_name(_request));

        // Find Message
        uint64_t chat_id;
        uint64_t target = _request->target();
        if (_request->is_user()) {
            // Private Messages
            auto chat = _service->db.get_private_chat(_id, target);
            if (!chat) {
                // Create new chat
                chat_id = _service->db.create_chat_and_messages("Private Messages", false);
                auto me = _service->db.get_user_by_id(_id);
                auto other = _service->db.get_user_by_id(target);
                if (other.has_value() && me.has_value()) {
                    _service->db.add_member(chat_id, _id);
                    _service->db.add_member(chat_id, target);
                    absl::PrintF("Create a new private chat %ul (user %ul and %ul)\n", chat_id, _id, target);
                } else {
                    this->Finish(grpc::Status(grpc::StatusCode::NOT_FOUND, absl::StrFormat("%s Not Found", request_target_name(_request))));
                    delete this;
                    return;
                }
            } else {
                // Already created
                chat_id = chat->id();
            }
        } else {
            // Group Message
            chat_id = target;
        }

        // Checking Messages
        auto messages = _service->db.get_messages_by_id(chat_id);
        if (!messages) {
            this->Finish(grpc::Status(grpc::StatusCode::NOT_FOUND, absl::StrFormat("%s Not Found", request_target_name(_request))));
            delete this;
            return;
        }

        // Sending messages
        _writing = true;
        absl::PrintF("Sending %s Messages to %s\n", request_target_name(_request), _username);
        StartWrite(messages.value());
    }


    void OnWriteDone(bool ok) override {
        if (!ok || !_connected) {
            _connected = false;
            _service->_clients.remove(this);
            absl::PrintF("Error occurs! (user: %s, ok: %d, connected: %d)\n", _username.c_str(), ok, _connected);
            return;
        }

        // Prepare for the next write (if needed)
        if (!_pendingMessages.empty()) {
            absl::PrintF("Sending updated messages to %s\n", _username.c_str());
            _writing = true;
            StartWrite(&(_pendingMessages.front()));
            _pendingMessages.pop_front();
        } else {
            _writing = false;
            absl::PrintF("Completed to send messages to %s\n", _username.c_str());
        }
    }

    void OnDone() override {
        absl::PrintF("User %s terminated from %s\n", _username, request_target_name(_request));
        _connected = false;
        _service->_clients.remove(this);
        this->Finish(grpc::Status::OK);
        delete this;
    }

    void OnCancel() override {
        absl::PrintF("User %s disconnected from %s\n", _username, request_target_name(_request));
        _connected = false;
    }

    void NotifyNewMessage(uint64_t chat_id, const Message &message) override {
        if (!_connected) return;

        if (_request->is_user() || chat_id != _request->target()) return;


        MessageList messages;
        *messages.add_messages() = message;

        if (_writing) {
            absl::PrintF("Notify new message (chat %u) to %s (pending)\n", chat_id, _username.c_str());
            _pendingMessages.push_back(messages);
        } else {
            absl::PrintF("Notify new message (chat %u) to %s\n", chat_id, _username.c_str());
            _writing = true;
            StartWrite(&messages);
        }
    }

    void NotifyNewPrivateMessage(uint64_t user_id, const Message &message) override {
        if (!_connected) return;

        if (!_request->is_user() || user_id != _request->target()) return;

        MessageList messages;
        *messages.add_messages() = message;

        if (_writing) {
            absl::PrintF("Notify new message (chat %u) to %s (pending)\n", user_id, _username.c_str());
            _pendingMessages.push_back(messages);
        } else {
            absl::PrintF("Notify new message (chat %u) to %s\n", user_id, _username.c_str());
            _writing = true;
            StartWrite(&messages);
        }
    }

private:

    ChatApiService *_service;
    grpc::CallbackServerContext *_context;
    const FetchRequest *_request;

    std::string _username;
    uint64_t _id;

    bool _connected = false;
    bool _writing = false;
    std::list<MessageList> _pendingMessages;
};

grpc::ServerWriteReactor<api::chat::MessageList> *
ChatApiService::FetchMessageList(grpc::CallbackServerContext *context, const FetchRequest *request) {
    auto reactor = new ChatApiService::MessageStreamReactor(this, context, request);
    reactor->Start();
    return reactor;
}
