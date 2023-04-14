#pragma once

#include "client.h"
#include <memory>

class Connection final
{
public:
    Connection() = default;
    Connection(Client* client, DataBase* db);
    ~Connection() = default;
private:
    std::unique_ptr<Client>      _client;
    std::thread                  _thread;
    DataBase*                    _dataBase;
    Client::CONNECTION_STATUS    _status;

    auto setClientPos(std::streampos&)                                                ->void;

    auto loadSignedNUM()                                                        const ->long;
    auto loadString()                                                           const ->char*;
    auto loadRequest()                                                          const ->Client::REQUEST_TYPE;
    auto loadUnsignedNUM()                                                      const ->size_t;
    auto loadUserData()                                                         const ->User;
    auto loadMessage()                                                          const ->Message;

    auto getClient()                                                            const ->Client const*;
    auto getClientPos()                                                         const ->std::streampos const&;

    auto sendUnsignedNUM(size_t)                                                const ->void;
    auto sendSignedNUM(long)                                                    const ->void;
    auto sendString(char const*, size_t const)                                  const ->void;
    auto sendResponse(Client::RESPONSE_TYPE const&, int const)                  const ->void;
    auto sendMessages(std::vector<Message> const&)                              const ->void;

    auto handlerExitRequest()                                                         ->void;
    auto handlerSignUpRequest()                                                       ->void;
    auto handlerSignInRequest()                                                       ->void;
    auto handlerGetMessagesRequest()                                                  ->void;
    auto handlerSendMessageRequest()                                                  ->void;
    auto handlerChPasswordRequest()                                                   ->void;
    auto handlerChLoginRequest()                                                      ->void;
    auto handlerClearChatRequest()                                                    ->void;
    auto requestHandlerFunc()                                                         ->void;

    auto showIp()                                                               const ->void;

public:
    auto start()                                                                      ->void;
    auto stop()                                                                       ->void;

    auto getConnectionStatus()                                                  const ->Client::CONNECTION_STATUS;

};
