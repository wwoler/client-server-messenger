#pragma once
#include<iostream>

class Message final {
public:
    Message() = default;
    Message(std::wstring const& sender, std::wstring const& receiver, std::wstring const& content, time_t&& time);
    Message(std::wstring const& sender, std::wstring const& content, time_t&& time);
    ~Message() = default;

    auto getSender()    const	->std::wstring const&;
    auto getReceiver()  const   ->std::wstring const&;
    auto getContent()  	const   ->std::wstring const&;
    auto getTime()      const   ->time_t       const&;

    auto setSender(std::wstring const&)   ->void;
    auto setReceiver(std::wstring const&) ->void;
    auto setContent(std::wstring const&)  ->void;
    auto setTime(time_t const&)           ->void;

private:
    std::wstring        _sender;
    std::wstring        _receiver;
    std::wstring        _content;
    time_t              _time;

};