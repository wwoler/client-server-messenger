#pragma once
#include "Singleton.h"
#include "User.h"
#include "ConsoleManager.h"
#include "Message.h"
#include <string>
#include <iostream>
#include <regex>
#include <iomanip>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctime>

using namespace ConsoleAttributes;

class CloseChat{};

class Chat : public Singleton<Chat>
{
private:
    enum class RESPONSE_TYPE : uint8_t
    {
        REJECTED = 0,
        ACCEPTED = 1
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


private:
    int                   _socket;
    std::unique_ptr<User> _currentUser;
    void (Chat::*         _state)();
    bool                  _bUserStatus;
    unsigned              _consoleWidth;

    auto chatMenu() const                                          ->void;
    auto userMenu() const                                          ->void;
    auto chatBox(std::string const&, size_t const&) const    ->void;

    auto login()                                                   ->void;
    auto signUp()                                                  ->void;

    auto chatLoop()                                                ->void;
    auto userLoop()                                                ->void;

    auto action_for_chat()                                         ->void;
    auto action_for_user()                                         ->void;

    auto getMessages()                                             ->void;
    auto sendMessage()                                             ->void;

    auto clear_chat()                                              ->void;
    auto get_info()                                                ->void;
    auto changePassword()                                          ->void;
    auto changeLogin()                                             ->void;

    auto set_user_data(std::string&, std::regex const&)            ->bool;
    auto set_current_user(std::unique_ptr<User> user)              ->void;

    auto logout()                                                  ->void;
    auto exit()                                                    ->void;
    auto flush_input_buffer()                                      ->void;
    auto time_to_string(time_t time)                              ->std::string;


    auto sendUnsignedNUM(size_t const)     const                   ->void;
    auto sendSignedNUM(long const)         const                   ->void;
    auto sendRequest(REQUEST_TYPE const)   const                   ->void;
    auto sendString(std::string const&)   const                    ->void;
    auto sendUserData(User&)               const                   ->void;
    auto sendMessage(Message&)             const                   ->void;


    auto loadResponse()                    const                   ->RESPONSE_TYPE;
    auto loadUnsignedNUM()                 const                   ->size_t;
    auto loadSignedNUM()                   const                   ->long;
    auto loadString()                      const                   ->std::string;
    auto loadMessage()                     const                   ->Message;

public:
    auto run()                                                     ->void;
    ~Chat() override;

protected:
    Chat();
};