#include "Connection.h"

Connection::Connection(Client* client, DataBase* db):
    _client(std::make_unique<Client>(client)),
    _dataBase(db),
    _status(Client::CONNECTION_STATUS::NON) {}


auto Connection::start() ->void
{
    _status = Client::CONNECTION_STATUS::UP;
    _thread = std::thread(&Connection::requestHandlerFunc, this);

}

auto Connection::stop() ->void
{
    _client->disconnect();
    _thread.join();
}

auto Connection::loadRequest() const ->Client::REQUEST_TYPE
{
    return _client->loadRequest();
}

auto Connection::loadUnsignedNUM() const ->size_t
{
    return _client->loadUnsignedNUM();
}

auto Connection::loadSignedNUM() const ->long
{
    return _client->loadSignedNUM();
}

auto Connection::loadUserData() const ->User
{
    return _client->loadUserData();
}

auto Connection::loadMessage() const ->Message
{
    return _client->loadMessage();
}

auto Connection::loadString() const ->wchar_t*
{
    return _client->loadString();
}

auto Connection::getClient() const ->Client const*
{
    return _client.get();
}

auto Connection::sendUnsignedNUM(size_t num) const ->void
{
    _client->sendUnsignedNUM(num);
}

auto Connection::sendSignedNUM(long num) const ->void
{
    _client->sendSignedNUM(num);
}

auto Connection::sendResponse(Client::RESPONSE_TYPE const& response, int const size) const ->void
{
    _client->sendResponse(response, size);
}

auto Connection::sendString(const wchar_t* str, size_t const len) const ->void
{
    _client->sendString(str, len);
}

auto Connection::sendMessages(std::vector<Message> const& messages) const ->void
{
    _client->sendMessages(messages);
}

auto Connection::requestHandlerFunc() ->void
{
    Client::RESPONSE_TYPE response;

    try {
        while (true) {
            auto request = loadRequest();

            switch (request) {

                case Client::REQUEST_TYPE::EXIT:
                    std::cout << "disconnected\n";
                    _status = Client::CONNECTION_STATUS::DOWN;
                    return;
                case Client::REQUEST_TYPE::SIGN_UP: {
                    User user(loadUserData());

                    _dataBase->lock();
                    if (_dataBase->signUp(user)) {
                        response = Client::RESPONSE_TYPE::ACCEPTED;
                        sendResponse(response, sizeof(bool));
                        setClientPos(_dataBase->getUserStreamPos());
                    } else {
                        response = Client::RESPONSE_TYPE::REJECTED;
                        sendResponse(response, sizeof(bool));
                    }
                    _dataBase->unlock();
                    break;
                }
                case Client::REQUEST_TYPE::SIGN_IN: {
                    User user(loadUserData());

                    _dataBase->lock();
                    if (_dataBase->login(user)) {
                        response = Client::RESPONSE_TYPE::ACCEPTED;
                        sendResponse(response, 1);
                        setClientPos(_dataBase->getUserStreamPos());
                        sendString(user.getUsername().c_str(), user.getUsername().size());
                        _userData = std::make_unique<User>(user);
                    } else {
                        response = Client::RESPONSE_TYPE::REJECTED;
                        sendResponse(response, 1);
                    }
                    _dataBase->unlock();
                    break;
                }
                case Client::REQUEST_TYPE::GET_MESSAGES: {

                    auto sender_ = loadString();
                    auto receiver_ = loadString();

                    User sender(sender_);
                    User receiver(receiver_);

                    _dataBase->lock();
                    if ((receiver.getUsername() == L"common_chat")
                        || _dataBase->isExisting(receiver)) {
                        response = Client::RESPONSE_TYPE::ACCEPTED;
                        sendResponse(response, sizeof(bool));
                    } else {
                        response = Client::RESPONSE_TYPE::REJECTED;
                        sendResponse(response, sizeof(bool));
                        delete[] sender_;
                        delete[] receiver_;
                        break;
                    }

                    std::vector<Message> vec =
                            _dataBase->getMessages(receiver.getUsername(), sender.getUsername());

                    _dataBase->unlock();

                    size_t vecSize = vec.size();
                    if (vecSize != 0)
                    {
                        sendUnsignedNUM(vecSize);
                        sendMessages(vec);
                    } else {
                        sendUnsignedNUM(vecSize);
                    }

                    break;
                }
                case Client::REQUEST_TYPE::SEND_MESSAGE: {
                    User user(loadString());

                    _dataBase->lock();
                    if (user.getUsername() == L"common_chat"
                        || _dataBase->isExisting(user)) {
                        response = Client::RESPONSE_TYPE::ACCEPTED;
                        sendResponse(response, sizeof(bool));
                        Message message(loadMessage());
                        _dataBase->sendMessage(message);
                    } else {
                        response = Client::RESPONSE_TYPE::REJECTED;
                        sendResponse(response, sizeof(bool));
                    }
                    _dataBase->unlock();
                    break;
                }
                case Client::REQUEST_TYPE::CH_PASSWORD: {
                    User user(loadUserData());
                    _dataBase->lock();
                    _dataBase->changeUserData(user, getClientPos());
                    _dataBase->unlock();
                    response = Client::RESPONSE_TYPE::ACCEPTED;
                    sendResponse(response, sizeof(bool));

                    break;
                }
                case Client::REQUEST_TYPE::CH_LOGIN: {
                    User user(loadUserData());

                    _dataBase->lock();
                    if (_dataBase->isExisting(user)) {
                        response = Client::RESPONSE_TYPE::ACCEPTED;
                        sendResponse(response, sizeof(bool));
                        _dataBase->changeUserData(user, getClientPos());
                    } else {
                        response = Client::RESPONSE_TYPE::REJECTED;
                        sendResponse(response, sizeof(bool));
                    }
                    _dataBase->unlock();

                    break;
                }
                case Client::REQUEST_TYPE::CLEAR_CHAT: {
                    User user(loadString());

                    _dataBase->lock();
                    if (_dataBase->isExisting(user)) {
                        response = Client::RESPONSE_TYPE::ACCEPTED;
                        sendResponse(response, sizeof(bool));
                    } else {
                        response = Client::RESPONSE_TYPE::REJECTED;
                        sendResponse(response, sizeof(bool));
                        break;
                    }
                    _dataBase->unlock();

                    bool ans = static_cast<bool>(loadRequest());
                    if (ans)
                        _dataBase->clearChat(_userData->getUsername(), user.getUsername());

                    break;
                }
                default:
                    break;
            }
        }
    } catch(std::exception const& ex) {
        _dataBase->unlock();
        std::cout << ex.what() << std::endl;
        _status = Client::CONNECTION_STATUS::DOWN;
        return;
    }
}

auto Connection::getClientPos() const ->std::streampos const&
{
    return _client->getPos();
}

auto Connection::setClientPos(std::streampos& pos) ->void
{
    _client->setPos(pos);
}

auto Connection::getConnectionStatus() const ->Client::CONNECTION_STATUS
{
    return _status;
}




