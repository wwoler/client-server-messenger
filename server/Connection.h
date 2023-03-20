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
    std::unique_ptr<User>        _userData;
    std::unique_ptr<Client>      _client;
    std::thread                  _thread;
    DataBase*                    _dataBase;
    Client::CONNECTION_STATUS    _status;


    auto requestHandlerFunc()                                                         ->void;
    auto setClientPos(std::streampos&)                                                ->void;

    auto loadSignedNUM()                                                        const ->long;
    auto loadString()                                                           const ->wchar_t*;
    auto loadRequest()                                                          const ->Client::REQUEST_TYPE;
    auto loadUnsignedNUM()                                                      const ->size_t;
    auto loadUserData()                                                         const ->User;
    auto loadMessage()                                                          const ->Message;

    auto getClient()                                                            const ->Client const*;
    auto getClientPos()                                                         const ->std::streampos const&;

    auto sendUnsignedNUM(size_t)                                                const ->void;
    auto sendSignedNUM(long)                                                    const ->void;
    auto sendString(wchar_t const*, size_t const)                               const ->void;
    auto sendResponse(Client::RESPONSE_TYPE const&, int const)                  const ->void;
    auto sendMessages(std::vector<Message> const&)                              const ->void;

public:
    auto start()                                                                      ->void;
    auto stop()                                                                       ->void;

    auto getConnectionStatus()                                                  const ->Client::CONNECTION_STATUS;

};