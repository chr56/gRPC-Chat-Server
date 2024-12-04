#include <iostream>

#include "chat_api.h"

using namespace api::chat;


grpc::ServerUnaryReactor *
ChatApiService::FetchChatList(grpc::CallbackServerContext *context, const None *none, ChatList *list) {

    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto name = userManager.check_user_credentials(metadata);
    if (!name) {
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    // Get All Chats
    auto chatList = chatManager.convert(chatManager.list_all_chats());
    *list = *chatList.value();

    // Finish
    reactor->Finish(grpc::Status::OK);

    return reactor;
}
