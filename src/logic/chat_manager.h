#ifndef CHAT_MANAGER_H
#define CHAT_MANAGER_H

#include <list>
#include <optional>

#include "api_chat.pb.h"

#include "../database/database.h"

class ChatManager {
public:

    explicit ChatManager(const Database &database);

    std::list<api::chat::Chat>
    list_all_chats();

    std::optional<api::chat::Chat *>
    get_chat_by_id(uint64_t id);

    std::optional<api::chat::MessageList *>
    get_messages_by_id(uint64_t id);

    uint64_t create_chat_and_messages(std::string name, std::string description, bool is_group);
    bool delete_chat(uint64_t id);

private:
    std::map<uint64_t, api::chat::Chat> _all_chats;
    std::map<uint64_t, api::chat::MessageList> _all_messages; // chat_id to messages

    void append_message(uint64_t chat_id, uint64_t timestamp, uint64_t user_id, std::string user_name, std::string content) const;
    void setup_default_chat();

    Database db;
};


#endif //CHAT_MANAGER_H
