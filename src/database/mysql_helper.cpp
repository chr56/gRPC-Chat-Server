/**
 * @author gao66666
 */
#include "mysql_helper.h"


#include <mysqlx/xdevapi.h>
#include <iostream>

using namespace mysqlx;

MySQLHelper::MySQLHelper(std::string host_, std::string database_name_, std::string password_)
        : host(std::move(host_)), database_name(std::move(database_name_)), password(std::move(password_)) {}

MySQLHelper::~MySQLHelper() = default;


bool MySQLHelper::update(const std::string &table_name,
                         const std::string &id_name, int id,
                         const std::string &column, const std::string &value) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable(table_name);
        const Result &result = table.update().set(column, value).where(id_name + " = :id").bind("id", id).execute();
        return result.getAffectedItemsCount() > 0;
    } catch (const std::exception &err) {
        std::cerr << "Error on updating " << table_name << ": " << err.what() << std::endl;
    }
    return false;
}


uint64_t MySQLHelper::add_new_user(const std::string &user_name, const std::string &user_password) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("user_info");
        const Result &result = table.insert("user_name", "user_password", "user_icon_path").values(user_name, user_password, "").execute();
        return result.getAutoIncrementValue();
    } catch (const std::exception &err) {
        std::cerr << "Error adding user: " << err.what() << std::endl;
    }
    return 0;
}

bool MySQLHelper::delete_user(int user_id) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("user_info");
        const Result &result = table.remove().where("user_id = :id").bind("id", user_id).execute();
        return result.getAffectedItemsCount() > 0;
    } catch (const std::exception &err) {
        std::cerr << "Error deleting user: " << err.what() << std::endl;
    }
    return false;
}

bool MySQLHelper::valid_user_password(int user_id, const std::string &password) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("user_info");
        RowResult result =
                table.select("user_id").where("user_id = :id AND user_password = :pwd").bind("id", user_id).bind("pwd", password).execute();
        return result.count() > 0;
    } catch (const std::exception &err) {
        std::cerr << "Error checking user: " << err.what() << std::endl;
    }
    return false;
}

std::string MySQLHelper::get_user_name(int id) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("user_info");
        RowResult result = table.select("user_name").where("user_id = :id").bind("id", id).execute();

        for (Row row: result) {
            return row[0].get<std::string>();
        }
    } catch (const std::exception &err) {
        std::cerr << "Error get username: " << err.what() << std::endl;
    }
    return "";
}

std::list<MySQLHelper::User> MySQLHelper::get_all_users() {
    std::list<User> users;
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("user_info");
        RowResult result = table.select("user_id", "user_name", "user_password").execute();

        for (Row row: result) {
            users.emplace_back(row[0].get<int>(), row[1].get<std::string>(), row[2].get<std::string>());
        }
    } catch (const std::exception &err) {
        std::cerr << "Error get all users: " << err.what() << std::endl;
    }
    return users;
}

std::optional<MySQLHelper::User> MySQLHelper::get_user_by_id(int user_id) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("user_info");
        RowResult result = table.select("user_id", "user_name", "user_password").where("user_id = :id").bind("id", user_id).execute();

        for (Row row: result) {
            return User{row[0].get<int>(), row[1].get<std::string>(), row[2].get<std::string>()};
        }
    } catch (const std::exception &err) {
        std::cerr << "Error getting user by id: " << err.what() << std::endl;
    }
    return std::nullopt;
}

std::optional<MySQLHelper::User> MySQLHelper::get_user_by_name(const std::string &name) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("user_info");
        RowResult result = table.select("user_id", "user_name", "user_password").where("user_name = :name").bind("id", name).execute();

        for (Row row: result) {
            return User{row[0].get<int>(), row[1].get<std::string>(), row[2].get<std::string>()};
        }
    } catch (const std::exception &err) {
        std::cerr << "Error getting user by name: " << err.what() << std::endl;
    }
    return std::nullopt;
}


bool MySQLHelper::make_friend(int user_id, int friend_id, const std::string &team) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("user_friend");
        auto r1 = table.insert("user_id", "friend_id", "team").values(user_id, friend_id, team).execute();
        auto r2 = table.insert("user_id", "friend_id", "team").values(friend_id, user_id, team).execute();
        return r1.getAffectedItemsCount() > 0 && r2.getAutoIncrementValue() > 0;
    } catch (const std::exception &err) {
        std::cerr << "Error making friends: " << err.what() << std::endl;
    }
    return false;
}

bool MySQLHelper::remove_friend(int user_id, int friend_id) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("user_friend");
        auto r1 = table.remove().where("user_id = :id AND friend_id = :fid").bind("id", user_id).bind("fid", friend_id).execute();
        auto r2 = table.remove().where("user_id = :fid AND friend_id = :id").bind("id", user_id).bind("fid", friend_id).execute();
        return r1.getAffectedItemsCount() > 0 && r2.getAutoIncrementValue() > 0;
    } catch (const std::exception &err) {
        std::cerr << "Error removing friends: " << err.what() << std::endl;
    }
    return false;
}

std::list<MySQLHelper::Friend> MySQLHelper::get_user_friends(int id) {
    std::list<Friend> friends;

    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("user_friend");
        RowResult result = table.select("friend_id", "team").where("user_id = :id").bind("id", id).execute();

        for (Row row: result) {
            int friend_id = row[0].get<int>();
            std::string team = row[1].get<std::string>();
            std::string name = get_user_name(friend_id);
            friends.emplace_back(friend_id, name, team);
        }
    } catch (const std::exception &err) {
        std::cerr << "Error getting user friends: " << err.what() << std::endl;
    }
    return friends;
}


uint64_t MySQLHelper::create_group(const std::string &group_name) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("group_info");
        const Result &result = table.insert("group_name").values(group_name).execute();
        return result.getAutoIncrementValue();
    } catch (const std::exception &err) {
        std::cerr << "Error creating group: " << err.what() << std::endl;
    }
    return false;
}

bool MySQLHelper::delete_group(int group_id) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("group_info");
        const Result &result = table.remove().where("group_id = :id").bind("id", group_id).execute();
        return result.getAffectedItemsCount() > 0;
    } catch (const std::exception &err) {
        std::cerr << "Error deleting group: " << err.what() << std::endl;
    }
    return false;
}

std::list<MySQLHelper::Group> MySQLHelper::get_all_groups() {
    std::list<Group> groups;
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table group_info = database.getTable("group_info");
        RowResult result = group_info.select("group_id", "group_name").execute();

        for (auto row: result) {
            int group_id = row[0].get<int>();
            std::string group_name = row[1].get<std::string>();
            groups.emplace_back(group_id, group_name);
        }
    } catch (const std::exception &err) {
        std::cerr << "Error fetching groups: " << err.what() << std::endl;
    }
    return groups;
}

std::optional<MySQLHelper::Group> MySQLHelper::get_group_by_id(int group_id) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table group_info = database.getTable("group_info");
        RowResult result = group_info.select("group_id", "group_name").where("group_id = :id").bind("id", group_id).execute();

        Row row = result.fetchOne();
        if (!row) { return std::nullopt; }

        int fetched_group_id = row[0].get<int>();
        std::string group_name = row[1].get<std::string>();
        return Group{fetched_group_id, group_name};

    } catch (const std::exception &err) {
        std::cerr << "Error fetching group by ID: " << err.what() << std::endl;
    }
    return std::nullopt;
}

std::list<MySQLHelper::User> MySQLHelper::get_group_members(int group_id) {
    std::list<User> users;
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table group_member = database.getTable("group_member");
        RowResult result = group_member.select("member_id").where("group_id = :group_id").bind("group_id", group_id).execute();

        for (auto row: result) {
            int member_id = row[0];
            auto user_opt = get_user_by_id(member_id);
            if (user_opt) {
                users.push_back(*user_opt);
            }
        }
    } catch (const std::exception &err) {
        std::cerr << "Error fetching group members: " << err.what() << std::endl;
    }
    return users;
}

bool MySQLHelper::add_member(int group_id, int member_id) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("group_member");
        const Result &result =
                table.insert("group_id", "member_id").values(group_id, member_id).execute();
        return result.getAffectedItemsCount() > 0;
    } catch (const std::exception &err) {
        std::cerr << "Error adding members: " << err.what() << std::endl;
    }
    return false;
}

bool MySQLHelper::remove_member(int group_id, int member_id) {
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("group_member");
        const Result &result =
                table.remove().where("group_id = :gid AND member_id = :mid").bind("gid", group_id).bind("mid", member_id).execute();
        return result.getAffectedItemsCount() > 0;
    } catch (const std::exception &err) {
        std::cerr << "Error removing members: " << err.what() << std::endl;
    }
    return false;
}

std::list<MySQLHelper::Group> MySQLHelper::get_all_groups_for_user(int user_id) {
    std::list<Group> groups;
    try {
        Session session(host, 33060, "root", password);
        Schema database = session.getSchema(database_name);
        Table table = database.getTable("group_member");
        RowResult result = table.select("group_id").where("member_id = :id").bind("id", user_id).execute();

        for (Row row: result) {
            int group_id = row[0].get<int>();
            auto group = get_group_by_id(group_id);
            if (group) {
                groups.push_back(*group);
            }
        }
    } catch (const std::exception &err) {
        std::cerr << "Error getting groups for user: " << err.what() << std::endl;
    }
    return groups;
}
