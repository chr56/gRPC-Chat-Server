#include <iostream>

#include "absl/strings/str_format.h"

#include "chat_api.h"

using namespace api::chat;

class ChatApiService::ChatListReactor : public grpc::ServerUnaryReactor {
public:
    ChatListReactor(
            grpc::CallbackServerContext *context, ChatList *reply, uint64_t user_id, Database &db
    ) : _context(context) {
        const std::list<Chat> &allChats = db.get_all_group_chats_for_user(user_id);
        for (const auto &chat: allChats) {
            Chat *newChat = pending.add_chats();
            newChat->CopyFrom(chat);
        }
        reply->Swap(&pending);
        Finish(grpc::Status::OK);
    }

    void OnDone() override {
        absl::PrintF("Completed to send Chat list!\n");
        delete this;
    }

    void OnCancel() override {
        absl::PrintF("Error when sending all chats\n");
        delete this;
    }

private:
    grpc::CallbackServerContext *_context;
    ChatList pending;
};


grpc::ServerUnaryReactor *
ChatApiService::FetchChatList(grpc::CallbackServerContext *context, const None *none, ChatList *list) {

    grpc::ServerUnaryReactor *reactor;

    // Authenticate user
    auto metadata = context->client_metadata();
    auto user = valid_user_credentials(metadata);
    if (!user) {
        reactor = context->DefaultReactor();
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }
    absl::PrintF("Sending Chat list to %s \n", user.value().name());

    reactor = new ChatApiService::ChatListReactor(context, list, user.value().id(), db);

    return reactor;
}
