#ifndef SERVER_USER_MANAGER_H
#define SERVER_USER_MANAGER_H

#include <string_view>
#include <list>
#include <map>
#include <optional>

#include <grpc++/grpc++.h>

#include <api_chat.pb.h>
#include <api_chat.grpc.pb.h>

class UserManager {
public:

    UserManager();

    ~UserManager();

    std::optional<api::chat::User *>
    get_user_by_id(uint64_t id);

    std::optional<api::chat::User *>
    get_user_by_name(std::string_view name);

    typedef std::multimap<grpc::string_ref, grpc::string_ref> Metadata;

    bool check_user_credentials(std::string &name, std::string &password);
    bool check_user_credentials(api::chat::UserCredentials& credentials);

    std::optional<std::string> check_user_credentials(Metadata &metadata);


private:
    api::chat::UserCredentialsList _userCredentials;
    std::map<uint64_t, api::chat::User> _users; // user_id <-> User
};


#endif //SERVER_USER_MANAGER_H
