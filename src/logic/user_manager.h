#ifndef SERVER_USER_MANAGER_H
#define SERVER_USER_MANAGER_H

#include <string_view>
#include <list>
#include <map>
#include <optional>

#include <grpc++/grpc++.h>

#include "api_chat.pb.h"

#include "../database/database.h"

class UserManager {
public:

    UserManager(const Database &database);

    ~UserManager();


    std::list<api::chat::User *>
    list_all_users();

    std::optional<api::chat::User *>
    get_user_by_id(uint64_t id);

    std::optional<api::chat::User *>
    get_user_by_name(std::string_view name);

    bool set_user_relationship(uint64_t user_id, uint64_t friend_id, bool is_friend);

    std::optional<api::chat::User *>
    register_user(const std::string &name, const std::string &password);

    //region
    typedef std::multimap<grpc::string_ref, grpc::string_ref> Metadata;

    bool check_user_credentials(std::string &name, std::string &password);
    bool check_user_credentials(api::chat::UserCredentials &credentials);

    std::optional<api::chat::User*> check_user_credentials(Metadata &metadata);
    //endregion


private:
    api::chat::UserCredentialsList _userCredentials;
    std::map<uint64_t, api::chat::User> _users; // user_id <-> User
    api::chat::User add_user(int id, const std::string &username, const std::string &password, const std::string &description);

    Database db;
};


#endif //SERVER_USER_MANAGER_H
