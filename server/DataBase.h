#pragma once

#include "Singleton.h"
#include "User.h"
#include "Message.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <memory>
#include <iostream>
#include <chrono>
#include <mutex>
#include <nlohmann/json.hpp>


class DataBase : public Singleton<DataBase>
{
private:
    enum class TypeExisting : uint8_t
    {
        login,
        username,
        login_and_username,
    };

private:
	friend class Connection;
        std::mutex                      _mutex;
	std::fstream                    _userStream;
	std::fstream                    _messageStream;
	std::filesystem::path           _path;
	std::streampos                  _currentUserPos;

	auto to_json(User& user)                                                 ->void;
	auto to_json(Message& mess)                                              ->void;
	auto from_json(User& user)                                               ->void;
	auto from_json(Message& mess)                                            ->void;

	auto login(User& userID)                                                 ->bool;

	auto signUp(User& userID)                                                ->bool;

	auto isExisting(User& , TypeExisting,  User* resultData = nullptr)       ->bool;

	auto clearChat(std::string const& user1, std::string const& user2)       ->bool;

	auto getMessages(std::string const& from, std::string const& to,
                     std::vector<Message>& messages)                             ->bool;

	auto sendMessage(Message& message)                                       ->bool;

	auto adjustment()                                                        ->void;

        auto getUserStreamPos()                                                  ->std::streampos&;

    	auto changeLogin(User&, std::streampos const&)                           ->bool;

        auto changePassword(User&, std::streampos const&)                        ->void;
protected:
	explicit DataBase(std::filesystem::path const&& path = "./base/");

public:
	~DataBase() override
	{
	    _userStream.close();
        if(_messageStream.is_open())
            _messageStream.close();
	}
};
