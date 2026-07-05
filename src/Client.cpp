/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcarlier <tcarlier@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 15:16:37 by tcarlier          #+#    #+#             */
/*   Updated: 2026/07/05 19:06:09 by tcarlier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::~Client() {}

Client::Client() 
    :
    _Fd(-1),
    _ipAddress(""),
    _username(""),
    _nickname(""),
    _hostname(""),
	_password(""),
	_registered(false),
    _isAuth(false),
    _isLog(false),
    _buffer("")
{}

Client::Client(std::string IPadd) 
    :
	_Fd(-1),
    _ipAddress(IPadd),
    _username(""),
	_nickname(""),
	_hostname(""),
    _password(""),
	_registered(false),
    _isAuth(false),
    _isLog(false),
    _buffer("")
{}

Client::Client(const Client &other) 
{
    *this = other;
}

Client &Client::operator=(const Client &other) 
{
    if (this != &other) 
    {
        this->_Fd = other._Fd;
        this->_ipAddress = other._ipAddress;
        this->_username = other._username;
        this->_nickname = other._nickname;
        this->_hostname = other._hostname;
        this->_password = other._password;
        this->_registered = other._registered;
        this->_isAuth = other._isAuth;
        this->_isLog = other._isLog;
    }
    return *this;
}

void Client::SetFd(int fd)
{
    this->_Fd = fd;
}

void Client::SetIPadd(std::string IPadd)
{
    this->_ipAddress = IPadd;
}

void Client::SetUsername(std::string username)
{
    this->_username = username;
}

void Client::SetNickname(std::string nickname)
{
    this->_nickname = nickname;
}

void Client::SetHostname(std::string hostname)
{
    this->_hostname = hostname;
}

void Client::SetPassword(std::string password)
{
    this->_password = password;
}

void Client::SetRegistered(bool registered)
{
    this->_registered = registered;
}

void Client::SetAuth(bool auth)
{
    this->_isAuth = auth;
}

void Client::SetLog(bool log)
{
    this->_isLog = log;
}

int Client::GetFd(void)
{
    return this->_Fd;
}

std::string Client::GetIPadd(void)
{
    return this->_ipAddress;
}

std::string Client::GetUsername(void)
{
    return this->_username;
}               

std::string Client::GetNickname(void)
{
    return this->_nickname;
}

std::string Client::GetHostname(void)
{
    return this->_hostname;
}

std::string Client::GetPassword(void)
{
    return this->_password;
}

bool Client::GetRegistered(void)
{
    return this->_registered;
}

bool Client::GetAuth(void)
{
    return this->_isAuth;
}

bool Client::GetLog(void)
{
    return this->_isLog;
}
