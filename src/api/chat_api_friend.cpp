#include "chat_api.h"

using namespace api::chat;

grpc::ServerUnaryReactor *
ChatApiService::FetchFriendList(grpc::CallbackServerContext *context, const None *none, UserList *list) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto name = userManager.check_user_credentials(metadata);
    if (!name) {
        absl::PrintF("Illegal user tried to login!\n");
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    reactor->Finish(grpc::Status(grpc::StatusCode::UNIMPLEMENTED, ""));
    return reactor;
}

grpc::ServerUnaryReactor *
ChatApiService::ManageFriend(::grpc::CallbackServerContext *context, const FriendManageOperation *operation, None *none) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto name = userManager.check_user_credentials(metadata);
    if (!name) {
        absl::PrintF("Illegal user tried to login!\n");
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    auto action = operation->action();
    uint64_t user_id = operation->user_id();
    uint64_t friend_id = operation->friend_id();
    switch (action) {
        case FriendManageOperation::ActionType::FriendManageOperation_ActionType_Add:
            userManager.set_user_relationship(user_id, friend_id, true);
            reactor->Finish(grpc::Status::OK);
            return reactor;
            break;
        case FriendManageOperation::ActionType::FriendManageOperation_ActionType_Remove:
            userManager.set_user_relationship(user_id, friend_id, false);
            reactor->Finish(grpc::Status::OK);
            return reactor;
            break;
        default:
            break;
    }


    reactor->Finish(grpc::Status(grpc::StatusCode::INTERNAL, ""));
    return reactor;
}

