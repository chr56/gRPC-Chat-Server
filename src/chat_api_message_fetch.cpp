#include <iostream>

#include "chat_api.h"

using namespace api::chat;


class ChatApiService::MessageStreamReactor : public grpc::ServerWriteReactor<ChatMessages>, ChatApiService::Client {
public:
    MessageStreamReactor(ChatApiService *service, grpc::CallbackServerContext *context, const FetchMessageListRequest *request)
            : _service(service), _context(context), _request(request) {}

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
        std::cout << "User " << _username << " terminated!\n";
        _connected = false;
        _service->_clients.remove(this);
        this->Finish(grpc::Status::OK);
        delete this;
    }

    void OnCancel() override {
        std::cout << "User " << _username << " disconnected!\n";
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
    const FetchMessageListRequest *_request; // todo

    std::string _username;

    bool _connected = false;
    bool _writing = false;
    std::list<ChatMessages> _pendingMessages;
};

grpc::ServerWriteReactor<api::chat::ChatMessages> *
ChatApiService::FetchMessageList(grpc::CallbackServerContext *context, const FetchMessageListRequest *request) {
    auto reactor = new ChatApiService::MessageStreamReactor(this, context, request);
    reactor->Start();
    return reactor;
}
