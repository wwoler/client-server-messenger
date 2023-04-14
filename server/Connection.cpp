#include "Connection.h"

Connection::Connection(Client* client, DataBase* db):
    _client(client),
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

auto Connection::showIp() const -> void
{
    _client->showIp();
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

auto Connection::loadString() const ->char*
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

auto Connection::sendString(const char* str, size_t const len) const ->void
{
    _client->sendString(str, len);
}

auto Connection::sendMessages(std::vector<Message> const& messages) const ->void
{
    _client->sendMessages(messages);
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

auto Connection::handlerExitRequest() -> void
{
    std::cout << "Client disconnected ";
    showIp();
    _status = Client::CONNECTION_STATUS::DOWN;
    _client->setUserData(nullptr);
}

auto Connection::handlerSignUpRequest() -> void
{
    Client::RESPONSE_TYPE response;
    User user(loadUserData());

    if (_dataBase->signUp(user))
    {
        response = Client::RESPONSE_TYPE::ACCEPTED;
        sendResponse(response, sizeof(response));
        setClientPos(_dataBase->getUserStreamPos());
    }
    else
    {
        response = Client::RESPONSE_TYPE::REJECTED;
        sendResponse(response, sizeof(response));
    }
}

auto Connection::handlerSignInRequest() -> void
{
    Client::RESPONSE_TYPE response;
    User user(loadUserData());

    if (_dataBase->login(user))
    {
        response = Client::RESPONSE_TYPE::ACCEPTED;
        sendResponse(response, sizeof(response));
        setClientPos(_dataBase->getUserStreamPos());
        sendString(user.getUsername().c_str(), user.getUsername().size());
        _client->setUserData(new User(user));
    }
    else
    {
        response = Client::RESPONSE_TYPE::REJECTED;
        sendResponse(response, sizeof(response));
    }
}

auto Connection::handlerGetMessagesRequest() -> void
{
    Client::RESPONSE_TYPE response;
    auto sender_ = loadString();
    auto receiver_ = loadString();

    User sender(sender_);
    User receiver(receiver_);
    std::vector<Message> messages;

    if (_dataBase->getMessages(receiver.getUsername(), sender.getUsername(), messages))
    {
        response = Client::RESPONSE_TYPE::ACCEPTED;
        sendResponse(response, sizeof(response));
    }
    else
    {
        response = Client::RESPONSE_TYPE::REJECTED;
        sendResponse(response, sizeof(response));
        delete[] sender_;
        delete[] receiver_;
        return;
    }
    size_t vecSize = messages.size();
    if (vecSize != 0)
    {
        sendUnsignedNUM(vecSize);
        sendMessages(messages);
    } else
    {
        sendUnsignedNUM(vecSize);
    }
    delete[] sender_;
    delete[] receiver_;
}

auto Connection::handlerSendMessageRequest() -> void
{
    Client::RESPONSE_TYPE response;

    Message message(loadMessage());

    if (_dataBase->sendMessage(message))
    {
        response = Client::RESPONSE_TYPE::ACCEPTED;
        sendResponse(response, sizeof(response));
    }
    else
    {
        response = Client::RESPONSE_TYPE::REJECTED;
        sendResponse(response, sizeof(response));
    }
}

auto Connection::handlerChPasswordRequest() -> void
{
    Client::RESPONSE_TYPE response;
    std::string newPassword(loadString());

    _client->getUserData()->setPass(newPassword);
    _dataBase->changePassword(*_client->getUserData(), getClientPos());
    response = Client::RESPONSE_TYPE::ACCEPTED;
    sendResponse(response, sizeof(response));
}

auto Connection::handlerChLoginRequest() -> void
{
    Client::RESPONSE_TYPE response;
    std::string newLogin(loadString());
    std::string oldLogin = _client->getUserData()->getLogin();
    _client->getUserData()->setLogin(newLogin);

    if (_dataBase->changeLogin(*_client->getUserData(), getClientPos()))
    {
        response = Client::RESPONSE_TYPE::ACCEPTED;
        sendResponse(response, sizeof(response));
    }
    else
    {
        _client->getUserData()->setLogin(oldLogin);
        response = Client::RESPONSE_TYPE::REJECTED;
        sendResponse(response, sizeof(response));
    }
}

auto Connection::handlerClearChatRequest() -> void
{
    Client::RESPONSE_TYPE response;
    User user(loadString());
    bool confirm(static_cast<bool>(loadRequest()));
    if(!confirm) return;

    if (_dataBase->clearChat(_client->getUserData()->getUsername(), user.getUsername()))
    {
        response = Client::RESPONSE_TYPE::ACCEPTED;
        sendResponse(response, sizeof(response));
    }
    else
    {
        response = Client::RESPONSE_TYPE::REJECTED;
        sendResponse(response, sizeof(response));
        return;
    }
}

auto Connection::requestHandlerFunc() ->void
{
    try {
        while (true)
        {
            auto request = loadRequest();

            switch (request) {

                case Client::REQUEST_TYPE::EXIT:           handlerExitRequest();        return;
                case Client::REQUEST_TYPE::SIGN_UP:        handlerSignUpRequest();      break;
                case Client::REQUEST_TYPE::SIGN_IN:        handlerSignInRequest();      break;
                case Client::REQUEST_TYPE::GET_MESSAGES:   handlerGetMessagesRequest(); break;
                case Client::REQUEST_TYPE::SEND_MESSAGE:   handlerSendMessageRequest(); break;
                case Client::REQUEST_TYPE::CH_PASSWORD:    handlerChPasswordRequest();  break;
                case Client::REQUEST_TYPE::CH_LOGIN:       handlerChLoginRequest();     break;
                case Client::REQUEST_TYPE::CLEAR_CHAT:     handlerClearChatRequest();   break;
                default:
                    break;
            }
        }
    } catch(std::exception const& ex)
    {
        std::cout << ex.what();
        showIp();
        _status = Client::CONNECTION_STATUS::DOWN;
        return;
    }
}






