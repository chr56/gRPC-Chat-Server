#ifndef CHAT_MANAGER_H
#define CHAT_MANAGER_H

#include <list>
#include <optional>

#include "api_chat.pb.h"

#include "../database/database.h"

class ChatManager {
public:

    typedef uint64_t chat_id;

    explicit ChatManager(const Database &database);

    std::list<api::chat::Chat>
    list_all_chats();

    std::optional<api::chat::Chat *>
    get_chat_by_id(chat_id id);

    std::list<api::chat::Chat>
    get_all_group_chats(uint64_t user_id);

    std::list<api::chat::Chat>
    get_all_private_chats(uint64_t user_id);

    std::optional<api::chat::Chat>
    get_private_chat(uint64_t user1_id, uint64_t user2_id);

    std::optional<api::chat::MessageList *>
    get_messages_by_id(chat_id id);

    uint64_t create_chat_and_messages(std::string name, std::string description, bool is_group);
    bool delete_chat_and_messages(uint64_t id);

private:

    std::map<chat_id, api::chat::Chat> _all_chats;

    std::map<chat_id, api::chat::MessageList> _all_messages;

    void append_message(chat_id chat_id, uint64_t timestamp, uint64_t user_id, std::string user_name, std::string content);
    void setup_default_chat();

    Database db;
};


#endif //CHAT_MANAGER_H
