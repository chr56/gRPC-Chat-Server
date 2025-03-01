#pragma once
#ifndef CHAT_API_H
#define CHAT_API_H

#include <list>

#include <grpc++/grpc++.h>

#include "api_chat.pb.h"
#include "api_chat.grpc.pb.h"

#include "../database/database.h"


class ChatApiService final : public api::chat::ChatService::CallbackService {
public:
    explicit ChatApiService(const Database &database) :
            db(database), _clients() {}

    grpc::ServerUnaryReactor *
    Login(
            grpc::CallbackServerContext *context,
            const api::chat::UserCredentials *credentials, api::chat::LoginResult *result
    ) override;

    grpc::ServerUnaryReactor *
    Register(
            grpc::CallbackServerContext *context,
            const api::chat::UserCredentials *credentials, api::chat::LoginResult *result
    ) override;

    grpc::ServerUnaryReactor *
    FetchChatList(
            grpc::CallbackServerContext *context,
            const api::chat::None *none, api::chat::ChatList *list
    ) override;

    grpc::ServerWriteReactor<api::chat::MessageList> *
    FetchMessageList(
            grpc::CallbackServerContext *context,
            const api::chat::FetchRequest *request
    ) override;

    grpc::ServerUnaryReactor *
    FetchChatMemberList(
            grpc::CallbackServerContext *context,
            const api::chat::FetchRequest *request, api::chat::UserList *list
    ) override;

    grpc::ServerUnaryReactor *
    SendMessageTo(
            grpc::CallbackServerContext *context,
            const api::chat::SendMessageRequest *request, api::chat::None *none
    ) override;

    grpc::ServerUnaryReactor *
    CreateChat(
            grpc::CallbackServerContext *context,
            const api::chat::CreateChatRequest *request, api::chat::None *none
    ) override;

    grpc::ServerUnaryReactor *
    DeleteChat(
            grpc::CallbackServerContext *context,
            const api::chat::DeleteChatRequest *request, api::chat::None *none
    ) override;


    grpc::ServerUnaryReactor *
    ManageGroupMember(
            grpc::CallbackServerContext *context,
            const api::chat::GroupMemberManageOperation *operation, api::chat::None *none
    ) override;


    grpc::ServerUnaryReactor *
    FetchFriendList(
            grpc::CallbackServerContext *context,
            const api::chat::None *none, api::chat::UserList *list
    ) override;

    grpc::ServerUnaryReactor *
    ManageFriend(
            grpc::CallbackServerContext *context,
            const api::chat::FriendManageOperation *operation, api::chat::None *none
    ) override;

    grpc::ServerUnaryReactor *
    ManageUserInfo(
            grpc::CallbackServerContext *context,
            const api::chat::ManageUserInfoOperation *operation, api::chat::None *none
    ) override;

private:

    class Client {
    public:
        virtual void NotifyNewMessage(uint64_t chat_id, const api::chat::Message &message) = 0;
        virtual void NotifyNewPrivateMessage(uint64_t user_id, const api::chat::Message &message) = 0;
    };

    void notifyClients(uint64_t target, bool is_user, const api::chat::Message &message);

    Database db;
    std::list<Client *> _clients;

    class MessageStreamReactor;

    class ChatListReactor;

    typedef std::multimap<grpc::string_ref, grpc::string_ref> Metadata;
    std::optional<api::chat::User> valid_user_credentials(Metadata &metadata);

    friend MessageStreamReactor;
};


#endif //CHAT_API_H
