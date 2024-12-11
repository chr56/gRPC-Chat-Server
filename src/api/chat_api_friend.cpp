#include "chat_api.h"

using namespace api::chat;

grpc::ServerUnaryReactor *
ChatApiService::FetchFriendList(grpc::CallbackServerContext *context, const None *none, UserList *list) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto user = valid_user_credentials(metadata);
    if (!user) {
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    const auto &all_users = db.get_user_friends(user->id());
    absl::PrintF("Sending friends list to %s \n", user.value().name());
    for (const auto &friend_user: all_users) {
        User *newUser = list->add_users();
        newUser->CopyFrom(friend_user);
    }
    reactor->Finish(grpc::Status::OK);
    absl::PrintF("Completed to send friends list to %s \n", user.value().name());
    return reactor;
}

grpc::ServerUnaryReactor *
ChatApiService::ManageFriend(::grpc::CallbackServerContext *context, const FriendManageOperation *operation, None *none) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto user = valid_user_credentials(metadata);
    if (!user) {
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }


    auto action = operation->action();
    uint64_t user_id = operation->user_id();
    uint64_t friend_id = operation->friend_id();
    switch (action) {
        case FriendManageOperation::ActionType::FriendManageOperation_ActionType_Add:
            db.make_friend(user_id, friend_id);
            reactor->Finish(grpc::Status::OK);
            return reactor;
            break;
        case FriendManageOperation::ActionType::FriendManageOperation_ActionType_Remove:
            db.remove_friend(user_id, friend_id);
            reactor->Finish(grpc::Status::OK);
            return reactor;
            break;
        default:
            break;
    }


    reactor->Finish(grpc::Status(grpc::StatusCode::INTERNAL, ""));
    return reactor;
}

