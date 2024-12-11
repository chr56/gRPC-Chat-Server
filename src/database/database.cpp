#include "database.h"
#include <algorithm>

Database::Database() {
    setup_test_data();
}

Database::~Database() = default;

std::list<api::chat::User> Database::get_all_users() {
    std::list<api::chat::User> users;
    for (const auto &pair: _users) users.push_back(pair.second);
    return users;
}

std::optional<api::chat::User> Database::get_user_by_id(uint64_t user_id) {
    auto it = _users.find(user_id);
    if (it != _users.end()) return it->second;
    return std::nullopt;
}

std::optional<api::chat::User> Database::get_user_by_name(const std::string &user_name) {
    for (const auto &[id, user]: _users) {
        if (user.name() == user_name) return user;
    }
    return std::nullopt;
}

bool Database::check_user_credentials(std::string &user_name, std::string &password) {
    for (const auto &pair: _user_credentials) {
        if (pair.second.name() == user_name && pair.second.password() == password) return true;
    }
    return false;
}

std::optional<api::chat::User> Database::valid_user_credentials(std::string &user_name, std::string &password) {
    for (const auto &pair: _user_credentials) {
        if (pair.second.name() == user_name && pair.second.password() == password) {
            return get_user_by_id(pair.second.id());
        }
    }
    return std::nullopt;
}

std::list<api::chat::User> Database::get_user_friends(uint64_t user_id) {
    std::list<api::chat::User> friends;
    for (const auto &[linkage, value]: _user_relationship) {
        if (linkage.first == user_id && value) {
            if (auto friend_user = get_user_by_id(linkage.second)) {
                friends.push_back(*friend_user);
            }
        }
    }
    return friends;
}

bool Database::make_friend(uint64_t user_id, uint64_t friend_id) {
    _user_relationship[{user_id, friend_id}] = true;
    _user_relationship[{friend_id, user_id}] = true;
    return true;
}

bool Database::remove_friend(uint64_t user_id, uint64_t friend_id) {
    _user_relationship[{user_id, friend_id}] = false;
    _user_relationship[{friend_id, user_id}] = false;
    return true;
}

std::optional<api::chat::User> Database::add_new_user(std::string user_name, std::string password) {
    auto id = user_id_incremental++;

    api::chat::User user;
    user.set_id(id);
    user.set_name(user_name);
    _users[id] = user;

    api::chat::UserCredentials credentials;
    credentials.set_id(id);
    credentials.set_name(user_name);
    credentials.set_password(password);
    _user_credentials[id] = credentials;

    return {user};
}

bool Database::rename_user(uint64_t user_id, std::string new_user_name) {
    const auto &target = get_user_by_id(user_id);
    if (target.has_value()) {
        auto user = target.value();
        user.set_name(new_user_name);
        _user_credentials[user_id].set_name(new_user_name);
        return true;
    }
    return false;
}

bool Database::change_password(uint64_t user_id, std::string new_password) {
    const auto &target = get_user_by_id(user_id);
    if (target.has_value()) {
        _user_credentials[user_id].set_password(new_password);
        return true;
    }
    return false;
}

bool Database::delete_user(uint64_t user_id) {
    const auto &target = get_user_by_id(user_id);
    if (target.has_value()) {
        _users.erase(user_id);
        _user_credentials.erase(user_id);
        return true;
    }
    return false;
}


//</editor-fold>

//<editor-fold  desc="Chat">

std::list<api::chat::Chat> Database::get_all_chats() {
    std::list<api::chat::Chat> chats;
    for (const auto &pair: _chats) chats.push_back(pair.second);
    return chats;
}

std::optional<api::chat::Chat> Database::get_chat_by_id(uint64_t chat_id) {
    auto it = _chats.find(chat_id);
    if (it != _chats.end()) return it->second;
    return std::nullopt;
}

std::list<api::chat::Chat> Database::get_all_chats_for_user(uint64_t user_id) {
    std::list<api::chat::Chat> groups;
    for (const auto &[id, chat]: _chats) {
        for (const auto &member: chat.members().users()) {
            if (member.id() == user_id) {
                groups.push_back(chat);
                break;
            }
        }
    }
    return groups;
}

std::list<api::chat::Chat> Database::get_all_group_chats_for_user(uint64_t user_id) {
    std::list<api::chat::Chat> groups;
    for (const auto &[id, chat]: _chats) {
        if (chat.is_group()) {
            for (const auto &member: chat.members().users()) {
                if (member.id() == user_id) {
                    groups.push_back(chat);
                    break;
                }
            }
        }
    }
    return groups;
}

std::list<api::chat::Chat> Database::get_all_private_chats_for_user(uint64_t user_id) {
    std::list<api::chat::Chat> privates;
    for (const auto &[id, chat]: _chats) {
        if (!chat.is_group()) {
            for (const auto &member: chat.members().users()) {
                if (member.id() == user_id) {
                    privates.push_back(chat);
                    break;
                }
            }
        }
    }
    return privates;
}

std::optional<api::chat::Chat> Database::get_private_chat(uint64_t user1_id, uint64_t user2_id) {
    for (const auto &[id, chat]: _chats) {
        if (!chat.is_group()) {
            const auto &members = chat.members().users();
            if (members.size() == 2 &&
                ((members[0].id() == user1_id && members[1].id() == user2_id) ||
                 (members[0].id() == user2_id && members[1].id() == user1_id))) {
                return chat;
            }
        }
    }
    return std::nullopt;
}

bool Database::add_member(uint64_t chat_id, uint64_t user_id) {
    auto chat = _chats.find(chat_id);
    if (chat != _chats.end()) {
        auto *members = chat->second.mutable_members()->mutable_users();
        if (std::none_of(members->begin(), members->end(), [user_id](const api::chat::User &user) { return user.id() == user_id; })) {
            if (auto user = get_user_by_id(user_id)) {
                members->Add()->CopyFrom(*user);
                return true;
            }
        } else {
            absl::PrintF("Database: user %ul is already in chat %ul", user_id, chat_id);
            return true;
        }
    }
    return false;
}

bool Database::remove_member(uint64_t chat_id, uint64_t user_id) {
    auto chat = _chats.find(chat_id);
    if (chat != _chats.end()) {
        auto *members = chat->second.mutable_members()->mutable_users();
        auto new_end = std::remove_if(members->begin(), members->end(), [user_id](const api::chat::User &user) { return user.id() == user_id; });
        members->erase(new_end, members->end());
        return true;
    }
    return false;
}

uint64_t Database::create_chat_and_messages(std::string name, bool is_group) {
    uint64_t id = chat_id_incremental++;
    api::chat::Chat chat;
    chat.set_id(id);
    chat.set_name(name);
    chat.set_is_group(is_group);
    _chats[id] = chat;
    api::chat::MessageList messages;
    messages.set_chat_id(id);
    _messages[id] = messages;
    return id;
}

bool Database::delete_chat_and_messages(uint64_t chat_id) {
    _chats.erase(chat_id);
    _messages.erase(chat_id);
    return true;
}

std::optional<api::chat::MessageList *> Database::get_messages_by_id(uint64_t chat_id) {
    auto it = _messages.find(chat_id);
    if (it != _messages.end()) return &it->second;
    return std::nullopt;
}

void Database::setup_test_data() {

    // User

    for (uint64_t i = 1; i <= 12; ++i) {
        add_new_user("user" + std::to_string(i), "qwerty");
    }
    const auto &all_users = get_all_users();


    // Group

    uint64_t public_chat_id = create_chat_and_messages("Public", true);
    for (const auto &user: all_users) {
        add_member(public_chat_id, user.id());
    }

    for (uint64_t i = 1; i <= 5; ++i) {
        std::string name = "Chat " + std::to_string(i);
        uint64_t chat_id = create_chat_and_messages(name, true);

        for (const auto &user: all_users) {
            if ((user.id() % 2) != (chat_id % 3)) add_member(chat_id, user.id());
        }
    }

    // Friends
    for (const auto &user: all_users) {
        for (const auto &friend_user: all_users) {
            if ((user.id() % 3) == (friend_user.id() % 2)) make_friend(user.id(), friend_user.id());
        }
    }
}