#include "authenticator.h"

Authenticator::Authenticator()
{
    api::chat::User *newUser;

    newUser = _knownUsers.add_users();
    newUser->set_name("user1");
    newUser->set_password("qwerty");

    newUser = _knownUsers.add_users();
    newUser->set_name("user2");
    newUser->set_password("qwerty");

    newUser = _knownUsers.add_users();
    newUser->set_name("user3");
    newUser->set_password("qwerty");

    newUser = _knownUsers.add_users();
    newUser->set_name("user4");
    newUser->set_password("qwerty");

    newUser = _knownUsers.add_users();
    newUser->set_name("user5");
    newUser->set_password("qwerty");
}

Authenticator::~Authenticator()
{
    _knownUsers.clear_users();
}

constexpr std::string_view field_name("user-name");
constexpr std::string_view field_password("user-password");

std::optional<std::string> Authenticator::check_user_credentials(Metadata &metadata)
{
    std::string name;
    std::string password;
    for (const auto &[key, value] : std::as_const(metadata))
    {
        if (std::string(key.data(), key.size()) == field_name)
        {
            name = std::string(value.data(), value.size());
        }
        if (std::string(key.data(), key.size()) == field_password)
        {
            password = std::string(value.data(), value.size());
        }
    }

    auto matched_result = std::find_if(
        _knownUsers.users().begin(), _knownUsers.users().end(),
        [&name, &password](const auto &it)
        {
            return it.name() == name && it.password() == password;
        });
    bool success = matched_result != _knownUsers.users().end();

    return success ? std::optional{name} : std::nullopt;
}