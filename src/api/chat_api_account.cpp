#include <iostream>

#include "absl/strings/str_format.h"

#include "chat_api.h"

using namespace api::chat;

constexpr std::string_view field_id("user-id");
constexpr std::string_view field_name("user-name");
constexpr std::string_view field_password("user-password");

std::optional<api::chat::User> ChatApiService::valid_user_credentials(Metadata &metadata) {
    uint64_t id = 0;
    std::string password;
    for (const auto &[key, value]: std::as_const(metadata)) {
        if (std::string(key.data(), key.size()) == field_id) {
            std::string id_string = std::string(value.data(), value.size());
            try {
                id = std::stoull(id_string);
            } catch (...) {
                absl::PrintF("Metadata is corrupted (user-id: %s)! \n", id_string);
                return std::nullopt;
            }
        }
        if (std::string(key.data(), key.size()) == field_password) {
            password = std::string(value.data(), value.size());
        }
    }
    auto user = db.valid_user_credentials(id, password);
    return user;
}

grpc::ServerUnaryReactor *
ChatApiService::Login(grpc::CallbackServerContext *context, const UserCredentials *credentials, LoginResult *result) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto user = valid_user_credentials(metadata);
    if (!user) {
        absl::PrintF("An Illegal user tried to login!\n");
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    } else {
        result->set_user_id(user.value().id());
        absl::PrintF("User %s (%u) logged in\n", user.value().name(), user.value().id());
        reactor->Finish(grpc::Status::OK);
        return reactor;
    }
}

grpc::ServerUnaryReactor *
ChatApiService::Register(grpc::CallbackServerContext *context, const UserCredentials *credentials, LoginResult *result) {
    auto reactor = context->DefaultReactor();

    auto &name = credentials->name();
    auto &password = credentials->password();

    auto user = db.add_new_user(name, password);
    if (user.has_value()) {
        result->set_name(user->name());
        result->set_user_id(user.value().id());
        reactor->Finish(grpc::Status::OK);
        absl::PrintF("User %s registered! \n", name);
        return reactor;
    } else {
        const std::string message = absl::StrFormat("Could not register user with name %s! \n", name);
        result->set_user_id(0);
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAVAILABLE, message));
        std::cout << message;
        return reactor;
    }
}

grpc::ServerUnaryReactor *
ChatApiService::ManageUserInfo(grpc::CallbackServerContext *context, const ManageUserInfoOperation *operation, None *none) {
    auto reactor = context->DefaultReactor();

    // Authenticate user
    auto metadata = context->client_metadata();
    auto user = valid_user_credentials(metadata);
    if (!user) {
        reactor->Finish(grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Invalid credentials"));
        return reactor;
    }

    ManageUserInfoOperation::ActionType action = operation->action();
    switch (action) {
        case ManageUserInfoOperation::UpdateUserName:
            if (db.rename_user(user->id(), operation->data())) {
                reactor->Finish(grpc::Status::OK);
            } else {
                reactor->Finish(grpc::Status(grpc::StatusCode::INTERNAL, "Failed!"));
            }
            break;
        case ManageUserInfoOperation::UpdatePasswords:
            if (db.change_password(user->id(), operation->data())) {
                reactor->Finish(grpc::Status::OK);
            } else {
                reactor->Finish(grpc::Status(grpc::StatusCode::INTERNAL, "Failed!"));
            }
            break;
        case ManageUserInfoOperation::DeleteAccount:
            if (db.delete_user(user->id())) {
                reactor->Finish(grpc::Status::OK);
            } else {
                reactor->Finish(grpc::Status(grpc::StatusCode::INTERNAL, "Failed!"));
            }
            break;
        default:
            reactor->Finish(grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Unknown command!"));
            break;
    }

    return reactor;
}