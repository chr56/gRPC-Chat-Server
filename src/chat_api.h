#pragma once
#ifndef CHAT_API_H
#define CHAT_API_H

#include <list>

#include <grpc++/grpc++.h>

#include "api_chat.pb.h"
#include "api_chat.grpc.pb.h"

#include "authenticator.h"
#include "chat_manager.h"


class ChatApiService final : public api::chat::ChatService::CallbackService {
public:
    ChatApiService() : _clients() {}

    grpc::ServerUnaryReactor *
    Login(::grpc::CallbackServerContext *context, const ::api::chat::UserCredentials *credentials, ::api::chat::None *none) override;

    grpc::ServerWriteReactor<api::chat::MessageList> *
    FetchMessageList(::grpc::CallbackServerContext *context, const ::api::chat::FetchMessageListRequest *request) override;

    grpc::ServerUnaryReactor *
    SendMessageTo(::grpc::CallbackServerContext *context, const ::api::chat::SendMessageRequest *request, ::api::chat::None *none) override;



private:

    class Client {
    public:
        virtual void NotifyNewMessage(const api::chat::Message &message) = 0;
    };

    void notifyClients(uint64_t dialogId, const api::chat::Message &message);

    Authenticator authenticator;
    ChatManager chatManager;
    std::list<Client *> _clients;

    class MessageStreamReactor;
};


#endif //CHAT_API_H
