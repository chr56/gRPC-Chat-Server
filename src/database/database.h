#ifndef SERVER_DATABASE_H
#define SERVER_DATABASE_H

#include <list>
#include <string>
#include <optional>


#include "api_chat.pb.h"

class Database {
public:
    Database();

    ~Database();



    //<editor-fold desc="User">

    std::list<api::chat::User> get_all_users();

    std::optional<api::chat::User> get_user_by_id(uint64_t user_id);

    std::optional<api::chat::User> get_user_by_name(const std::string& user_name);

    bool check_user_credentials(std::string &user_name, std::string &password);

    std::optional<api::chat::User> valid_user_credentials(std::string &user_name, std::string &password);

    std::list<api::chat::User> get_user_friends(uint64_t user_id);

    bool make_friend(uint64_t user_id, uint64_t friend_id);

    bool remove_friend(uint64_t user_id, uint64_t friend_id);

    std::optional<api::chat::User> add_new_user(std::string user_name, std::string password);

    bool rename_user(uint64_t user_id, std::string new_user_name);

    bool change_password(uint64_t user_id, std::string new_password);

    bool delete_user(uint64_t user_id);


    //</editor-fold>


    //<editor-fold desc="Chat">

    std::list<api::chat::Chat> get_all_chats();

    std::optional<api::chat::Chat> get_chat_by_id(uint64_t chat_id);

    std::list<api::chat::Chat> get_all_chats_for_user(uint64_t user_id);

    std::list<api::chat::Chat> get_all_group_chats_for_user(uint64_t user_id);

    std::list<api::chat::Chat> get_all_private_chats_for_user(uint64_t user_id);

    std::optional<api::chat::Chat> get_private_chat(uint64_t user1_id, uint64_t user2_id);

    std::optional<api::chat::MessageList *> get_messages_by_id(uint64_t chat_id);

    uint64_t create_chat_and_messages(std::string name, bool is_group);

    bool delete_chat_and_messages(uint64_t chat_id);

    bool add_member(uint64_t chat_id, uint64_t user_id);

    bool remove_member(uint64_t chat_id, uint64_t user_id);

    //</editor-fold>

private:

    std::map<uint64_t, api::chat::Chat> _chats;
    std::map<uint64_t, api::chat::MessageList> _messages;

    std::map<uint64_t, api::chat::User> _users;
    std::map<uint64_t, api::chat::UserCredentials> _user_credentials;

    std::map<std::pair<uint64_t, uint64_t>, bool> _user_relationship;

    void setup_test_data();

    uint64_t user_id_incremental = 10001;
    uint64_t chat_id_incremental = 10;
};


#endif //SERVER_DATABASE_H
