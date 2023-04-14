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
        NON,
        UP,
        DOWN
    };

    enum class REQUEST_TYPE : uint8_t
    {
        EXIT,
        SIGN_UP,
        SIGN_IN,
        GET_MESSAGES,
        SEND_MESSAGE,
        CH_PASSWORD,
        CH_LOGIN,
        CLEAR_CHAT
    };

    enum class RESPONSE_TYPE : uint8_t
    {
        REJECTED,
        ACCEPTED
    };

public:
    Client() = default;
    Client(int socket, sockaddr_in const& addr);
    ~Client();

private:
    int                     _socket;
    sockaddr_in             _address;
    std::streampos          _userPos;
    std::unique_ptr<User>   _userData;

public:
    auto loadRequest()                                  const ->REQUEST_TYPE;
    auto loadUserData()                                 const ->User;
    auto loadMessage()                                  const ->Message;
    auto loadString()                                   const ->char*;
    auto loadUnsignedNUM()                              const ->size_t;
    auto loadSignedNUM()                                const ->long;


    auto sendUnsignedNUM(size_t const)                  const ->void;
    auto sendSignedNUM(long const)                      const ->void;
    auto sendString(char const*, size_t)                const ->void;
    auto sendResponse(RESPONSE_TYPE const, int const)   const ->void;
    auto sendMessages(std::vector<Message> const&)      const ->void;
    auto disconnect()                                   const ->void;

    auto getUserData()                                        ->User*;
    auto getPos()                                       const ->std::streampos const&;
    auto setUserData(User* data = nullptr)                    ->void;
    auto setPos(std::streampos const&)                        ->void;
    auto showIp()                                       const ->void;

};
