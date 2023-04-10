#include "DataBase.h"

using json = nlohmann::json;

DataBase::DataBase(std::filesystem::path const&& path) :
        _path(path)
{
    std::filesystem::create_directories(_path);
    std::filesystem::create_directories(_path.string() + "/Messages_List");

    _messageStream.open(_path.string() + "/Messages_List/common_chat.json",
                        std::ios::in | std::ios::out | std::ios::binary);

    if (!_messageStream.is_open())
    {
        _messageStream.open(_path.string() + "/Messages_List/common_chat.json",
                            std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    }

    _userStream.open(_path.string() + "users.json",
                     std::ios::in | std::ios::out | std::ios::binary);

    if (!_userStream.is_open())
    {
        _userStream.open(_path.string() + "users.json",
                         std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
    }
    _messageStream.close();
}

auto DataBase::adjustment()  -> void
{
    auto pos = _userStream.tellp();
    _userStream.seekp(pos);
    auto count = 0;
    auto ch = _userStream.get();
    while (ch != '\n')
    {
        count++;
        ch = _userStream.get();
    }

    _userStream.seekg(pos);
    for (int i = 0; i < count; ++i)
    {
        _userStream.put(' ');
    }
}

auto DataBase::to_json(User& user)  ->void
{
    json js;
    js["Login"] = user.getLogin();
    js["Password"] = user.getPass();
    js["Username"] = user.getUsername();
    _userStream << js;
}


auto DataBase::to_json(Message& mess)  ->void
{
    json js;
    js["Content"] = mess.getContent();
    js["Sender"] = mess.getSender();
    js["Time"] = mess.getTime();
    _messageStream << js;
}

auto DataBase::from_json(User& user)  ->void
{
    json js;
    _userStream >> js;

    user.setLogin(js["Login"]);
    user.setPass(js["Password"]);
    user.setUsername(js["Username"]);
}

auto DataBase::from_json(Message& mess)  ->void
{
    json js;
    _messageStream >> js;

    mess.setSender(js["Sender"]);
    mess.setContent(js["Content"]);
    mess.setTime(js["Time"]);
}

auto DataBase::login(User& userID)  ->bool
{
    std::string buffPassword = userID.getPass();
    std::unique_ptr<User> result(new User());

    std::lock_guard<std::mutex> lk(_mutex);
    if (isExisting(userID, TypeExisting::login, result.get()) && userID.getPass() == buffPassword)
    {
        userID.setUsername(result->getUsername());
        return true;
    }
    return false;
}

auto DataBase::signUp(User& userID)  ->bool
{
    std::lock_guard<std::mutex> lk(_mutex);
    if (isExisting(userID, TypeExisting::login_and_username))
        return false;

    _userStream.seekp(0u, std::ios::end);

    _userStream << std::setw(219) << '\n';

    _userStream.seekp(-219, std::ios::cur);

    to_json(userID);

    return true;
}

auto DataBase::isExisting(User& userID, TypeExisting type, User* resultData)  ->bool
{
    _userStream.seekp(0u, std::ios::beg);

    _userStream.get();

    User buff;
    while (_userStream.good())
    {
        _userStream.unget();

        _currentUserPos = _userStream.tellp();

        from_json(buff);
        switch(type)
        {
            case TypeExisting::login:
                if (userID.getLogin() == buff.getLogin())
                {
                    if(resultData)
                    {
                        resultData->setPass(buff.getPass());
                        resultData->setUsername(buff.getUsername());
                    }
                    return true;
                }
                break;
            case TypeExisting::username:
                if (userID.getUsername() == buff.getUsername())
                {
                    return true;
                }
                break;
            case TypeExisting::login_and_username:
                if (userID.getUsername() == buff.getUsername() || userID.getLogin() == buff.getLogin())
                    return true;
                break;
        }

        while (_userStream.get() != '\n');
        _userStream.get();
    }
    _userStream.clear();
    return false;
}

auto DataBase::clearChat(std::string const& user1, std::string const& user2)  -> bool
{
    User buffUser(user2);
    std::lock_guard<std::mutex> lk(_mutex);
    if(!isExisting(buffUser, TypeExisting::username)) return false;
    std::string dialog;
    user1 > user2 ?
    dialog.assign(user1 + user2):
    dialog.assign(user2 + user1);

    _messageStream.open(_path.string() + "Messages_List/" + dialog + ".json", std::ios::out | std::ios::trunc);
    _messageStream.close();
    return true;
}

auto DataBase::sendMessage(Message& message)  ->bool
{
    User buffUser(message.getReceiver());
    std::lock_guard<std::mutex> lk(_mutex);
    if (buffUser.getUsername() == "common_chat")
    {
        _messageStream.open(_path.string() + "Messages_List/common_chat.json", std::ios::app | std::ios::out);
    }
    else if(isExisting(buffUser, TypeExisting::username))
    {
        std::string dialog;
        message.getReceiver() > message.getSender() ?
        dialog.assign(message.getReceiver() + message.getSender()):
        dialog.assign(message.getSender() + message.getReceiver());

        _messageStream.open(_path.string() + "Messages_List/" + dialog + ".json", std::ios::app | std::ios::out);
    }
    else return false;

    to_json(message);

    _messageStream.close();
    return true;
}


auto DataBase::getMessages(std::string const& from, std::string const& to, std::vector<Message>& messages)  -> bool
{
    User buffUser(from);
    std::lock_guard<std::mutex> lk(_mutex);
    if (from == "common_chat")
    {
        _messageStream.open(_path.string() + "Messages_List/common_chat.json", std::ios::out | std::ios::in);
    }
    else if(isExisting(buffUser, TypeExisting::username))
    {
        std::string dialog;
        to > from ?
        dialog.assign(to + from) :
        dialog.assign(from + to);

        _messageStream.open(_path.string() + "Messages_List/" + dialog + ".json", std::ios::out | std::ios::in);
    }
    else return false;

    _messageStream.get();

    messages.reserve(20);
    Message buffMessage;
    while (_messageStream.good())
    {
        _messageStream.unget();

        from_json(buffMessage);
        messages.push_back(buffMessage);

        _messageStream.get();
    }
    _messageStream.close();
    return true;
}

auto DataBase::getUserStreamPos() -> std::streampos&
{
    return _currentUserPos;
}


auto DataBase::changeLogin(User& newData, std::streampos const& pos) ->bool
{
    std::lock_guard<std::mutex> lk(_mutex);

    if(isExisting(newData, TypeExisting::login)) return false;

    _userStream.seekp(pos);

    to_json(newData);

    adjustment();

    return true;
}

auto DataBase::changePassword(User& newData, std::streampos const& pos) -> void
{
    std::lock_guard<std::mutex> lk(_mutex);

    _userStream.seekp(pos);

    to_json(newData);

    adjustment();
}


