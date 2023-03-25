#pragma once

#include "Message.h"
#include "User.h"
#include "DataBase.h"
#include <thread>
#include <string>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <vector>


class Client final
{
public:
    enum class CONNECTION_STATUS : uint8_t
    {
        NON          = 0,
        UP           = 1,
        DOWN         = 2
    };

    enum class REQUEST_TYPE : uint8_t
    {
        EXIT         = 0,
        SIGN_UP      = 1,
        SIGN_IN      = 2,
        GET_MESSAGES = 3,
        SEND_MESSAGE = 4,
        CH_PASSWORD  = 5,
        CH_LOGIN     = 6,
        CLEAR_CHAT   = 7
    };

    enum class RESPONSE_TYPE : uint8_t
    {
        REJECTED = 0,
        ACCEPTED = 1
    };

public:
    Client() = default;
    Client(int socket, sockaddr_in const& addr);
    ~Client();

private:
    int                 _socket;
    sockaddr_in         _address;
    std::streampos      _userPos;

public:
    auto loadRequest()                                  const ->REQUEST_TYPE;
    auto loadUserData()                                 const ->User;
    auto loadMessage()                                  const ->Message;
    auto loadString()                                   const ->wchar_t*;
    auto loadUnsignedNUM()                              const ->size_t;
    auto loadSignedNUM()                                const ->long;


    auto sendUnsignedNUM(size_t const)                  const ->void;
    auto sendSignedNUM(long const)                      const ->void;
    auto sendString(wchar_t const*, size_t)             const ->void;
    auto sendResponse(RESPONSE_TYPE const, int const)   const ->void;
    auto sendMessages(std::vector<Message> const&)      const ->void;
    auto disconnect()                                   const ->void;

    auto getPos()                                       const ->std::streampos const&;
    auto setPos(std::streampos const&)                        ->void;
    auto showIp()                                       const ->void;

};