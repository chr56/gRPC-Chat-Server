#include <iostream>

#include "chat_api.h"

using namespace api::chat;


class ChatApiService::MessageStreamReactor : public grpc::ServerWriteReactor<MessageList>, ChatApiService::Client {
public:
    MessageStreamReactor(ChatApiService *service, grpc::CallbackServerContext *context, const FetchMessageListRequest *request)
            : _service(service), _context(context), _request(request) {}

    void Start() {

        // Authenticate user
        auto metadata = _context->client_metadata();
        auto name = _service->userManager.check_user_credentials(metadata);
        if (!name) {
            this->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
            delete this;
            std::cout << "Illegal user tried to login!\n";
            return;
        }

        // Register user
        _username = *name;
        _connected = true;
        _service->_clients.push_back(this);
        std::cout << "User " << name.value() << " connected to Chat(id: " << _request->chat_id() << ")\n";

        // Update messages

        uint64_t target_chat = _request->chat_id();
        auto messages = _service->chatManager.get_messages_by_id(target_chat);

        if (!messages) {
            this->Finish(grpc::Status(grpc::StatusCode::NOT_FOUND, "Chat Not Found"));
            delete this;
            return;
        }

        _writing = true;
        std::cout << "Sending Chat Messages(id: " << target_chat << ") to " << name.value() << "\n";
        StartWrite(messages.value());
    }


    void OnWriteDone(bool ok) override {
        if (!ok || !_connected) {
            _connected = false;
            _service->_clients.remove(this);
            std::cout << "Error occurs! (" << "user: " << _username << ", ok: " << ok << ", connected: " << _connected << ")\n";
            return;
        }

        // Prepare for the next write (if needed)
        if (!_pendingMessages.empty()) {
            std::cout << "Sending updated messages to " << _username << "\n";
            _writing = true;
            StartWrite(&(_pendingMessages.front()));
            _pendingMessages.pop_front();
        } else {
            _writing = false;
            std::string msg("Completed to send messages to ");
            std::cout << (msg + _username + "\n");
        }
    }

    void OnDone() override {
        std::cout << "User " << _username << " terminated from Chat(id: " << _request->chat_id() << ")\n";
        _connected = false;
        _service->_clients.remove(this);
        this->Finish(grpc::Status::OK);
        delete this;
    }

    void OnCancel() override {
        std::cout << "User " << _username << " disconnected from Chat(id: " << _request->chat_id() << ")\n";
        _connected = false;
    }

    void NotifyNewMessage(uint64_t chat_id, const Message &message) override {
        if (!_connected) return;

        if (chat_id != _request->chat_id()) return;


        MessageList messages;
        *messages.add_messages() = message;

        if (_writing) {
            std::cout << "Notify new message (chat " << chat_id << ") to " << _username << " (pending)" << "\n";
            _pendingMessages.push_back(messages);
        } else {
            std::cout << "Notify new message (chat " << chat_id << ") to " << _username << "\n";
            _writing = true;
            StartWrite(&messages);
        }
    }

private:

    ChatApiService *_service;
    grpc::CallbackServerContext *_context;
    const FetchMessageListRequest *_request;

    std::string _username;

    bool _connected = false;
    bool _writing = false;
    std::list<MessageList> _pendingMessages;
};

grpc::ServerWriteReactor<api::chat::MessageList> *
ChatApiService::FetchMessageList(grpc::CallbackServerContext *context, const FetchMessageListRequest *request) {
    auto reactor = new ChatApiService::MessageStreamReactor(this, context, request);
    reactor->Start();
    return reactor;
}
