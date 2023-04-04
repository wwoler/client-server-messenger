#pragma once
#include <iostream>

class User final {
public:
	User() = default;
	User(std::string const& login, std::string const& pass);
	User(std::string const& login, std::string const& pass, std::string const& username);
	User(std::string const& username);
	~User() = default;

	auto getLogin() const				           ->std::string const&;
	auto getPass()  const			               ->std::string const&;
	auto getUsername() 	const		               ->std::string const&;

	auto setLogin(std::string const&)              ->void;
	auto setPass(std::string const&)               ->void;
	auto setUsername(std::string const&)           ->void;

private:
	std::string _login;
	std::string _pass;
	std::string _username;
};