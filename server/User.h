#pragma once
#include <iostream>

class User final {
public:
	User() = default;
	User(const std::wstring& login, const std::wstring& pass);
	User(std::wstring const& login,std::wstring const& pass, std::wstring const& username);
	User(std::wstring const& username);
	~User() = default;

	auto getLogin() const				           ->std::wstring const&;
	auto getPass()  const			               ->std::wstring const&;
	auto getUsername() 	const		               ->std::wstring const&;

	auto setLogin(std::wstring const&)             ->void;
	auto setPass(std::wstring const&)              ->void;
	auto setUsername(std::wstring const&)          ->void;

private:
	std::wstring _login;
	std::wstring _pass;
	std::wstring _username;
};