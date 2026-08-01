/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcarlier <tcarlier@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 15:18:38 by tcarlier          #+#    #+#             */
/*   Updated: 2026/08/01 10:47:34 by tcarlier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

bool Server::_sig = false;

Server::Server() : _Port(-1),
				   _SerSocketFd(-1),
				   _Password(""),
				   _Clients(),
				   _fds(),
				   _NextChannelId(0),
				   _hostName(""),
				   _ClientNames(),
				   _Topics()
// _parsedMessages(),
// _Channels()
{
}

Server::Server(char **av) : _SerSocketFd(-1),
							_Clients(),
							_fds(),
							_NextChannelId(0),
							_hostName(""),
							_ClientNames(),
							_Topics()
// _parsedMessages(),
// _Channels()
{
	_Port = setPort(av[1]);
	_Password = setPassword(av[2]);
}

Server::Server(const Server &other)
{
	if (this != &other)
	{
		*this = other;
	}
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_Port = other._Port;
		_Password = other._Password;
		_SerSocketFd = other._SerSocketFd;
		_NextChannelId = other._NextChannelId;
		_hostName = other._hostName;
		_ClientNames = other._ClientNames;
		_Topics = other._Topics;
		// _parsedMessages = other._parsedMessages;
		_Clients = other._Clients;
		_fds = other._fds;
		// _Channels = other._Channels;
	}
	return *this;
}

Server::~Server()
{
	if (_SerSocketFd != -1)
		close(_SerSocketFd);
}

int Server::setPort(char *port)
{
	int portNum = atoi(port);
	if (portNum <= 1023 || portNum >= 49152)
	{
		std::cerr << RED << "Error: Port number must be between 1024 and 49151." << WHI << std::endl;
		exit(EXIT_FAILURE);
	}
	return portNum;
}

std::string Server::setPassword(char *password)
{
	if (std::string(password).empty())
	{
		std::cerr << RED << "Error: Password cannot be empty." << WHI << std::endl;
		exit(EXIT_FAILURE);
	}
	return std::string(password);
}

void Server::ServerInit()
{
	SerSocket();

	std::cout << GRE << "Server <" << _SerSocketFd << "> Connected" << WHI << std::endl;
	std::cout << "Waiting to accept a connection...\n";
}

void Server::SerSocket()
{
	int en = 1;
	struct sockaddr_in serverAddr;
	struct pollfd NewPollFd;
	memset(&serverAddr, 0, sizeof(serverAddr));
	//serverAddr.sin_len = sizeof(serverAddr); //marche que sur macOS
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(this->_Port);

	_SerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_SerSocketFd < 0)
		throw std::runtime_error("Error: Failed to create socket.");

	if (setsockopt(_SerSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) < 0)
		throw std::runtime_error("Error: Failed to set socket options.");
	if (fcntl(_SerSocketFd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error: Failed to set socket to non-blocking mode.");
	if (bind(_SerSocketFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
		throw std::runtime_error("Error: Failed to bind socket.");
	if (listen(_SerSocketFd, SOMAXCONN) < 0)
		throw std::runtime_error("Error: Failed to listen on socket.");

	NewPollFd.fd = _SerSocketFd;
	NewPollFd.events = POLLIN;
	NewPollFd.revents = 0;
	_fds.push_back(NewPollFd);
}

void Server::SigHandler(int signum)
{
	(void)signum;
	std::cout << YEL << "Signal recieved ! Shutting down the server.\n"
			  << WHI << std::endl;
	Server::_sig = true;
}

void Server::Closefds()
{
	for (std::list<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
	{
		std::cout << RED << "Closing connection with client: " << it->GetNickname() << WHI << std::endl;
		close(it->GetFd());
	}
	if (_SerSocketFd != -1)
	{
		std::cout << YEL << "Closing server socket: " << _SerSocketFd << WHI << std::endl;
		close(_SerSocketFd);
	}
}

void Server::AcceptNewClient()
{
	Client newClient;
	struct sockaddr_in clientAddr;
	struct pollfd NewPollFd;
	socklen_t addrLen = sizeof(clientAddr);
	int incomingFd = accept(_SerSocketFd, (struct sockaddr *)&clientAddr, &addrLen);
	if (incomingFd < 0)
	{
		std::cerr << RED << "Error: Failed to accept new client." << WHI << std::endl;
		return;
	}
	if (fcntl(incomingFd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << RED << "Error: Failed to set new client socket to non-blocking mode." << WHI << std::endl;
		close(incomingFd);
		return;
	}
	NewPollFd.fd = incomingFd;
	NewPollFd.events = POLLIN | POLLOUT;
	NewPollFd.revents = 0;

	newClient.SetFd(incomingFd);
	newClient.SetIPadd(inet_ntoa(clientAddr.sin_addr));
	_Clients.push_back(newClient);
	_fds.push_back(NewPollFd);
}

bool Server::getSig() const
{
	return Server::_sig;
}

void Server::SendMessage(int fd, std::string msg)
{
	msg += "\r\n";
	send(fd, msg.c_str(), msg.size(), 0);
}

void Server::ParseMessage(std::string message, Client *client)
{
	std::istringstream iss(message);
	std::string command;
	iss >> command;

	if (command == "CAP")
	{
		std::string subCommand;
		iss >> subCommand;
		if (subCommand == "LS")
		{
			// client->AppendOutBuffer("CAP * LS :\r\n");
		}
	}
	else if (command == "PASS" || command == "pass")
	{
		std::string password;
		iss >> password;
		if (password == this->_Password)
		{
			client->SetRegistered(true);
		}
		else
		{
			client->AppendOutBuffer(":ircserv 464 : Password incorrect\r\n");
		}
	}
	else if (command == "NICK")
	{
		std::string nickname;
		iss >> nickname;

		for (size_t i = 0; i < nickname.length(); ++i)
		{
			if (!isalnum(nickname[i]) && nickname[i] != '-' && nickname[0] != '-' && nickname[i] != '_' && nickname[i] != '[' && nickname[i] != ']' && nickname[i] != '\\' && nickname[i] != '`' && nickname[i] != '{' && nickname[i] != '}')
			{
				client->AppendOutBuffer(":ircserv 432 * " + nickname + " :Erroneous nickname\r\n");
				return;
			}
		}

		if (_ClientNames.find(nickname) != _ClientNames.end())
		{
			client->AppendOutBuffer(":ircserv 433 * " + nickname + " :Nickname is already in use\r\n");
			return;
		}
		if (!client->GetNickname().empty())
			_ClientNames.erase(client->GetNickname());

		client->AppendOutBuffer(":" + client->GetNickname() + " NICK :" + nickname + "\r\n");
		client->SetNickname(nickname);
		_ClientNames.insert(nickname);
	}
	else if (command == "USER")
	{
		std::string username, mode, unused, realname;
		iss >> username >> mode >> unused;
		std::getline(iss, realname);

		if (!username.empty())
			client->SetUsername(username);
	}
	else if (command == "PING")
	{
		std::string token;
		iss >> token;
		client->AppendOutBuffer(":ircserv PONG " + token + "\r\n");
	}
	else if (command == "PRIVMSG")
	{
		std::string target, text;
		iss >> target;
		std::getline(iss, text);

		if (!text.empty() && text[0] == ' ')
			text = text.substr(1);
		if (!text.empty() && text[0] == ':')
			text = text.substr(1);

		// channel
		if (target[0] == '#')
		{
			Channel *targetChannel = NULL;
			for (std::vector<Channel>::iterator it = _Channels.begin(); it != _Channels.end(); ++it)
			{
				if (it->GetName() == target)
				{
					targetChannel = &(*it);
					break;
				}
			}

			if (targetChannel)
			{
				std::string msg = ":" + client->GetNickname() + " PRIVMSG " + target + " :" + text + "\r\n";
				// On utilise BroadcastMessage en excluant l'expéditeur
				targetChannel->BroadcastMessage(msg, client);
			}
			else
			{
				client->AppendOutBuffer(":ircserv 401 " + client->GetNickname() + " " + target + " :No such nick/channel\r\n");
			}
		}
		// private msg
		else
		{
			Client *targetClient = getClientByNick(target);
			if (targetClient)
			{
				std::string forwardMsg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " PRIVMSG " + target + " :" + text + "\r\n";
				targetClient->AppendOutBuffer(forwardMsg);

				std::cout << "Message routé de " << client->GetNickname() << " vers " << target << std::endl;
			}
			else
			{
				client->AppendOutBuffer(":ircserv 401 " + client->GetNickname() + " " + target + " :No such nick/channel\r\n");
				std::cout << "Destinataire introuvable : " << target << std::endl;
			}
		}
	}
	else if (command == "QUIT")
	{
		std::string reason;
		std::getline(iss, reason);
		if (!reason.empty() && reason[0] == ':')
			reason = reason.substr(1);
		for (std::vector<Channel>::iterator it = _Channels.begin(); it != _Channels.end(); ++it)
		{
			it->RemoveMember(client);
			std::string quitMsg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " QUIT :" + reason + "\r\n";
			it->BroadcastMessage(quitMsg, client);
		}
		int fd = client->GetFd();
		close(fd);
		ClearClients(fd);
		return;
	}
	else if (command == "JOIN")
	{
		std::string channelName;
		std::string key = "";
		iss >> channelName;
		if (iss >> key)
		{
		}
		if (!client->GetRegistered())
		{
			client->AppendOutBuffer(":ircserv 451 " + client->GetNickname() + " :You have not registered\r\n");
			return;
		}
		if (!channelName.empty())
		{
			if (channelName[0] != '#')
			{
				channelName = "#" + channelName;
			}
			Channel *channel = NULL;
			for (std::vector<Channel>::iterator it = _Channels.begin(); it != _Channels.end(); ++it)
			{
				if (it->GetName() == channelName)
				{
					channel = &(*it);
					break;
				}
			}

			if (!channel)
			{
				_Channels.push_back(Channel(channelName));
				channel = &_Channels.back();
			}
			std::list<Client *> invitedList = channel->GetInvited();
			if (channel->getInv_only() && std::find(invitedList.begin(), invitedList.end(), client) == invitedList.end())
			{
				client->AppendOutBuffer(":ircserv 473 " + client->GetNickname() + " " + channelName + " :Cannot join channel (+i)\r\n");
				return;
			}
			channel->AddMember(client, key);
			std::string joinMsg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " JOIN :" + channelName + "\r\n";
			channel->BroadcastMessage(joinMsg);

			std::string rpl_namreply = ":ircserv 353 " + client->GetNickname() + " = " + channelName + " :" + channel->GetMemberList() + "\r\n";
			client->AppendOutBuffer(rpl_namreply);

			std::string rpl_endofnames = ":ircserv 366 " + client->GetNickname() + " " + channelName + " :End of /NAMES list.\r\n";
			client->AppendOutBuffer(rpl_endofnames);
		}
	}
	else if (command == "TOPIC")
	{
		std::string channelName;
		iss >> channelName;
		std::string text;

		if (!channelName.empty())
		{
			if (channelName[0] != '#')
			{
				channelName = "#" + channelName;
			}
			Channel *channel = NULL;
			for (std::vector<Channel>::iterator it = _Channels.begin(); it != _Channels.end(); ++it)
			{
				if (it->GetName() == channelName)
				{
					channel = &(*it);
					break;
				}
			}

			if (channel && channel->GetMemberList().find(client->GetNickname()) != std::string::npos)
			{
				if (getline(iss, text) && !text.empty())
				{
					if (text[0] == ' ')
						text = text.substr(1);
					if (text[0] == ':')
						text = text.substr(1);
					if (channel->getTopicOnlyOP() && !channel->IsOPbyNick(client->GetNickname()) && !text.empty())
					{
						client->AppendOutBuffer(":ircserv 482 " + client->GetNickname() + " " + channelName + " :You're not a channel operator (+t)\r\n");
						return;
					}
					channel->SetTopic(text);
					channel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " TOPIC " + channelName + " :" + channel->GetTopic() + "\r\n");
				}
			}
			else
			{
				client->AppendOutBuffer(":ircserv 403 " + client->GetNickname() + " " + channelName + " :No such channel\r\n");
			}
			if (channel && text.empty() && !channel->GetTopic().empty())
			{
				client->AppendOutBuffer(":ircserv 332 " + client->GetNickname() + " " + channelName + " :" + channel->GetTopic() + "\r\n");
			}
		}
	}
	else if (command == "INVITE")
	{
		if (!client->GetRegistered())
		{
			client->AppendOutBuffer(":ircserv 451 " + client->GetNickname() + " :You have not registered\r\n");
			return;
		}
		std::string targetNick, channelName;
		iss >> targetNick >> channelName;
		if (targetNick.empty() || channelName.empty())
		{
			client->AppendOutBuffer(":ircserv 461 " + client->GetNickname() + " INVITE :Not enough parameters\r\n");
			return;
		}
		if (channelName[0] != '#')
		{
			channelName = "#" + channelName;
		}
		Channel *channel = NULL;
		for (std::vector<Channel>::iterator it = _Channels.begin(); it != _Channels.end(); ++it)
		{
			if (it->GetName() == channelName)
			{
				channel = &(*it);
				break;
			}
		}
		if (channel->getInv_only() && !channel->IsOPbyNick(client->GetNickname()))
		{
			client->AppendOutBuffer(":ircserv 482 " + client->GetNickname() + " " + channelName + " :You're not a channel operator\r\n");
			return;
		}
		Client *targetClient = getClientByNick(targetNick);
		channel->setInvited(targetClient);
		targetClient->AppendOutBuffer(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " INVITE " + targetNick + " :" + channelName + "\r\n");
		client->AppendOutBuffer(":ircserv 341 " + client->GetNickname() + " " + targetNick + " :" + channelName + "\r\n");
	}
	else if (command == "NOTICE")
	{
		std::string target, text;
		iss >> target;
		std::getline(iss, text);

		if (!text.empty() && text[0] == ' ')
			text = text.substr(1);
		if (!text.empty() && text[0] == ':')
			text = text.substr(1);

		// channel
		if (target[0] == '#')
		{
			Channel *targetChannel = NULL;
			for (std::vector<Channel>::iterator it = _Channels.begin(); it != _Channels.end(); ++it)
			{
				if (it->GetName() == target)
				{
					targetChannel = &(*it);
					break;
				}
			}

			if (targetChannel)
			{
				std::string msg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " NOTICE " + target + " :" + text + "\r\n";
				targetChannel->BroadcastMessage(msg, client);
			}
		}
		else
		{
			Client *targetClient = getClientByNick(target);
			if (targetClient)
			{
				std::string msg = ":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " NOTICE " + target + " :" + text + "\r\n";
				targetClient->AppendOutBuffer(msg);

				std::cout << "Message routé de " << client->GetNickname() << " vers " << target << std::endl;
			}
		}
	}
	else if (command == "PART")
	{
		std::string channelName;
		iss >> channelName;

		if (!channelName.empty())
		{
			if (channelName[0] != '#')
			{
				channelName = "#" + channelName;
			}
			Channel *channel = NULL;
			for (std::vector<Channel>::iterator it = _Channels.begin(); it != _Channels.end(); ++it)
			{
				if (it->GetName() == channelName)
				{
					channel = &(*it);
					break;
				}
			}

			if (channel)
			{
				channel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " PART :" + channelName + "\r\n");
				channel->RemoveMember(client);
			}
			else
			{
				client->AppendOutBuffer(":ircserv 403 " + client->GetNickname() + " " + channelName + " :No such channel\r\n");
			}
		}
	}
	else if (command == "KICK")
	{
		if (!client->GetRegistered())
		{
			client->AppendOutBuffer(":ircserv 451 " + client->GetNickname() + " :You have not registered\r\n");
			return;
		}
		std::string channelName, targetNick, reason;
		iss >> channelName >> targetNick;
		std::getline(iss, reason);
		Channel *channel = NULL;
		for (std::vector<Channel>::iterator it = _Channels.begin(); it != _Channels.end(); ++it)
		{
			if (it->GetName() == channelName)
			{
				channel = &(*it);
				break;
			}
		}
		if (!channel->IsOPbyNick(client->GetNickname()))
		{
			client->AppendOutBuffer(":ircserv 482 " + client->GetNickname() + " :You're not a channel operator\r\n");
			return;
		}
		channel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " KICK " + channelName + " " + targetNick + reason + "\r\n");
		channel->RemoveMember(getClientByNick(targetNick));
	}
	else if (command == "MODE")
	{
		if (!client->GetRegistered())
		{
			client->AppendOutBuffer(":ircserv 451 " + client->GetNickname() + " :You have not registered\r\n");
			return;
		}
		std::string channel;
		iss >> channel;
		Channel *targetChannel = NULL;
		for (std::vector<Channel>::iterator it = _Channels.begin(); it != _Channels.end(); ++it)
		{
			if (it->GetName() == channel)
			{
				targetChannel = &(*it);
				break;
			}
			if (it + 1 == _Channels.end())
			{
				client->AppendOutBuffer(":ircserv 403 " + client->GetNickname() + " " + channel + " :No such channel\r\n");
				return;
			}
		}
		if (targetChannel->IsMember(client) == false)
		{
			client->AppendOutBuffer(":ircserv 482 " + client->GetNickname() + " " + channel + " :You're not a member of that channel\r\n");
			return;
		}
		std::string mode;
		iss >> mode;
		std::vector<std::string> modes;
		bool flag = false;
		if (!mode.empty() && (mode[0] != '+' && mode[0] != '-'))
		{
			client->AppendOutBuffer(":ircserv 501 " + client->GetNickname() + " :Unknown MODE flag\r\n");
			return;
		}
		while (iss && (mode[0] == '+' || mode[0] == '-'))
		{
			if (!mode.empty())
				modes.push_back(mode);
			iss >> mode;
			flag = true;
		}
		if (targetChannel->IsOPbyNick(client->GetNickname()) == true)
		{
			int count = 0;
			std::vector<std::string> args;
			while (iss)
			{
				if (!mode.empty())
					args.push_back(mode);
				count++;
				iss >> mode;
			}
			(void)count;
			char sign = '+';
			for (std::vector<std::string>::iterator it = modes.begin(); it != modes.end(); ++it)
			{
				std::vector<std::string>::iterator argIt = args.begin();
				if ((*it)[0] == '+' || (*it)[0] == '-')
				{
					sign = (*it)[0];
					(*it) = (*it).substr(1);
				}
				for (size_t i = 0; i < (*it).size(); ++i)
				{
					// printf("Mode: %s, Sign: %c\n", (*it).c_str(), sign);
					// printf("Arg: %s\n", argIt->c_str());
					switch ((*it)[i])
					{
					case 'o':
						if (sign == '+')
						{
							if (targetChannel->SetOP(argIt->c_str()))
								targetChannel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " " + "MODE " + channel.c_str() + " " + "+o " + argIt->c_str() + "\r\n");
							argIt++;
						}
						else
						{
							if (targetChannel->DeOP(argIt->c_str()))
								targetChannel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " " + "MODE " + channel.c_str() + " " + "-o " + argIt->c_str() + "\r\n");
							argIt++;
						}
						break;
					case 'k':
						if (sign == '+' && targetChannel->CheckPassword(""))
						{
							targetChannel->SetPassword(argIt->c_str());
							targetChannel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " " + "MODE " + channel.c_str() + " " + "+k " + argIt->c_str() + "\r\n");
							argIt++;
						}
						else if (sign == '-' && !targetChannel->CheckPassword(""))
						{
							if (targetChannel->CheckPassword(argIt->c_str()))
							{
								targetChannel->SetPassword("");
								targetChannel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " " + "MODE " + channel.c_str() + " " + "-k\r\n");
							}
							else
							{
								client->AppendOutBuffer(":ircserv 467 " + client->GetNickname() + " " + channel + " :Incorrect channel key\r\n");
							}
							argIt++;
						}
						else if (sign == '+' && !targetChannel->CheckPassword(""))
						{
							client->AppendOutBuffer(":ircserv 467 " + client->GetNickname() + " " + channel + " :Channel key already set\r\n");
							argIt++;
						}
						else if (sign == '-' && targetChannel->CheckPassword(""))
						{
							client->AppendOutBuffer(":ircserv 467 " + client->GetNickname() + " " + channel + " :No channel key is set\r\n");
							argIt++;
						}
						break;
					case 'l':
						if (sign == '+')
						{
							int limit = atoi(argIt->c_str());
							if (limit > 0 || argIt->c_str() == std::string("0"))
							{
								targetChannel->SetLimit(limit);
								targetChannel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " " + "MODE " + channel.c_str() + " " + "+l " + argIt->c_str() + "\r\n");
							}
							else
							{
								client->AppendOutBuffer(":ircserv 461 " + client->GetNickname() + " MODE :Not enough parameters\r\n");
							}
							argIt++;
						}
						else if (sign == '-' && targetChannel->GetLimit() != -1)
						{
							targetChannel->SetLimit(-1);
							targetChannel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " " + "MODE " + channel.c_str() + " " + "-l\r\n");
						}
						else if (sign == '-' && targetChannel->GetLimit() == -1)
						{
							client->AppendOutBuffer(":ircserv 461 " + client->GetNickname() + " MODE :No limit is set\r\n");
						}
						break;
					case 'i':
						if (sign == '+')
						{
							if (targetChannel->getInv_only())
							{
								client->AppendOutBuffer(":ircserv 461 " + client->GetNickname() + " MODE :Invite only is already set\r\n");
								break;
							}
							targetChannel->SetInvOnly(true);
							targetChannel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " " + "MODE " + channel.c_str() + " " + "+i\r\n");
						}
						else if (sign == '-')
						{
							if (!targetChannel->getInv_only())
							{
								client->AppendOutBuffer(":ircserv 461 " + client->GetNickname() + " MODE :Invite only is not set\r\n");
								break;
							}
							targetChannel->SetInvOnly(false);
							targetChannel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " " + "MODE " + channel.c_str() + " " + "-i\r\n");
						}
						break;
					case 't':
						if (sign == '+')
						{
							if (targetChannel->getTopicOnlyOP())
							{
								client->AppendOutBuffer(":ircserv 461 " + client->GetNickname() + " MODE :Topic only for ops is already set\r\n");
								break;
							}
							targetChannel->SetTopicOnlyOP(true);
							targetChannel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " " + "MODE " + channel.c_str() + " " + "+t\r\n");
						}
						else if (sign == '-')
						{
							if (!targetChannel->getTopicOnlyOP())
							{
								client->AppendOutBuffer(":ircserv 461 " + client->GetNickname() + " MODE :Topic only for ops is not set\r\n");
								break;
							}
							targetChannel->SetTopicOnlyOP(false);
							targetChannel->BroadcastMessage(":" + client->GetNickname() + "!" + client->GetUsername() + "@" + client->GetIPadd() + " " + "MODE " + channel.c_str() + " " + "-t\r\n");
						}
						break;
					default:
						if ((*it)[i] == '+' || (*it)[i] == '-')
							sign = (*it)[i];
						else
							client->AppendOutBuffer(":ircserv 472 " + client->GetNickname() + " " + (*it) + " :is unknown mode char to me\r\n");
						break;
					}
				}
			}
		}
		else if (flag){
			client->AppendOutBuffer(":ircserv 482 " + client->GetNickname() + " " + channel + " :You're not a channel operator\r\n");
			return;
		}
	}
	if (!client->GetNickname().empty() && !client->GetUsername().empty() && client->GetRegistered() && !client->GetLog())
	{
		client->SetLog(true);

		std::string welcomeMsg = ":ircserv 001 " + client->GetNickname() + " :Welcome to the ft_irc network " + client->GetNickname() + "\r\n";
		client->AppendOutBuffer(welcomeMsg);

		std::cout << GRE << "Client <" << client->GetFd() << "> enregistré en tant que " << client->GetNickname() << WHI << std::endl;
	}
	else if (!client->GetNickname().empty() && !client->GetUsername().empty() && !client->GetRegistered())
	{
		client->AppendOutBuffer(":ircserv 451 " + client->GetNickname() + " :You have not registered\r\n");
	}
}

Client *Server::getClientByFd(int fd)
{
	for (std::list<Client>::iterator it = this->_Clients.begin(); it != this->_Clients.end(); ++it)
	{
		if (it->GetFd() == fd)
			return (&(*it));
	}
	return (NULL);
}

void Server::ClearClients(int fd)
{
	for (std::list<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
	{
		if (it->GetFd() == fd)
		{
			std::cout << RED << "Removing client: " << it->GetNickname() << " [" << fd << "]" << WHI << std::endl;
			if (!it->GetNickname().empty())
			{
				_ClientNames.erase(it->GetNickname());
			}
			_Clients.erase(it);
			break;
		}
	}
}

void Server::ReceiveNewData(int fd)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);

	if (bytes <= 0)
	{
		std::cout << RED << "Client " << getClientByFd(fd)->GetNickname() << " [" << fd << "] > Disconnected" << WHI << std::endl;
		ClearClients(fd);
		close(fd);
	}

	else
	{
		Client *client = getClientByFd(fd);
		if (!client)
			return;

		client->AppendBuffer(buff);
		std::string currentBuffer = client->GetBuffer();
		size_t pos;

		while ((pos = currentBuffer.find('\n')) != std::string::npos)
		{
			std::string message = currentBuffer.substr(0, pos);

			if (!message.empty() && message[message.length() - 1] == '\r')
				message.erase(message.length() - 1);

			currentBuffer.erase(0, pos + 1);

			if (message.empty())
				continue;

			std::cout << YEL << "Message extrait de " << client->GetNickname() << " [" << fd << "] "
																								": "
					  << WHI << message << std::endl;

			ParseMessage(message, client);
			if (!getClientByFd(fd))
				return;
		}
		client->SetBuffer(currentBuffer);
	}
}

Client *Server::getClientByNick(std::string nick)
{
	for (std::list<Client>::iterator it = this->_Clients.begin(); it != this->_Clients.end(); ++it)
	{
		if (it->GetNickname() == nick)
			return &(*it);
	}
	return NULL;
}

void Server::run()
{
	if (poll(&this->_fds[0], this->_fds.size(), -1) == -1)
	{
		if (errno == EINTR && this->_sig)
			std::cout << YEL << "Info: Server interrupted by signal." << WHI << std::endl;
		else
			throw std::runtime_error("Error: Polling failed.");
	}
	for (size_t i = 0; i < this->_fds.size(); i++)
	{
		if (this->_fds[i].revents & POLLIN)
		{
			if (this->_fds[i].fd == _SerSocketFd)
				AcceptNewClient();
			else
			{
				Client *client = getClientByFd(this->_fds[i].fd);
				if (!client)
				{
					this->_fds.erase(this->_fds.begin() + i);
					if (i > 0)
						i--;
					continue;
				}
				ReceiveNewData(client->GetFd());
			}
		}
		if (this->_fds[i].revents & POLLOUT)
		{
			Client *client = getClientByFd(this->_fds[i].fd);
			if (client && !client->GetOutBuffer().empty())
			{
				std::string out = client->GetOutBuffer();
				std::cout << ">>> Envoi physique à " << client->GetNickname() << " [" << client->GetFd() << "] : " << out;
				ssize_t bytes = send(client->GetFd(), out.c_str(), out.size(), 0);
				if (bytes > 0)
				{
					client->EraseOutBuffer(bytes);
				}
				else if (bytes < 0)
				{
					std::cerr << "Erreur lors du send() sur le fd " << client->GetFd() << std::endl;
				}
			}
		}
	}
}
