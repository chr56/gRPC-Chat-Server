
#include <iostream>
#include <string_view>

#include "chat_api.h"

using namespace api::chat;

class ChatApiService::MessageStreamReactor : public grpc::ServerWriteReactor<ChatMessages>, ChatApiService::Client {
public:
    MessageStreamReactor(ChatApiService *service, grpc::CallbackServerContext *context)
            : _service(service), _context(context) {}

    void Start() {

        // Authenticate user
        auto metadata = _context->client_metadata();
        auto name = _service->authenticator.check_user_credentials(metadata);
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
        std::cout << "User " << name.value() << " connected\n";

        // Update messages
        _writing = true;
        std::cout << "Sending all messages to " << name.value() << "\n";
        StartWrite(&(_service->_messages));
    }


    void OnWriteDone(bool ok) override {
        if (!ok || !_connected) {
            _connected = false;
            _service->_clients.remove(this);
            std::cout << "Error occurs! (" << "user: " << _username << ", ok: " << ok << ", connected: " << _connected << ")\n";
            this->Finish(grpc::Status(grpc::StatusCode::UNKNOWN, "Unexpected Failure"));
            delete this;
            return;
        }

        // Prepare for the next write (if needed)
        if (!_pendingMessages.empty()) {
            std::cout << "Sending updated messages to " << _username << "\n";
            _writing = true;
            StartWrite(&(_pendingMessages.front()));
            _pendingMessages.pop_front();
        } else {
            // std::cout << "Completed to send messages to " << _username << "\n";
            _writing = false;
        }
    }

    void OnDone() override {
        std::cout << "User " << _username << " disconnected!\n";
        _connected = false;
        _service->_clients.remove(this);
        this->Finish(grpc::Status::OK);
        delete this;
    }

    void OnCancel() override {
        std::cout << "User " << _username << " canceled!\n";
        _connected = false;
    }

    void NotifyNewMessage(const ChatMessage &message) override {
        if (!_connected) return;

        ChatMessages messages;
        *messages.add_messages() = message;

        if (_writing) {
            std::cout << "Notify new message to user " << _username << " (pending)" << "\n";
            _pendingMessages.push_back(messages);
        } else {
            std::cout << "Notify new message to user " << _username << "\n";
            _writing = true;
            StartWrite(&messages);
        }
    }

private:

    ChatApiService *_service;
    grpc::CallbackServerContext *_context;

    std::string _username;

    bool _connected = false;
    bool _writing = false;
    std::list<ChatMessages> _pendingMessages;
};

grpc::ServerUnaryReactor *ChatApiService::SendMessage(
        grpc::CallbackServerContext *context, const ChatMessage *request, None *response
) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto name = authenticator.check_user_credentials(metadata);
    if (!name) {
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    // Add message to the list and notify clients
    auto *msg = _messages.add_messages();
    *msg = *request;
    msg->set_from(*name);

    notifyClients(*msg);

    reactor->Finish(grpc::Status::OK);
    return reactor;
}

grpc::ServerWriteReactor<ChatMessages> *
ChatApiService::FetchMessageList(grpc::CallbackServerContext *context, const None *request) {
    auto reactor = new ChatApiService::MessageStreamReactor(this, context);
    reactor->Start();
    return reactor;
}

void ChatApiService::notifyClients(const ChatMessage &message) {
    for (auto client: _clients) {
        client->NotifyNewMessage(message);
    }
}

