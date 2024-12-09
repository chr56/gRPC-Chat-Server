
#include "chat_api.h"

using namespace api::chat;


grpc::ServerUnaryReactor *
ChatApiService::CreateChat(grpc::CallbackServerContext *context, const CreateChatRequest *request, None *none) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto user = userManager.check_user_credentials(metadata);
    if (!user) {
        absl::PrintF("Illegal user tried to login!\n");
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }


    chatManager.create_chat_and_messages(request->name(), request->description(), true);

    reactor->Finish(grpc::Status::OK);
    return reactor;
}

grpc::ServerUnaryReactor *
ChatApiService::DeleteChat(grpc::CallbackServerContext *context, const DeleteChatRequest *request, None *none) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto user = userManager.check_user_credentials(metadata);
    if (!user) {
        absl::PrintF("Illegal user tried to login!\n");
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    if (chatManager.delete_chat_and_messages(request->target_chat_id())) {
        reactor->Finish(grpc::Status::OK);
    } else {
        reactor->Finish(grpc::Status(grpc::StatusCode::INTERNAL, "Failed!"));
    }
    return reactor;
}

grpc::ServerUnaryReactor *
ChatApiService::ManageGroupMember(grpc::CallbackServerContext *context, const GroupMemberManageOperation *operation, None *none) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto user = userManager.check_user_credentials(metadata);
    if (!user) {
        absl::PrintF("Illegal user tried to login!\n");
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    // Chat
    uint64_t chat_id = operation->target_chat_id();
    const auto &founded_chat = chatManager.get_chat_by_id(chat_id);
    if (!founded_chat) {
        reactor->Finish(grpc::Status(grpc::StatusCode::NOT_FOUND, absl::StrFormat("Chat %ul not found!", chat_id)));
        return reactor;
    }

    // Member
    uint64_t user_id = operation->target_user_id();
    const auto &founded_member = userManager.get_user_by_id(user_id);
    if (!founded_member) {
        reactor->Finish(grpc::Status(grpc::StatusCode::NOT_FOUND, absl::StrFormat("User %ul not found!", user_id)));
        return reactor;
    }

    // Action
    Chat *chat = founded_chat.value();
    User *member = founded_member.value();
    GroupMemberManageOperation::ActionType action = operation->action();
    if (action == GroupMemberManageOperation::AddUser) {
        chatManager.add_members(chat_id, member);
    } else {
        UserList *members = chat->mutable_members();
        reactor->Finish(grpc::Status(grpc::StatusCode::UNIMPLEMENTED, ""));
    }

    reactor->Finish(grpc::Status::OK);
    return reactor;
}

