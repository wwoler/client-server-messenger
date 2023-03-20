#include "Chat.h"
#include <limits>

#ifdef __linux__
#define localtime(time_point, curr_time) localtime_r(curr_time, time_point)

#elif defined(_WIN32) || defined(_WIN64)
#define localtime(time_point, curr_time) localtime_s(time_point, curr_time)

#endif


Chat::Chat():
    _consoleWidth(100),
    _socket(-1),
    _state(nullptr) {}

Chat::~Chat()
{
    close(_socket);
}

auto Chat::flush_input_buffer()  ->void
{
    auto max_ = std::numeric_limits<std::streamsize>::max;
    std::wcin.ignore(max_(), '\n');
}

auto Chat::set_user_data(std::wstring& data, std::wregex const& reg)  ->bool
{
    std::getline(std::wcin, data);
    while (!std::regex_match(data, reg))
    {
        setConsoleColor(Color::RED);
        std::wcout << L"Incorrect input, try again(quit to quit)\n> ";
        setConsoleColor(Color::DEFAULT);
        std::getline(std::wcin, data);
    }

    if(data == L"quit" || data ==L"common_chat")
        return false;

    return true;
}


auto Chat::set_current_user(std::unique_ptr<User> user)  ->void
{
    _currentUser.reset(user.get());
    user.release();
}


auto Chat::time_to_string(time_t& time)  ->std::wstring
{
    std::tm time_point;

    localtime(&time_point, &time);

    std::wstring year = std::to_wstring(time_point.tm_year + 1900u);

    std::wstring month;
    if (time_point.tm_mon + 1u < 10u)
    {
        month.append(L"0");
    }
    month.append(std::to_wstring(time_point.tm_mon + 1u));

    std::wstring day;
    if (time_point.tm_mday < 10u)
    {
        day.append(L"0");
    }
    day.append(std::to_wstring(time_point.tm_mday));

    std::wstring hour;
    if (time_point.tm_hour < 10u)
    {
        hour.append(L"0");
    }
    hour.append(std::to_wstring(time_point.tm_hour));

    std::wstring min;
    if (time_point.tm_min < 10u)
    {
        min.append(L"0");
    }
    min.append(std::to_wstring(time_point.tm_min));

    return { day + L"-" + month + L"-" + year + L" " + hour + L":" + min };
}

void Chat::sendUnsignedNUM(size_t const num) const
{
    send(_socket, &num, sizeof(num), 0);
}

auto Chat::sendSignedNUM(long const num) const ->void
{
    send(_socket, &num, sizeof(num), 0);
}

auto Chat::sendRequest(REQUEST_TYPE const rt) const ->void
{
    send(_socket, &rt, sizeof(rt), 0);
}

auto Chat::sendString(std::wstring const& str) const ->void
{

    size_t strLen = str.size();

    if(strLen == 0) strLen += 1;

    send(_socket, &strLen, sizeof(strLen), 0);

    send(_socket, str.c_str(), strLen * sizeof(wchar_t), 0);
}

auto Chat::sendUserData(User& user) const ->void
{
    sendString(user.getLogin());
    sendString(user.getPass());
    sendString(user.getUsername());
}

auto Chat::sendMessage(Message& message) const ->void
{
    sendString(message.getSender());
    sendString(message.getReceiver());
    sendString(message.getContent());
    sendSignedNUM(message.getTime());
}

auto Chat::loadResponse() const ->Chat::RESPONSE_TYPE
{
    RESPONSE_TYPE response;
    if(recv(_socket, &response, sizeof(response), 0) == 0)
        throw std::runtime_error("Server is DOWN now\n");
    return response;
}

auto Chat::loadUnsignedNUM() const ->size_t
{
    size_t buff;
    if(recv(_socket, &buff, sizeof(buff), 0) == 0)
        throw std::runtime_error("Server is DOWN now");
    return  buff;
}

auto Chat::loadSignedNUM() const ->long
{
    long buff;
    if(recv(_socket, &buff, sizeof(buff), 0) == 0)
        throw std::runtime_error("Server is DOWN now");
    return buff;
}


auto Chat::loadString() const ->std::wstring
{
    size_t strLen;
    if(recv(_socket, &strLen, sizeof(strLen), 0) == 0)
        throw std::runtime_error("Server is DOWN now\n");
    auto* str = new wchar_t[strLen + 1]{};
    str[strLen] = '\0';
    if(recv(_socket, str, strLen * sizeof(wchar_t), 0) == 0)
        throw std::runtime_error("Server is DOWN now\n");
    std::wstring rez(str);
    delete[] str;
    return rez;
}

auto Chat::loadMessage() const ->Message
{
    auto sender = loadString();
    auto receiver = loadString();
    auto content = loadString();
    time_t time_ = loadSignedNUM();

    return {sender, receiver, content, std::move(time_)};
}


auto Chat::login()  ->void
{
    REQUEST_TYPE request = REQUEST_TYPE::SIGN_IN;
    sendRequest(request);
    flush_input_buffer();

    std::unique_ptr<User> user = std::make_unique<User>();
    std::wstring  login, password;

    std::wcout << L"Enter login\n> ";
    std::getline(std::wcin, login);
    user->setLogin(login);

    std::wcout << L"Enter password\n> ";
    std::getline(std::wcin, password);
    user->setPass(password);

    cls
    sendUserData(*user);

    if (loadResponse() == RESPONSE_TYPE::ACCEPTED)
    {
        user->setUsername(loadString());
        setConsoleColor(Color::GREEN);
        std::wcout << L"Login success\n\n";
        set_current_user(std::move(user));
        setConsoleColor(Color::BLUE);
        userLoop();
        return;
    }
    setConsoleColor(Color::RED);
    std::wcout << L"Incorrect login or password\n\n";

}

auto Chat::signUp()  ->void
{
    REQUEST_TYPE request = REQUEST_TYPE::SIGN_UP;
    sendRequest(request);
    flush_input_buffer();

    std::wregex regular(L"([A-Za-z0-9_]{4,15})");
    std::wstring login, password, username;

    std::wcout << L"Enter login | min;max length[4;15] | allowed characters[a-z A-Z 0-9 _] | (quit to quit):\n> ";
    if (!set_user_data(login, regular))
    {
        cls
        setConsoleColor(Color::RED);
        std::wcout << L"Registration has been denied\n\n";
        return;
    }
    std::wcout << L"Enter password | min;max length[4;15] | allowed characters[a-z A-Z 0-9 _] | (quit to quit):\n> ";
    if (!set_user_data(password, regular))
    {
        cls
        setConsoleColor(Color::RED);
        std::wcout << L"Registration has been denied\n\n";
        return;
    }

    std::wcout << L"Enter username | min;max length[4;15] | allowed characters[a-z A-Z 0-9 _] | (quit to quit):\n> ";
    if (!set_user_data(username, regular))
    {
        cls
        setConsoleColor(Color::RED);
        std::wcout << L"Registration has been denied\n\n";
        return;
    }

    cls
    std::unique_ptr<User> user = std::make_unique<User>(login, password, username);
    sendUserData(*user);
    if (loadResponse() == RESPONSE_TYPE::REJECTED)
    {
        setConsoleColor(Color::RED);
        std::wcout << L"Account with this name/login has already been registered\n\n";
        return;
    }

    setConsoleColor(Color::GREEN);
    std::wcout << L"Registration was successful\n\n";
}

auto Chat::getMessages()  ->void
{
    REQUEST_TYPE request = REQUEST_TYPE::GET_MESSAGES;
    sendRequest(request);
    flush_input_buffer();

    std::wstring chat_with;
    std::wcout << L"Enter username/chatname to get messages:\n> ";
    std::getline(std::wcin, chat_with);

    sendString(_currentUser->getUsername());
    sendString(chat_with);

    if (loadResponse() == RESPONSE_TYPE::REJECTED)
    {
        cls
        setConsoleColor(Color::RED);
        std::wcout << L"User/chat isn't existing\n\n";
        return;
    }
    cls
    size_t vecSize = loadUnsignedNUM();

    if (vecSize == 0)
    {
       setConsoleColor(Color::RED);
       std::wcout << L"You don't have any messages in this chat\n\n";
       return;
    }

    std::vector<Message> messages;
    messages.reserve(vecSize);

    for(size_t i = 0; i < vecSize; ++i)
    {
        messages.push_back(loadMessage());
    }


    /*Для разметки консоли*/
    Coord c{};
    c.X = 1;
    c.Y = 3;


    setConsoleColor(Color::BLUE);
    chatBox(chat_with, vecSize * 3u);


    setConsoleColor(Color::DEFAULT);
    for (auto const& message : messages)
    {
        SetConsoleCursor(c);
        std::wcout << message.getSender()
                   << L' ' << time_to_string(
                           const_cast<time_t&>(message.getTime()));
        c.Y += 1;
        SetConsoleCursor(c);
        std::wcout << message.getContent();
        c.Y += 2;
    }
    c.Y += 2;
    c.X = 0;

    SetConsoleCursor(c);

}

auto Chat::sendMessage()  ->void
{
    REQUEST_TYPE request = REQUEST_TYPE::SEND_MESSAGE;
    sendRequest(request);
    flush_input_buffer();


    std::wstring receiver, content;
    std::wcout << L"Enter username/chatname to send message:\n> ";
    std::getline(std::wcin, receiver);
    User temp(receiver);

    sendString(receiver);
    if (loadResponse() == RESPONSE_TYPE::REJECTED)
    {
        cls
        setConsoleColor(Color::RED);
        std::wcout << L"User/chat isn't existing\n\n";
        return;
    }
    cls

    std::wcout << L"Write message to " << receiver
               << L"{max symbols: " << _consoleWidth - 2 << L"}\n> ";

    std::getline(std::wcin, content);
    if (content.size() > (_consoleWidth - 2))
    {
        content.erase(content.begin() + (_consoleWidth - 2), content.end());
    }

    std::unique_ptr<Message> message = std::make_unique<Message>
            (_currentUser->getUsername(), receiver, content, time(nullptr));

    sendMessage(*message);

    cls
    setConsoleColor(Color::GREEN);
    std::wcout << "Message sent\n";
    std::wcout << std::endl;
}


auto Chat::logout()  ->void
{
    _bUserStatus = false;
    _currentUser.reset(nullptr);
}

auto Chat::exit()  ->void
{
    sendRequest(REQUEST_TYPE::EXIT);
    throw CloseChat{};
}



auto Chat::clear_chat()  ->void
{
    REQUEST_TYPE request = REQUEST_TYPE::CLEAR_CHAT;
    sendRequest(request);
    flush_input_buffer();

    std::wstring username;
    std::wcout << L"Enter username to clear chat with him:\n> ";
    std::getline(std::wcin, username);
    if (username == L"common_chat")
    {
        cls
        setConsoleColor(Color::RED);
        std::wcout << L"The shared chat cannot be cleared\n\n";
        return;
    }

    sendString(username);

    if (loadResponse() == RESPONSE_TYPE::REJECTED)
    {
        cls
        setConsoleColor(Color::RED);
        std::wcout << L"User/chat isn't existing\n\n";
        return;
    }

    bool done = false;
    wchar_t ch;
    while (!done)
    {
        std::wcout << L"Are you sure? Y/N\n> ";
        std::wcin >> ch;
        switch (ch)
        {
            case'y':
            case'Y':
                //_DB->clearChat(_currentUser->getUsername(), username);
                done = true;
                write(_socket, &done, sizeof(bool));
                break;
            case'n':
            case'N':
                done = false;
                write(_socket, &done, sizeof(bool));
                std::wcout << std::endl;
                return;
            default:
                flush_input_buffer();
                break;
        }
    }
    cls
    setConsoleColor(Color::GREEN);
    std::wcout << L"Chat has been cleared" << std::endl;
    std::wcout << std::endl;
}


auto Chat::changePassword()  ->void
{
    REQUEST_TYPE request = REQUEST_TYPE::CH_PASSWORD;
    sendRequest(request);
    flush_input_buffer();

    std::wregex regular(L"([A-Za-z0-9_]{4,15})");
    std::wstring newPassword;

    std::wcout << L"Enter new password | min;max length[4;15] | allowed characters[a-z A-Z 0-9 _] | (quit to quit):\n> ";
    if (!set_user_data(newPassword, regular))
    {
        cls
        setConsoleColor(Color::RED);
        std::wcout << L"Changing login has been denied\n\n";
        return;
    }

    _currentUser->setPass(newPassword);
    sendUserData(*_currentUser);

    cls
    if(loadResponse() == RESPONSE_TYPE::ACCEPTED)
    {
        setConsoleColor(Color::GREEN);
        std::wcout << L"Password has been changed" << std::endl;
        std::wcout << std::endl;
    }
}

auto Chat::changeLogin()  ->void
{
    REQUEST_TYPE request = REQUEST_TYPE::CH_LOGIN;
    sendRequest(request);
    flush_input_buffer();

    std::wregex regular(L"([A-Za-z0-9_]{4,15})");
    std::wstring newLogin;

    std::wcout << L"Enter new login | min;max length[4;15] | allowed characters[a-z A-Z 0-9 _] | (quit to quit):\n> ";
    if (!set_user_data(newLogin, regular))
    {
        cls
        setConsoleColor(Color::RED);
        std::wcout << L"Changing password has been denied\n\n";

        return;
    }

    User tempUser(*_currentUser);
    tempUser.setLogin(newLogin);
    sendUserData(tempUser);

    if (loadResponse() == RESPONSE_TYPE::REJECTED)
    {
        cls
        setConsoleColor(Color::RED);
        std::wcout << L"Account with this login has already exited\n\n";
        return;
    }
    cls

    _currentUser->setLogin(newLogin);
    setConsoleColor(Color::GREEN);
    std::wcout << L"Login has been changed" << std::endl;
    std::wcout << std::endl;
}


auto Chat::get_info()  ->void
{
    setConsoleColor(Color::DEFAULT);

    std::wcout << L'┌' << std::setfill(L'─') << std::setw(27u) << L'┐' << std::endl;

    std::wcout << L'│' << std::setfill(L' ') << std::right << L"Username: " << _currentUser->getUsername()
               << std::setw(27u - _currentUser->getUsername().size() - 10u) << L'│' << std::endl;
    std::wcout << L'│' << std::setfill(L' ') << std::right << std::setw(27u) << L'│' << std::endl;

    std::wcout << L'│' << std::setfill(L' ') << std::right << L"Login: " << _currentUser->getLogin()
               << std::setw(27u - _currentUser->getLogin().size() - 7u) << L'│' << std::endl;
    std::wcout << L'│' << std::setfill(L' ') << std::right << std::setw(27u) << L'│' << std::endl;

    std::wcout << L'│' << std::setfill(L' ') << std::right << L"Password: "<< _currentUser->getPass()
               << std::setw(27u - _currentUser->getPass().size() - 10u) << L'│' << std::endl;


    std::wcout << L'└' << std::setfill(L'─') << std::setw(27u) << L'┘' << std::endl;
    std::wcout << std::endl;
}

auto Chat::chatBox(std::wstring const& username, size_t const& count) const  -> void
{

    std::wcout << L'┌' << std::setfill(L'─') << std::setw(_consoleWidth - 1u) << L'┐' << std::endl;
    std::wcout << std::left;
    std::wcout << L'│' << std::setfill(L' ') << std::setw(_consoleWidth - 2u) << username << L'│' << std::endl;
    std::wcout << std::right;
    std::wcout << L'├' << std::setfill(L'─') << std::setw(_consoleWidth - 1u) << L'┤' << std::endl;
    std::wcout << std::left;

    for (size_t i = 0u; i < count; ++i)
        std::wcout << L'│' << std::setfill(L' ') << std::right << std::setw(_consoleWidth - 1u) << L'│' << std::endl;

    std::wcout << L'└' << std::setfill(L'─') << std::setw(_consoleWidth - 1u) << L'┘' << std::endl;
}



auto Chat::chatMenu() const  ->void
{
    setConsoleColor(Color::DEFAULT);
    std::wstring menu_words[] =
    {
            {L"Login"},
            {L"SignUp"},
            {L"Exit"}
    };

    setConsoleColor(Color::MAGENTA);

    std::wcout << L'┌' << std::setfill(L'─') << std::setw(27u) << L'┐' << std::endl;
    for (size_t i = 0u, j = 0; i < 5u; ++i)
    {
        if (i % 2 == 0u)
        {
            std::wcout << L'│' << std::setfill(L' ') << j + 1 << "." << menu_words[j]
                       << std::right << std::setw(27u - menu_words[j++].size() - 2u) << L'│' << std::endl;
            continue;
        }
        std::wcout << L'│' << std::setfill(L' ') << std::right << std::setw(27u) << L'│' << std::endl;
    }

    std::wcout << L'└' << std::setfill(L'─') << std::setw(27u) << L'┘' << std::endl;
    std::wcout << L"> ";


}

auto Chat::userMenu() const  ->void
{
    std::wstring menu_words[] =
    {
            {L"Send message"},
            {L"Get messages"},
            {L"Get info about account"},
            {L"Clear chat"},
            {L"Change password"},
            {L"Change login"},
            {L"Logout"}
    };

    setConsoleColor(Color::GREEN);

    std::wcout << L'┌' << std::setfill(L'─') << std::setw(27u) << L'┐' << std::endl;
    for (size_t i = 0u, j = 0; i < 13u; ++i)
    {
        if (i % 2 == 0u)
        {
            std::wcout << L'│' << std::setfill(L' ') << j + 1u << "." << menu_words[j]
                       << std::right << std::setw(27u - menu_words[j++].size() - 2u) << L'│' << std::endl;
            continue;
        }
        std::wcout << L'│' << std::setfill(L' ') << std::right << std::setw(27u) << L'│' << std::endl;
    }

    std::wcout << L'└' << std::setfill(L'─') << std::setw(27u) << L'┘' << std::endl;
    std::wcout << L"> ";

}

auto Chat::action_for_chat()  ->void
{
    void (Chat::*fact[])() = {
            &Chat::login, &Chat::signUp,
            &Chat::exit
    };
    int act;
    while (!(std::wcin >> act) || act < 1 || act > 3)
    {
        setConsoleColor(Color::RED);
        std::wcin.clear();
        std::wcout << L"Bad input, try again\n> ";
        flush_input_buffer();
        setConsoleColor(Color::DEFAULT);
    }

    _state = fact[act - 1];
}


auto Chat::action_for_user()  ->void
{
    void (Chat:: * fact[])() = {
            &Chat::sendMessage,     &Chat::getMessages,
            &Chat::get_info,        &Chat::clear_chat,
            &Chat::changePassword,  &Chat::changeLogin,
            &Chat::logout
    };
    int act;
    while (!(std::wcin >> act) || act < 1 || act > 7)
    {
        setConsoleColor(Color::RED);
        std::wcin.clear();
        std::wcout << L"Bad input, try again\n> ";
        flush_input_buffer();
        setConsoleColor(Color::DEFAULT);
    }

    _state = fact[act - 1];
}


auto Chat::userLoop()  ->void
{
    _bUserStatus = true;
    while (_bUserStatus)
    {
        userMenu();
        action_for_user();
        cls
        (this->*_state)();
    }
}

auto Chat::chatLoop()  ->void
{
    try
    {
        while (true)
        {

            chatMenu();
            action_for_chat();
            cls
            (this->*_state)();
        }
    }
    catch (CloseChat const&)
    {
        cls;
        std::wcerr << L"Chat closing.....\n\n";
    }

}

auto Chat::run()  ->void
{
    bool rez;

    _socket = socket(AF_INET, SOCK_STREAM, 0);

    if(_socket == -1) throw std::runtime_error("socket error");

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(55555);
    serverAddr.sin_family = AF_INET;


    int connection = connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    if(connection == -1) throw std::runtime_error("connection error");

    chatLoop();
}


