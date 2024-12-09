#include "user_manager.h"

using namespace api::chat;

std::list<User *>
UserManager::list_all_users() {
    std::list<User *> users(_users.size());
    for (const auto &pair: _users) {
        auto user = pair.second;
        users.push_back(&user);
    }
    return users;
}


std::optional<api::chat::User *>
UserManager::get_user_by_id(uint64_t id) {
    auto it = _users.find(id);
    if (it != _users.end()) {
        return &(it->second);
    } else {
        return std::nullopt;
    }
}

std::optional<api::chat::User *>
UserManager::get_user_by_name(std::string_view name) {
    auto it = std::find_if(
            _users.begin(), _users.end(),
            [&name](const std::pair<uint64_t, api::chat::User> &user_pair) {
                return user_pair.second.name() == name;  // Check if name matches
            });
    if (it != _users.end()) {
        return &it->second;
    } else {
        return std::nullopt;
    }
}

int user_id = 101;

std::optional<api::chat::User *>
UserManager::register_user(const std::string &name, const std::string &password) {
    if (get_user_by_name(name).has_value()) {
        return std::nullopt; // existed
    } else {
        User user = add_user(user_id++, name, password, name);
        return std::optional{&user};
    }
}


bool UserManager::set_user_relationship(uint64_t user_id, uint64_t friend_id, bool is_friend) {
    return false; // todo
}

UserManager::UserManager(const Database &database) : db(database) {
    using namespace api::chat;
    for (int i = 1; i < 10; ++i) {
        auto username = std::string("user") + std::to_string(i);
        auto description = std::string("Test user");
        auto password = std::string("qwerty");
        add_user(i, username, password, description);
    }
}

User UserManager::add_user(int id, const std::string &username, const std::string &password, const std::string &description) {
    UserCredentials *credentials = _userCredentials.add_users();
    credentials->set_id(id);
    credentials->set_name(username);
    credentials->set_password(password);

    User user;
    user.set_id(id);
    user.set_name(username);
    user.set_description(description);

    _users[id] = user;

    return user;
}

UserManager::~UserManager() {
    _userCredentials.clear_users();
    _users.clear();
}

constexpr std::string_view field_name("user-name");
constexpr std::string_view field_password("user-password");

bool UserManager::check_user_credentials(std::string &name, std::string &password) {
    auto matched_result = std::find_if(
            _userCredentials.users().begin(), _userCredentials.users().end(),
            [&name, &password](const auto &it) {
                return it.name() == name && it.password() == password;
            });

    bool matched = matched_result != _userCredentials.users().end();

    return matched;
}


bool UserManager::check_user_credentials(api::chat::UserCredentials &credentials) {
    std::string name = credentials.name();
    std::string password = credentials.password();
    return check_user_credentials(name, password);
}

std::optional<std::string>
UserManager::check_user_credentials(UserManager::Metadata &metadata) {
    std::string name;
    std::string password;
    for (const auto &[key, value]: std::as_const(metadata)) {
        if (std::string(key.data(), key.size()) == field_name) {
            name = std::string(value.data(), value.size());
        }
        if (std::string(key.data(), key.size()) == field_password) {
            password = std::string(value.data(), value.size());
        }
    }
    auto success = check_user_credentials(name, password);

    return success ? std::optional{name} : std::nullopt;
}
