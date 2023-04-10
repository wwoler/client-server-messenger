#include"User.h"

User::User(std::string const& login, std::string const& pass):
	_login(login), _pass(pass) {}

User::User(std::string const& login, std::string const& pass, std::string const& username):
    _login(login),
    _pass(pass),
    _username(username) {}

User::User(std::string const& username):
    _username(username) {}

auto User::getLogin() const	                                    ->std::string const&
{
	return _login;
}

auto User::getPass() const	                                    ->std::string const&
{
	return _pass;
}

auto User::getUsername() const	                                ->std::string const&
{
	return _username;
}

auto User::setLogin(std::string const& login)              ->void
{
	_login.assign(login);
}
auto User::setPass(std::string const& pass)                ->void
{
	_pass.assign(pass);
}
auto User::setUsername(std::string const& username)        ->void
{
	_username.assign(username);
}