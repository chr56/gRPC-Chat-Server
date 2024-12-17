#include <iostream>

#include "chat_api.h"

using namespace api::chat;


grpc::ServerUnaryReactor *
ChatApiService::SendMessageTo(grpc::CallbackServerContext *context, const SendMessageRequest *request, None *none) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto user = valid_user_credentials(metadata);
    if (!user) {
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    // Check Messages
    auto id = request->target();
    MessageList *pending = nullptr;
    bool is_private_message = request->is_user();
    if (!is_private_message) {
        // group message
        auto messages = db.get_group_messages_by_id(id);
        if (!messages) {
            reactor->Finish(grpc::Status(grpc::StatusCode::NOT_FOUND, "Chat Not Found"));
            return reactor;
        }
        pending = messages.value();
    } else {
        auto messages = db.get_private_messages_by_id(user.value().id(), id);
        if (!messages) {
            reactor->Finish(grpc::Status(grpc::StatusCode::NOT_FOUND, "Chat Not Found"));
            return reactor;
        }
        pending = messages.value();
    }
    //  Notify new messages
    auto *msg = pending->add_messages();
    msg->set_sender_user_name(user.value().name());
    msg->set_sender_user_id(0);
    *msg = request->message();
    notifyClients(id, is_private_message, *msg);


    reactor->Finish(grpc::Status::OK);
    return reactor;
}

void ChatApiService::notifyClients(uint64_t target, bool is_user, const Message &message) {
    for (auto client: _clients) {
        if (is_user)
            client->NotifyNewPrivateMessage(target, message);
        else
            client->NotifyNewMessage(target, message);
    }
}

