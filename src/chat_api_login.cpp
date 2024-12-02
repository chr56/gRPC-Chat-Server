#include <iostream>
#include "chat_api.h"

using namespace api::chat;

grpc::ServerUnaryReactor *
ChatApiService::Login(grpc::CallbackServerContext *context, const User *user, None *none) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto name = authenticator.check_user_credentials(metadata);
    if (!name) {
        std::cout << "Illegal user tried to login!\n";
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    } else {
        std::cout << "User " << name.value() << "longed in\n";
        reactor->Finish(grpc::Status::OK);
        return reactor;
    }
}