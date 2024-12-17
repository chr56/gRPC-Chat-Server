#include "database.h"
#include <algorithm>

Database::Database() {
    try {
        helper = new MySQLHelper("127.0.0.1", "chat", "qwerty");
        // setup_test_data();
    } catch (...) {
        absl::PrintF("Failed to connect database!");
        exit(1);
    }
}

Database::~Database() = default;

std::list<api::chat::User> Database::get_all_users() {
    std::list<api::chat::User> users;
    const auto &all = helper->get_all_users();
    for (const auto &item: all) {
        const std::optional<api::chat::User> &user = convert_user(item);
        if (user.has_value()) {
            users.push_back(user.value());
        }
    }
    return users;
}

std::optional<api::chat::User> Database::get_user_by_id(uint64_t user_id) {
    const auto &user = helper->get_user_by_id(user_id);
    return convert_user(user);
}

std::optional<api::chat::User> Database::get_user_by_name(const std::string &user_name) {
    const auto &user = helper->get_user_by_name(user_name);
    return convert_user(user);
}

bool Database::check_user_credentials(uint64_t user_id, std::string &password) {
    return helper->valid_user_password(user_id, password);
}

std::optional<api::chat::User> Database::valid_user_credentials(uint64_t user_id, std::string &password) {
    if (helper->valid_user_password(user_id, password)) {
        return get_user_by_id(user_id);
    } else {
        return std::nullopt;
    }
}

std::list<api::chat::User> Database::get_user_friends(uint64_t user_id) {
    std::list<api::chat::User> users;
    const auto &friends = helper->get_user_friends(user_id);;
    for (const auto &friend_user: friends) {
        const std::optional<api::chat::User> &user = convert_friend(friend_user);
        if (user.has_value()) {
            users.push_back(user.value());
        }
    }
    return users;
}

bool Database::make_friend(uint64_t user_id, uint64_t friend_id) {
    return helper->make_friend(user_id, friend_id, "friend");
}

bool Database::remove_friend(uint64_t user_id, uint64_t friend_id) {
    return helper->remove_friend(user_id, friend_id);
}

std::optional<api::chat::User> Database::add_new_user(std::string user_name, std::string password) {
    int id = helper->add_new_user(user_name, password);
    if (id > 0) {
        return get_user_by_id(id);
    } else {
        return std::nullopt;
    }
}

bool Database::rename_user(uint64_t user_id, std::string new_user_name) {
    return helper->update("user_info", "user_id", user_id, "user_name", new_user_name);
}

bool Database::change_password(uint64_t user_id, std::string new_password) {
    return helper->update("user_info", "user_id", user_id, "user_password", new_password);
}

bool Database::delete_user(uint64_t user_id) {
    return helper->delete_user(user_id);
}


//</editor-fold>

//<editor-fold  desc="Chat">

std::optional<api::chat::Chat> Database::get_group_chat_by_id(uint64_t chat_id) {
    const auto &group = helper->get_group_by_id(chat_id);
    return convert_group(group);
}

std::list<api::chat::Chat> Database::get_all_group_chats_for_user(uint64_t user_id) {
    std::list<api::chat::Chat> groups;
    const auto &all = helper->get_all_groups_for_user(user_id);
    for (const auto &group: all) {
        auto chat = convert_group(group);
        if (chat.has_value()) {
            groups.push_back(chat.value());
        }
    }
    return groups;
}

std::optional<api::chat::MessageList *> Database::get_group_messages_by_id(uint64_t chat_id) {
    auto it = _group_messages.find(chat_id);
    if (it != _group_messages.end()) return &it->second;
    api::chat::MessageList messages;
    messages.set_chat_id(chat_id);
    _group_messages[chat_id] = messages;
    return &messages;
}

std::optional<api::chat::MessageList *> Database::get_private_messages_by_id(uint64_t user1_id, uint64_t user2_id) {
    auto it = _private_messages.find({user1_id, user2_id});
    if (it != _private_messages.end()) return &it->second;
    create_private_chat_and_messages(user1_id, user2_id);
    return &_private_messages[{user1_id, user2_id}];
}

uint64_t Database::create_group_chat_and_messages(std::string name) {
    uint64_t id = helper->create_group(name);
    if (id > 0) {
        api::chat::MessageList messages;
        messages.set_chat_id(id);
        _group_messages[id] = messages;
        return id;
    } else {
        return 0;
    }
}

bool Database::delete_group_chat_and_messages(uint64_t chat_id) {
    _group_messages.erase(chat_id);
    return true;
}

bool Database::create_private_chat_and_messages(uint64_t user1_id, uint64_t user2_id) {
    api::chat::MessageList messages;
    messages.set_chat_id(0);
    _private_messages[{user1_id, user2_id}] = messages;
    return true;
}

bool Database::delete_private_chat_and_messages(uint64_t user1_id, uint64_t user2_id) {
    _private_messages.erase({user1_id, user2_id});
    return true;
}


std::list<api::chat::User> Database::get_chat_members(uint64_t chat_id) {
    std::list<api::chat::User> members;
    const auto &all = helper->get_group_members(chat_id);
    for (const auto &item: all) {
        const std::optional<api::chat::User> &user = convert_user(item);
        if (user.has_value()) {
            members.push_back(user.value());
        }
    }
    return members;
}

bool Database::add_member(uint64_t chat_id, uint64_t user_id) {
    return helper->add_member(chat_id, user_id);
}

bool Database::remove_member(uint64_t chat_id, uint64_t user_id) {
    return helper->remove_member(chat_id, user_id);
}

void Database::setup_test_data() {

    // User

    for (uint64_t i = 1; i <= 12; ++i) {
        add_new_user("user" + std::to_string(i), "qwerty");
    }
    const auto &all_users = get_all_users();


    // Group

    uint64_t public_chat_id = create_group_chat_and_messages("Public");
    for (const auto &user: all_users) {
        add_member(public_chat_id, user.id());
    }

    for (uint64_t i = 1; i <= 5; ++i) {
        std::string name = "Chat " + std::to_string(i);
        uint64_t chat_id = create_group_chat_and_messages(name);

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

std::optional<api::chat::User> Database::convert_user(const MySQLHelper::User &user_tuple) {
    try {
        auto id = std::get<0>(user_tuple);
        auto name = std::get<1>(user_tuple);

        api::chat::User user;
        user.set_id(id);
        user.set_name(name);
        return user;

    } catch (...) {
        absl::PrintF("Failed to convert user from database!\n");
        return std::nullopt;
    }
}

std::optional<api::chat::User> Database::convert_user(const std::optional<MySQLHelper::User> &user_tuple) {
    if (user_tuple) {
        return convert_user(user_tuple.value());
    } else {
        return std::nullopt;
    }
}

std::optional<api::chat::User> Database::convert_friend(const MySQLHelper::Friend &friend_tuple) {
    try {
        auto id = std::get<0>(friend_tuple);
        auto name = std::get<1>(friend_tuple);
        auto catalog = std::get<2>(friend_tuple);

        api::chat::User user;
        user.set_id(id);
        user.set_name(name);
        user.set_description(catalog);
        return user;

    } catch (...) {
        absl::PrintF("Failed to convert user from database!\n");
        return std::nullopt;
    }
}

std::optional<api::chat::User> Database::convert_friend(const std::optional<MySQLHelper::Friend> &friend_tuple) {
    if (friend_tuple) {
        return convert_friend(friend_tuple.value());
    } else {
        return std::nullopt;
    }
}

std::optional<api::chat::Chat> Database::convert_group(const MySQLHelper::Group &group_tuple) {
    try {
        auto id = std::get<0>(group_tuple);
        auto name = std::get<1>(group_tuple);

        api::chat::Chat chat;
        chat.set_id(id);
        chat.set_name(name);
        return chat;

    } catch (...) {
        absl::PrintF("Failed to convert group from database!\n");
        return std::nullopt;
    }
}

std::optional<api::chat::Chat> Database::convert_group(const std::optional<MySQLHelper::Group> &group_tuple) {
    if (group_tuple) {
        return convert_group(group_tuple.value());
    } else {
        return std::nullopt;
    }
}