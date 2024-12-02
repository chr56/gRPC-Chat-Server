#include <iostream>


#include "chat_api.h"
using namespace api::chat;

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

void ChatApiService::notifyClients(const ChatMessage &message) {
    for (auto client: _clients) {
        client->NotifyNewMessage(message);
    }
}
