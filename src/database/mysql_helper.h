#ifndef SERVER_MYSQL_HELPER_H
#define SERVER_MYSQL_HELPER_H

#include <string>
#include <list>
#include <tuple>
#include <optional>

#include <mysqlx/xdevapi.h>

class MySQLHelper {
private:
    mysqlx::Session session;
    mysqlx::Schema database;

public:

    // user_id, user_name, user_passwords
    typedef std::tuple<int, std::string, std::string> User;

    // friend_id, name, team
    typedef std::tuple<int, std::string, std::string> Friend;

    // group_id, name
    typedef std::tuple<int, std::string> Group;

    MySQLHelper(const char *host, const char *database_name, const char *password);
    ~MySQLHelper();

    bool update(std::string table_name, std::string id_name, int id, std::string column, std::string value);
    int add_new_user(std::string user_name, std::string user_password);
    bool delete_user(int user_id);
    bool valid_user_password(int user_id, std::string password);
    std::string get_user_name(int id);
    std::list<User> get_all_users();
    std::optional<User> get_user_by_id(int user_id);
    std::optional<User> get_user_by_name(std::string name);
    bool make_friend(int user_id, int friend_id, std::string team);
    bool remove_friend(int user_id, int friend_id);
    std::list<Friend> get_user_friends(int id);
    int create_group(std::string group_name);
    bool delete_group(int group_id);
    std::list<Group> get_all_groups();
    std::optional<Group> get_group_by_id(int group_id);
    bool add_member(int group_id, int member_id);
    bool remove_member(int group_id, int member_id);
    std::list<Group> get_all_groups_for_user(int user_id);
    std::list<User> get_group_members(int group_id);
};

#undef INTERNAL

#endif //SERVER_MYSQL_HELPER_H