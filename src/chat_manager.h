#ifndef CHAT_MANAGER_H
#define CHAT_MANAGER_H

#include <list>
#include <optional>

#include "api_chat.pb.h"

class ChatManager {
public:

    std::list<api::chat::Chat>
    list_all_chats();

    std::optional<api::chat::MessageList *>
    get_messages_by_id(uint64_t id);

    std::optional<api::chat::Chat *>
    get_chat_by_id(uint64_t id);

    ChatManager();

private:
    std::map<uint64_t, api::chat::Chat> _all_chats;
    std::map<uint64_t, api::chat::MessageList> _all_messages; // chat_id to messages
    void setup_default_chat();
};


#endif //CHAT_MANAGER_H
