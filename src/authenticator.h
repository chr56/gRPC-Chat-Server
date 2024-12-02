#ifndef _AUTHENTICATOR_H_
#define _AUTHENTICATOR_H_

#include <string_view>
#include <list>
#include <optional>

#include <grpc++/grpc++.h>

#include <api_chat.pb.h>
#include <api_chat.grpc.pb.h>

class Authenticator
{
public:
    Authenticator();
    ~Authenticator();

    typedef std::multimap<grpc::string_ref, grpc::string_ref> Metadata;

    std::optional<std::string> check_user_credentials(Metadata &metadata);

private:
    api::chat::Users _knownUsers;
};

#endif