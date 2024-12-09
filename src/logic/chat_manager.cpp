#include "chat_manager.h"

using namespace api::chat;

std::list<Chat> ChatManager::list_all_chats() {
    std::list<Chat> chat_list;
    for (const auto &pair: _all_chats) {
        auto chat = pair.second;
        chat_list.push_back(chat);
    }
    return chat_list;
}


std::optional<Chat *> ChatManager::get_chat_by_id(uint64_t id) {
    auto it = _all_chats.find(id);
    if (it != _all_chats.end()) {
        return &(it->second);
    } else {
        return std::nullopt;
    }
}


std::optional<MessageList *> ChatManager::get_messages_by_id(uint64_t id) {
    auto it = _all_messages.find(id);
    if (it != _all_messages.end()) {
        return &(it->second);
    } else {
        return std::nullopt;
    }
}

uint64_t used_id = 0;
uint64_t ChatManager::create_chat_and_messages(std::string name, std::string description, bool is_group) {
    uint64_t id = used_id++;

    Chat test_chat;
    test_chat.set_id(id);
    test_chat.set_name(name);
    test_chat.set_is_group(is_group);
    test_chat.set_description(description);
    _all_chats[id] = test_chat;

    MessageList test_message_list;
    _all_messages[id] = test_message_list;

    return id;
}


bool ChatManager::delete_chat_and_messages(uint64_t id) {
    _all_messages.erase(id);
    _all_chats.erase(id);
    return true;
}


ChatManager::ChatManager(const Database &database) : db(database), _all_chats(), _all_messages() {
    setup_default_chat();

}

void ChatManager::append_message(uint64_t chat_id,
                                 uint64_t timestamp,
                                 uint64_t user_id,
                                 std::string user_name,
                                 std::string content) {
    Message *p = _all_messages[chat_id].add_messages();
    p->set_timestamp(timestamp);
    p->set_content(content);
    p->set_type(Message::Text);
    p->set_sender_user_id(user_id);
    p->set_sender_user_name(user_name);
}

void ChatManager::setup_default_chat() {
    uint64_t id;

    // Default Chat
    id = create_chat_and_messages("Public", "Default Chat", true);
    append_message(id, 0, 0, "System", "Welcome to Default chat");

    // Test Chat
    create_chat_and_messages("Test1", "Test Chat", true);
}