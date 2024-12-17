#ifndef SERVER_DATABASE_H
#define SERVER_DATABASE_H

#include <list>
#include <string>
#include <optional>

#include "api_chat.pb.h"

#include "mysql_helper.h"

class Database {
public:
    Database();

    ~Database();



    //<editor-fold desc="User">

    std::list<api::chat::User> get_all_users();

    std::optional<api::chat::User> get_user_by_id(uint64_t user_id);

    std::optional<api::chat::User> get_user_by_name(const std::string &user_name);

    bool check_user_credentials(uint64_t user_id, std::string &password);

    std::optional<api::chat::User> valid_user_credentials(uint64_t user_id, std::string &password);

    std::list<api::chat::User> get_user_friends(uint64_t user_id);

    bool make_friend(uint64_t user_id, uint64_t friend_id);

    bool remove_friend(uint64_t user_id, uint64_t friend_id);

    std::optional<api::chat::User> add_new_user(std::string user_name, std::string password);

    bool rename_user(uint64_t user_id, std::string new_user_name);

    bool change_password(uint64_t user_id, std::string new_password);

    bool delete_user(uint64_t user_id);


    //</editor-fold>


    //<editor-fold desc="Chat">

    std::optional<api::chat::Chat> get_group_chat_by_id(uint64_t chat_id);

    std::list<api::chat::Chat> get_all_group_chats_for_user(uint64_t user_id);

    std::optional<api::chat::MessageList *> get_group_messages_by_id(uint64_t chat_id);

    std::optional<api::chat::MessageList *> get_private_messages_by_id(uint64_t user1_id, uint64_t user2_id);

    uint64_t create_group_chat_and_messages(std::string name);

    bool delete_group_chat_and_messages(uint64_t chat_id);

    bool create_private_chat_and_messages(uint64_t user1_id, uint64_t user2_id);

    bool delete_private_chat_and_messages(uint64_t user1_id, uint64_t user2_id);

    std::list<api::chat::User> get_chat_members(uint64_t chat_id);

    bool add_member(uint64_t chat_id, uint64_t user_id);

    bool remove_member(uint64_t chat_id, uint64_t user_id);

    //</editor-fold>

private:

    std::map<uint64_t, api::chat::MessageList> _group_messages;
    std::map<std::pair<uint64_t, uint64_t>, api::chat::MessageList> _private_messages;

    MySQLHelper *helper;

    void setup_test_data();


    static std::optional<api::chat::User> convert_user(const MySQLHelper::User &user_tuple);
    static std::optional<api::chat::User> convert_user(const std::optional<MySQLHelper::User> &user_tuple);
    static std::optional<api::chat::User> convert_friend(const MySQLHelper::Friend &friend_tuple);
    static std::optional<api::chat::User> convert_friend(const std::optional<MySQLHelper::Friend> &friend_tuple);
    static std::optional<api::chat::Chat> convert_group(const MySQLHelper::Group &group_tuple);
    static std::optional<api::chat::Chat> convert_group(const std::optional<MySQLHelper::Group> &group_tuple);

};


#endif //SERVER_DATABASE_H
