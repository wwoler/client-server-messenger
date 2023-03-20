#include"User.h"

User::User(std::wstring const& login, std::wstring const& pass):
	_login(login), _pass(pass) {}

User::User(std::wstring const& login,std::wstring const& pass, std::wstring const& username):
    _login(login),
    _pass(pass),
    _username(username) {}

User::User(std::wstring const& username):
    _username(username) {}

auto User::getLogin() const	                                    ->std::wstring const&
{
	return _login;
}

auto User::getPass() const	                                    ->std::wstring const&
{
	return _pass;
}

auto User::getUsername() const	                                ->std::wstring const&
{
	return _username;
}

auto User::setLogin(std::wstring const& login)              ->void
{
	_login.assign(login);
}
auto User::setPass(std::wstring const& pass)                ->void
{
	_pass.assign(pass);
}
auto User::setUsername(std::wstring const& username)        ->void
{
	_username.assign(username);
}