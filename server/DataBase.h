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

	auto isExisting(User& userID)                                            ->bool;

	auto clearChat(std::wstring const& user1, std::wstring const& user2)     ->void;

	auto getMessages(std::wstring const& from, std::wstring const& to)       ->std::vector<Message>;

	auto sendMessage(Message& message)                                       ->void;

	auto adjustment()                                                        ->void;

    auto getUserStreamPos()                                                  ->std::streampos&;

    auto changeUserData(User& newData, std::streampos const& pos)            ->void;

    auto lock()                                                              ->void;

    auto unlock()                                                            ->void;


protected:
	explicit DataBase(std::filesystem::path const&& path = L"./base/");

public:
	~DataBase() override
	{
		_userStream.close();
        if(_messageStream.is_open())
            _messageStream.close();
	}
};