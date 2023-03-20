#include "Message.h"


Message::Message(std::wstring const& sender, std::wstring const& receiver, std::wstring const& content, time_t&& time):
	_sender(sender),
    _receiver(receiver),
    _content(content),
    _time(time) {}

Message::Message(std::wstring const& sender, std::wstring const& content, time_t&& time):
	_sender(sender),
    _content(content),
    _time(time),
    _receiver() {}


auto Message::getSender() const  ->std::wstring const&
{
	return _sender;
}

auto Message::getReceiver() const ->std::wstring const&
{
	return _receiver;
}

auto Message::getContent() const ->std::wstring const&
{
	return _content;
}

auto Message::getTime() const ->time_t const&
{
	return _time;
}

auto Message::setSender(std::wstring const& sender)->void
{
	_sender.assign(sender);
}

auto Message::setReceiver(std::wstring const& receiver)->void
{
	_receiver.assign(receiver);
}

auto Message::setContent(std::wstring const& content)->void
{
	_content.assign(content);
}

auto Message::setTime(time_t const& time)->void
{
	_time = time;
}