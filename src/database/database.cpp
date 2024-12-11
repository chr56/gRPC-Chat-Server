#include "database.h"

using namespace api::chat;

Database::Database() {

}

Database::~Database() {

}

//<editor-fold desc="User">

std::list<User> Database::get_all_users() {
    return {};
}

std::optional<User> Database::get_user_by_id(uint64_t user_id) {
    return std::nullopt;
}

std::optional<User> Database::get_user_by_name(const std::string &user_name) {
    return std::nullopt;
}

bool Database::check_user_credentials(std::string &user_name, std::string &password) {
    return false;
}

std::optional<api::chat::User> Database::valid_user_credentials(std::string &user_name, std::string &password) {
    return std::nullopt;
}

std::list<User> Database::get_user_friends(uint64_t user_id) {
    return std::list<User>();
}

bool Database::make_friend(uint64_t user_id, uint64_t friend_id) {
    return false;
}

bool Database::remove_friend(uint64_t user_id, uint64_t friend_id) {
    return false;
}

std::optional<api::chat::User> Database::add_new_user(std::string user_name, std::string password) {
    return std::nullopt;
}

bool Database::rename_user(uint64_t user_id, std::string new_user_name) {
    return false;
}


//</editor-fold>

//<editor-fold  desc="Chat">

std::list<api::chat::Chat> Database::get_all_chats() {
    return {};
}

std::optional<api::chat::Chat> Database::get_chat_by_id(uint64_t chat_id) {
    return std::nullopt;
}

std::list<api::chat::Chat> Database::get_all_chats_for_user(uint64_t user_id) {
    return {};
}

std::list<api::chat::Chat> Database::get_all_group_chats_for_user(uint64_t user_id) {
    return {};
}

std::list<api::chat::Chat> Database::get_all_private_chats_for_user(uint64_t user_id) {
    return {};
}

std::optional<api::chat::Chat> Database::get_private_chat(uint64_t user1_id, uint64_t user2_id) {
    return std::nullopt;
}

bool Database::add_member(uint64_t chat_id, uint64_t user_id) {
    return false;
}

bool Database::remove_member(uint64_t chat_id, uint64_t user_id) {
    return false;
}

uint64_t Database::create_chat_and_messages(std::string name, bool is_group) {
    return 0;
}

bool Database::delete_chat_and_messages(uint64_t chat_id) {
    return false;
}

std::optional<api::chat::MessageList *> Database::get_messages_by_id(uint64_t chat_id) {
    return {};
}

//</editor-fold>

void setup_test_data() {

}