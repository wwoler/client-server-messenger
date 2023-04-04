#pragma once
#include<iostream>
#include"User.h"

class Message final {
public:
	Message() = default;
	Message(std::string const& sender, std::string const& receiver, std::string const& content, time_t const time);
	Message(std::string const& sender, std::string const& content, time_t const time);
	~Message() = default;

	auto getSender()    const	->std::string const&;
	auto getReceiver()  const   ->std::string const&;
	auto getContent()  	const   ->std::string const&;
	auto getTime()      const   ->time_t      const&;

	auto setSender(std::string const&)   ->void;
	auto setReceiver(std::string const&) ->void;
	auto setContent(std::string const&)  ->void;
	auto setTime(time_t const&)          ->void;

private:
	std::string        _sender;
	std::string        _receiver;
	std::string        _content;
	time_t              _time;

};