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

    reactor->Finish(grpc::Status(grpc::StatusCode::UNIMPLEMENTED, ""));
    return reactor;
}

