#pragma once
#ifndef CHAT_API_H
#define CHAT_API_H

#include <list>

#include <grpc++/grpc++.h>

#include "api_chat.pb.h"
#include "api_chat.grpc.pb.h"

#include "authenticator.h"


class SimpleChatApiService final : public api::chat::SimpleChat::CallbackService {
public:
    SimpleChatApiService() : _messages(), _clients() {}

    grpc::ServerUnaryReactor *SendMessage(
            grpc::CallbackServerContext *context, const api::chat::ChatMessage *request, api::chat::None *response) override;

    grpc::ServerWriteReactor<api::chat::ChatMessages> *FetchMessageList(
            grpc::CallbackServerContext *context, const api::chat::None *request) override;


private:

    class Client {
    public:
        virtual void NotifyNewMessage(const api::chat::ChatMessage &message) = 0;
    };

    void notifyClients(const api::chat::ChatMessage &message);

    Authenticator authenticator;
    api::chat::ChatMessages _messages;
    std::list<Client *> _clients;

    class MessageStreamReactor;
};

class ChatApiService final : public api::chat::Chat::CallbackService {
public:
    ChatApiService() : _messages(), _clients() {}

    grpc::ServerUnaryReactor *
    Login(::grpc::CallbackServerContext *context, const ::api::chat::User *user, ::api::chat::None *none) override;

    grpc::ServerWriteReactor<api::chat::ChatMessages> *
    FetchMessageList(::grpc::CallbackServerContext *context, const ::api::chat::FetchMessageListRequest *request) override;

    grpc::ServerUnaryReactor *
    SendMessageTo(::grpc::CallbackServerContext *context, const ::api::chat::SendMessageRequest *request,
                  ::api::chat::None *none) override;

private:

    class Client {
    public:
        virtual void NotifyNewMessage(const api::chat::ChatMessage &message) = 0;
    };

    void notifyClients(uint64_t dialogId, const api::chat::ChatMessage &message);

    Authenticator authenticator;
    api::chat::ChatMessages _messages;
    std::list<Client *> _clients;

    class MessageStreamReactor;
};


#endif //CHAT_API_H
