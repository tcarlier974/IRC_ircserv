/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igilbert <igilbert@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 13:06:15 by igilbert          #+#    #+#             */
/*   Updated: 2026/07/31 10:38:22 by igilbert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel(std::string name) : _name(name), _topic(""), _members(), _operators() {}

Channel::Channel(const Channel &other) : _name(other._name), _topic(other._topic), _members(other._members), _operators(other._operators) {}

Channel &Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_topic = other._topic;
		_members = other._members;
		_operators = other._operators;
	}
	return *this;
}

Channel::~Channel() {}

std::string Channel::GetName() const
{
	return _name;
}

void Channel::AddMember(Client *client)
{
	if (_members.empty() && _operators.empty())
	{
		_operators.push_back(client);
	}
	if (std::find(_members.begin(), _members.end(), client) == _members.end())
		_members.push_back(client);
	Nuser++;
	std::cout << "number of users: " << Nuser << std::endl;
}

void Channel::RemoveMember(Client *client)
{
	_members.erase(std::remove(_members.begin(), _members.end(), client), _members.end());
}

void Channel::BroadcastMessage(std::string message, Client *exclude)
{
	for (std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
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
    for (std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
    { 
        bool isOP = false;
		for (std::vector<Client*>::iterator opIt = _operators.begin(); opIt != _operators.end(); ++opIt)
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

bool Channel::IsOPbyNick(std::string nick)
{
	for (std::vector<Client*>::iterator opIt = this->_operators.begin(); opIt != this->_operators.end(); ++opIt)
	{
		if ((*opIt)->GetNickname() == nick)
			return true;
	}
	return false;
}

void Channel::SetOP(Client *client)
{
	_operators.push_back(client);
}