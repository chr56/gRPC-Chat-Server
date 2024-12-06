
#include "chat_api.h"

using namespace api::chat;


grpc::ServerUnaryReactor *
ChatApiService::CreateChat(grpc::CallbackServerContext *context, const CreateChatRequest *request, None *none) {
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
ChatApiService::DeleteChat(grpc::CallbackServerContext *context, const DeleteChatRequest *request, None *none) {
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
ChatApiService::ManageGroupMember(grpc::CallbackServerContext *context, const GroupMemberManageOperation *operation, None *none) {
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

