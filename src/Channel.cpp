/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igilbert <igilbert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 13:06:15 by igilbert          #+#    #+#             */
/*   Updated: 2026/08/01 12:47:27 by igilbert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel(std::string name) : _name(name), _topic(""), _members(), _operators(), _invited(), _inv_only(false), Nuser(0), _password(""), _limit(-1), _topic_onlyOP(false) {}

Channel::Channel(const Channel &other) : _name(other._name), _topic(other._topic), _members(other._members), _operators(other._operators), _invited(other._invited), _inv_only(other._inv_only), Nuser(other.Nuser), _password(other._password), _limit(other._limit), _topic_onlyOP(other._topic_onlyOP) {}

Channel &Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_topic = other._topic;
		_members = other._members;
		_operators = other._operators;
		_invited = other._invited;
		_inv_only = other._inv_only;
		Nuser = other.Nuser;
		_password = other._password;
		_limit = other._limit;
		_topic_onlyOP = other._topic_onlyOP;
	}
	return *this;
}

Channel::~Channel() {}

std::string Channel::GetName() const
{
	return _name;
}

void Channel::SetPassword(std::string pass)
{
	_password = pass;
}

void Channel::SetLimit(int limit)
{
	_limit = limit;
}

int Channel::GetLimit() const
{
	return _limit;
}

bool Channel::CheckPassword(std::string pass) const
{
	return (pass == _password);
}

bool Channel::AddMember(Client *client, std::string pass)
{
	if (!_password.empty() && !CheckPassword(pass))
	{
		client->AppendOutBuffer(":ircserv 475 " + client->GetNickname() + " " + _name + " :Cannot join channel (+k) - incorrect key\r\n");
		return false;
	}
	if (_limit >= 0 && Nuser >= _limit)
	{
		client->AppendOutBuffer(":ircserv 471 " + client->GetNickname() + " " + _name + " :Cannot join channel (+l) - channel is full\r\n");
		return false;
	}
	if (_members.empty() && _operators.empty())
	{
		_operators.push_back(client);
	}
	if (std::find(_members.begin(), _members.end(), client) == _members.end())
		_members.push_back(client);
	Nuser++;
	return true;
}

void Channel::RemoveMember(Client *client)
{
	_members.erase(std::remove(_members.begin(), _members.end(), client), _members.end());
	_operators.erase(std::remove(_operators.begin(), _operators.end(), client), _operators.end());
	Nuser--;
}

void Channel::BroadcastMessage(std::string message, Client *exclude)
{
	for (std::list<Client *>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if (*it != exclude)
		{
			(*it)->AppendOutBuffer(message);
		}
	}
}

std::string Channel::GetMemberList()
{
	std::string list = "";
	for (std::list<Client *>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		bool isOP = false;
		for (std::list<Client *>::iterator opIt = _operators.begin(); opIt != _operators.end(); ++opIt)
		{
			if (*it == *opIt)
			{
				isOP = true;
				break;
			}
		}
		if (isOP)
			list += "@";
		list += (*it)->GetNickname() + " ";
	}
	return list;
}

void Channel::SetTopic(std::string topic)
{
	_topic = topic;
}

std::string Channel::GetTopic() const
{
	return _topic;
}

void Channel::SetInvOnly(bool inv_only)
{
	_inv_only = inv_only;
}

bool Channel::getInv_only() const
{
	return _inv_only;
}

void Channel::SetTopicOnlyOP(bool topic_onlyOP)
{
	_topic_onlyOP = topic_onlyOP;
}

bool Channel::getTopicOnlyOP() const
{
	return _topic_onlyOP;
}

bool Channel::IsMember(Client *client) const
{
    return (std::find(_members.begin(), _members.end(), client) != _members.end());
}

std::list<Client *> Channel::GetInvited() const
{
	return _invited;
}

void Channel::setInvited(Client *client)
{
	if (std::find(_invited.begin(), _invited.end(), client) == _invited.end())
		_invited.push_back(client);
}

bool Channel::IsOPbyNick(std::string nick)
{
	for (std::list<Client *>::iterator opIt = this->_operators.begin(); opIt != this->_operators.end(); ++opIt)
	{
		if ((*opIt)->GetNickname() == nick)
			return true;
	}
	return false;
}

bool Channel::SetOP(std::string nick)
{
	for (std::list<Client *>::iterator it = this->_members.begin(); it != this->_members.end(); ++it)
	{
		if ((*it)->GetNickname() == nick)
		{
			if (std::find(this->_operators.begin(), this->_operators.end(), *it) == this->_operators.end())
				this->_operators.push_back(*it);
			return true;
			break;
		}
	}
	return false;
}

bool Channel::DeOP(std::string nick)
{
	for (std::list<Client *>::iterator it = this->_members.begin(); it != this->_members.end(); ++it)
	{
		if ((*it)->GetNickname() == nick)
		{
			_operators.erase(std::remove(_operators.begin(), _operators.end(), *it), _operators.end());
			break;
		}
	}
	return true;
}