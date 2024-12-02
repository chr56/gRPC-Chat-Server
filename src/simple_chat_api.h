//
// Created by Admin on 2024/12/2.
//

#ifndef _SIMPLE_CHAT_API_H
#define _SIMPLE_CHAT_API_H

#include "authenticator.h"
#include "api_chat.grpc.pb.h"
#include "api_chat.pb.h"
#include <grpc++/grpc++.h>
#include <list>

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

#endif //_SIMPLE_CHAT_API_H
