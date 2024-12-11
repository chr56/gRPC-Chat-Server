
#include "chat_api.h"

using namespace api::chat;


grpc::ServerUnaryReactor *
ChatApiService::CreateChat(grpc::CallbackServerContext *context, const CreateChatRequest *request, None *none) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto user = valid_user_credentials(metadata);
    if (!user) {
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }


    uint64_t chat_id = db.create_chat_and_messages(request->name(), true);
    db.add_member(chat_id, user->id());

    reactor->Finish(grpc::Status::OK);
    return reactor;
}

grpc::ServerUnaryReactor *
ChatApiService::DeleteChat(grpc::CallbackServerContext *context, const DeleteChatRequest *request, None *none) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto user = valid_user_credentials(metadata);
    if (!user) {
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    if (db.delete_chat_and_messages(request->target_chat_id())) {
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
    auto user = valid_user_credentials(metadata);
    if (!user) {
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    // Chat
    uint64_t chat_id = operation->target_chat_id();
    const auto &founded_chat = db.get_chat_by_id(chat_id);
    if (!founded_chat) {
        reactor->Finish(grpc::Status(grpc::StatusCode::NOT_FOUND, absl::StrFormat("Chat %ul not found!", chat_id)));
        return reactor;
    }

    // Member
    uint64_t member_id = operation->target_user_id();
    const auto &founded_member = db.get_user_by_id(member_id);
    if (!founded_member) {
        reactor->Finish(grpc::Status(grpc::StatusCode::NOT_FOUND, absl::StrFormat("User %ul not found!", member_id)));
        return reactor;
    }

    // Action
    auto chat = founded_chat.value();
    auto member = founded_member.value();
    GroupMemberManageOperation::ActionType action = operation->action();
    if (action == GroupMemberManageOperation::AddUser) {
        db.add_member(chat_id, member_id);
    } else {
        db.remove_member(chat_id, member_id);
    }

    reactor->Finish(grpc::Status::OK);
    return reactor;
}

