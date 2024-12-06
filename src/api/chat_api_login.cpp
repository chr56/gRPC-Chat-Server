#include <iostream>

#include "absl/strings/str_format.h"

#include "chat_api.h"

using namespace api::chat;

grpc::ServerUnaryReactor *
ChatApiService::Login(grpc::CallbackServerContext *context, const UserCredentials *credentials, LoginResult *result) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto name = userManager.check_user_credentials(metadata);
    if (!name) {
        absl::PrintF("Illegal user tried to login!\n");
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    } else {
        std::string &user_name = name.value();
        if (auto user = userManager.get_user_by_name(user_name)) {
            auto id = user.value()->id();
            result->set_user_id(id);
            absl::PrintF("User %s (%u) logged in\n", user_name, id);
        } else {
            result->set_user_id(0);
            absl::PrintF("Could not find user %s? \n", user_name);
        }
        reactor->Finish(grpc::Status::OK);
        return reactor;
    }
}