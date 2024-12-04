#include "chat_manager.h"

using namespace api::chat;

std::list<Chat *> ChatManager::list_all_chats() {
    std::list<Chat *> chat_list(_all_chats.size());
    for (const auto& pair : _all_chats) {
        auto chat = pair.second;
        chat_list.push_back(&chat);
    }
    return chat_list;
}


std::optional<MessageList *> ChatManager::get_messages_by_id(uint64_t id) {
    auto it = _all_messages.find(id);
    if (it != _all_messages.end()) {
        return &(it->second);
    } else {
        return std::nullopt;
    }
}

std::optional<Chat *> ChatManager::get_chat_by_id(uint64_t id) {
    auto it = _all_chats.find(id);
    if (it != _all_chats.end()) {
        return &(it->second);
    } else {
        return std::nullopt;
    }
}


ChatManager::ChatManager() : _all_chats(), _all_messages() {
    setup_default_chat();


    Chat test_chat;
    test_chat.set_id(100);
    test_chat.set_name("Test");
    test_chat.set_is_group(true);
    test_chat.set_description("Test");
    _all_chats[100] = test_chat;

    MessageList test_message_list;
    _all_messages[100] = test_message_list;
}

void ChatManager::setup_default_chat() {
    Chat default_chat;
    default_chat.set_id(0);
    default_chat.set_name("Public");
    default_chat.set_is_group(true);
    default_chat.set_description("Default chat");

    _all_chats[0] = default_chat;

    MessageList default_message_list;

    Message default_message;
    default_message.set_timestamp(0);
    default_message.set_content("Welcome to Default chat");
    default_message.set_type(Message::Text);
    default_message.set_sender_user_id(0);
    default_message.set_sender_user_name("System");

    *default_message_list.add_messages() = default_message;

    _all_messages[0] = default_message_list;
}
