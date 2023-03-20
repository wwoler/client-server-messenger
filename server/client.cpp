#include "client.h"

Client::Client(int socket, sockaddr_in const& addr):
    _socket(socket),
    _address(addr) {}

Client::Client(Client* client)
{
    _address = client->_address;
    _socket = client->_socket;
}

Client::~Client()
{
    close(_socket);
}

auto Client::loadRequest() const ->REQUEST_TYPE
{
    REQUEST_TYPE request;
    if(recv(_socket, &request, sizeof(request), 0) == 0)
        throw std::runtime_error("Client disconnected");
    return request;
}

auto Client::loadUnsignedNUM() const ->size_t
{
    size_t buff;
    if(recv(_socket, &buff, sizeof(buff), 0) == 0)
        throw std::runtime_error("Client disconnected");
    return buff;
}

auto Client::loadSignedNUM() const ->long
{
    long buff;
    if(recv(_socket, &buff, sizeof(buff), 0) == 0)
        throw std::runtime_error("Client disconnected");
    return buff;
}

auto Client::loadString() const ->wchar_t*
{
    size_t strLen;
    if(recv(_socket, &strLen, sizeof(strLen), 0) == 0)
        throw std::runtime_error("Client disconnected");
    auto* str = new wchar_t[strLen + 1]{};
    str[strLen] = '\0';
    if(recv(_socket, str, strLen * sizeof(wchar_t), 0) == 0)
        throw std::runtime_error("Client disconnected");
    return str;
}


auto Client::loadUserData() const ->User
{
    auto* login    = loadString();
    auto* password = loadString();
    auto* username = loadString();


    User user(login, password, username);

    delete[] login;
    delete[] password;
    delete[] username;

    return user;
}

auto Client::loadMessage() const ->Message
{
    auto* sender   = loadString();
    auto* receiver = loadString();
    auto* content  = loadString();
    time_t time_            = loadSignedNUM();

    Message message(sender, receiver, content, std::move(time_));

    delete[] sender;
    delete[] receiver;
    delete[] content;

    return message;
}

auto Client::sendUnsignedNUM(size_t const num) const ->void
{
    if(send(_socket, &num, sizeof(num), 0) == 0)
        throw std::runtime_error("Client disconnected");
}

auto Client::sendSignedNUM(long const num) const ->void
{
    if(send(_socket, &num, sizeof(num), 0) == 0)
        throw std::runtime_error("Client disconnected");
}

auto Client::sendResponse(RESPONSE_TYPE const response, int const size) const ->void
{
    if(send(_socket, &response, size, 0) == 0)
        throw std::runtime_error("Client disconnected");
}

auto Client::sendString(wchar_t const* str, size_t len) const ->void
{
    if(len == 0) len += 1;

    if(send(_socket, &len, sizeof(len), 0) == 0)
        throw std::runtime_error("Client disconnected");
    if(send(_socket, str, len * sizeof(wchar_t), 0) == 0)
        throw std::runtime_error("Client disconnected");
}


auto Client::sendMessages(std::vector<Message> const& messages) const ->void
{
    for(auto const& message : messages)
    {
        time_t time_ = message.getTime();

        sendString(message.getSender().c_str(), message.getSender().size());
        sendString(message.getReceiver().c_str(), message.getReceiver().size());
        sendString(message.getContent().c_str(), message.getContent().size());

        write(_socket, &time_, sizeof(time_));
    }
}


auto Client::getPos() const ->std::streampos const&
{
    return _userPos;
}

auto Client::setPos(std::streampos const& pos) ->void
{
    _userPos = pos;
}

auto Client::disconnect() const ->void
{
    shutdown(_socket, 0);
}








