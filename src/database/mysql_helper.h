#ifndef SERVER_MYSQL_HELPER_H
#define SERVER_MYSQL_HELPER_H

#include <string>
#include <list>
#include <tuple>
#include <optional>

class MySQLHelper {
private:
    std::string host;
    std::string database_name;
    std::string password;

public:

    // user_id, user_name, user_passwords
    typedef std::tuple<int, std::string, std::string> User;

    // friend_id, name, team
    typedef std::tuple<int, std::string, std::string> Friend;

    // group_id, name
    typedef std::tuple<int, std::string> Group;

    MySQLHelper(std::string host_, std::string database_name_, std::string password_);
    ~MySQLHelper();

    bool update(const std::string &table_name, const std::string &id_name, int id, const std::string &column, const std::string &value);
    uint64_t add_new_user(const std::string &user_name, const std::string &user_password);
    bool delete_user(int user_id);
    bool valid_user_password(int user_id, const std::string &password);
    std::string get_user_name(int id);
    std::list<User> get_all_users();
    std::optional<User> get_user_by_id(int user_id);
    std::optional<User> get_user_by_name(const std::string &name);
    bool make_friend(int user_id, int friend_id, const std::string &team);
    bool remove_friend(int user_id, int friend_id);
    std::list<Friend> get_user_friends(int id);
    uint64_t create_group(const std::string &group_name);
    bool delete_group(int group_id);
    std::list<Group> get_all_groups();
    std::optional<Group> get_group_by_id(int group_id);
    bool add_member(int group_id, int member_id);
    bool remove_member(int group_id, int member_id);
    std::list<Group> get_all_groups_for_user(int user_id);
    std::list<User> get_group_members(int group_id);
};

#endif //SERVER_MYSQL_HELPER_H