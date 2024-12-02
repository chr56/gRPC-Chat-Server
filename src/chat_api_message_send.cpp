#include <iostream>


#include "chat_api.h"

using namespace api::chat;

grpc::ServerUnaryReactor *SimpleChatApiService::SendMessage(
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

void SimpleChatApiService::notifyClients(const ChatMessage &message) {
    for (auto client: _clients) {
        client->NotifyNewMessage(message);
    }
}

grpc::ServerUnaryReactor *
ChatApiService::SendMessageTo(::grpc::CallbackServerContext *context, const ::api::chat::SendMessageRequest *request,
                              ::api::chat::None *none) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto name = authenticator.check_user_credentials(metadata);
    if (!name) {
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    //  Notify new messages to other clients
    auto *msg = _messages.add_messages();
    msg->set_from(*name);
    *msg = request->message();
    uint64_t id = request->to();
    notifyClients(id, *msg);

    reactor->Finish(grpc::Status::OK);
    return reactor;
}

void ChatApiService::notifyClients(uint64_t dialogId, const ChatMessage &message) {
    for (auto client: _clients) {
        client->NotifyNewMessage(message);
    }
}

