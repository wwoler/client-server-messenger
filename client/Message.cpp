#include "Message.h"

Message::Message(std::string const& sender, std::string const& receiver, std::string const& content, time_t const time):
	_sender(sender),
    _receiver(receiver),
    _content(content),
    _time(time) {}

Message::Message(std::string const& sender, std::string const& content, time_t const time):
	_sender(sender),
    _content(content),
    _time(time),
    _receiver() {}


auto Message::getSender() const  ->std::string const&
{
	return _sender;
}

auto Message::getReceiver() const ->std::string const&
{
	return _receiver;
}

auto Message::getContent() const ->std::string const&
{
	return _content;
}

auto Message::getTime() const ->time_t const&
{
	return _time;
}

auto Message::setSender(std::string const& sender)->void
{
	_sender.assign(sender);
}

auto Message::setReceiver(std::string const& receiver)->void
{
	_receiver.assign(receiver);
}

auto Message::setContent(std::string const& content)->void
{
	_content.assign(content);
}

auto Message::setTime(time_t const& time)->void
{
	_time = time;
}