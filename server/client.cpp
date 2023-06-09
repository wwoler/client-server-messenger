#include "client.h"

Client::Client(int socket, sockaddr_in const& addr):
    _socket(socket),
    _address(addr)
{
    showIp();
}

Client::~Client()
{
    close(_socket);
}

auto Client::loadRequest() const ->REQUEST_TYPE
{
    REQUEST_TYPE request;
    if(recv(_socket, &request, sizeof(request), 0) == 0)
        throw std::runtime_error("Client disconnected ");
    return request;
}

auto Client::loadUnsignedNUM() const ->size_t
{
    size_t buff;
    if(recv(_socket, &buff, sizeof(buff), 0) == 0)
        throw std::runtime_error("Client disconnected ");
    return buff;
}

auto Client::loadSignedNUM() const ->long
{
    long buff;
    if(recv(_socket, &buff, sizeof(buff), 0) == 0)
        throw std::runtime_error("Client disconnected ");
    return buff;
}

auto Client::loadString() const ->char*
{
    size_t strLen;
    if(recv(_socket, &strLen, sizeof(strLen), 0) == 0)
        throw std::runtime_error("Client disconnected ");
    auto* str = new char[strLen + 1]{};
    str[strLen] = '\0';
    if(recv(_socket, str, strLen, 0) == 0)
        throw std::runtime_error("Client disconnected ");
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

    Message message(sender, receiver, content, time_);

    delete[] sender;
    delete[] receiver;
    delete[] content;

    return message;
}

auto Client::sendUnsignedNUM(size_t const num) const ->void
{
    if(send(_socket, &num, sizeof(num), 0) == 0)
        throw std::runtime_error("Client disconnected ");
}

auto Client::sendSignedNUM(long const num) const ->void
{
    if(send(_socket, &num, sizeof(num), 0) == 0)
        throw std::runtime_error("Client disconnected ");
}

auto Client::sendResponse(RESPONSE_TYPE const response, int const size) const ->void
{
    if(send(_socket, &response, size, 0) == 0)
        throw std::runtime_error("Client disconnected ");
}

auto Client::sendString(char const* str, size_t len) const ->void
{
    if(len == 0) len += 1;

    if(send(_socket, &len, sizeof(len), 0) == 0)
        throw std::runtime_error("Client disconnected ");
    if(send(_socket, str, len, 0) == 0)
        throw std::runtime_error("Client disconnected ");
}


auto Client::sendMessages(std::vector<Message> const& messages) const ->void
{
    time_t time_;
    for(auto const& message : messages)
    {
        time_ = message.getTime();

        sendString(message.getSender().c_str(), message.getSender().size());
        sendString(message.getReceiver().c_str(), message.getReceiver().size());
        sendString(message.getContent().c_str(), message.getContent().size());
        sendSignedNUM(time_);
    }
}


auto Client::getUserData() ->User*
{
    return _userData.get();
}

auto Client::setUserData(User* data) -> void
{
    _userData.reset(data);
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

auto Client::showIp() const -> void
{
    unsigned char bytes[4];
    bytes[0] = _address.sin_addr.s_addr & 0xFF;
    bytes[1] = (_address.sin_addr.s_addr >> 8) & 0xFF;
    bytes[2] = (_address.sin_addr.s_addr >> 16) & 0xFF;
    bytes[3] = (_address.sin_addr.s_addr >> 24) & 0xFF;
    std::cout << static_cast<int>(bytes[0]) << '.'
              << static_cast<int>(bytes[1]) << '.'
              << static_cast<int>(bytes[2]) << '.'
              << static_cast<int>(bytes[3]) << std::endl;
}










