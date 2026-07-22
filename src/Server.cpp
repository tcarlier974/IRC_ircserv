/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igilbert <igilbert@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 15:18:38 by tcarlier          #+#    #+#             */
/*   Updated: 2026/07/22 20:24:17 by igilbert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

bool Server::_sig = false;

Server::Server() : 
	_Port(-1), 
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
    {}

Server::Server(char **av) :
	_SerSocketFd(-1),
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
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(this->_Port);

    _SerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_SerSocketFd < 0)
        throw std::runtime_error("Error: Failed to create socket.");
    
    if(setsockopt(_SerSocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) < 0)
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
	std::cout << YEL << "Signal recieved ! Shutting down the server.\n" << WHI << std::endl;
	Server::_sig = true;
}

void Server::Closefds()
{
	for (size_t i = 0; i < _Clients.size(); ++i)
	{
		std::cout << RED << "Closing connection with client: " << _Clients[i].GetNickname() << WHI << std::endl;
		close(_Clients[i].GetFd());
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

    if (command == "PASS")
    {
        std::string password;
        iss >> password;
        if (password == this->_Password)
            client->SetAuth(true);
    }
    else if (command == "NICK")
    {
        std::string nickname;
        iss >> nickname;
        
        if (_ClientNames.find(nickname) != _ClientNames.end())
        {
            client->AppendOutBuffer(":ircserv 433 * " + nickname + " :Nickname is already in use\r\n");
            return;
        }
        if (!client->GetNickname().empty())
            _ClientNames.erase(client->GetNickname());

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
    if (client->GetAuth() && !client->GetNickname().empty() && !client->GetUsername().empty() && !client->GetRegistered())
    {
        client->SetRegistered(true);
        client->SetLog(true);

        std::string welcomeMsg = ":ircserv 001 " + client->GetNickname() + " :Welcome to the ft_irc network " + client->GetNickname() + "\r\n";
        client->AppendOutBuffer(welcomeMsg);
        
        std::cout << GRE << "Client <" << client->GetFd() << "> enregistré en tant que " << client->GetNickname() << WHI << std::endl;
    }
}

Client	*Server::getClientByFd(int fd)
{
	for ( std::vector<Client>::iterator it = this->_Clients.begin(); it != this->_Clients.end(); ++it )
	{
		if ( it->GetFd() == fd )
			return ( &(*it) );
	}
	return ( NULL );
}

void Server::ClearClients(int fd)
{
    for (std::vector<Client>::iterator it = _Clients.begin(); it != _Clients.end(); ++it)
    {
        if (it->GetFd() == fd)
        {
            std::cout << RED << "Removing client: " << it->GetNickname() << WHI << std::endl;
            _Clients.erase(it);
            break;
        }
    }
}

void Server::ReceiveNewData(int fd)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1 , 0);

	if(bytes <= 0){
		std::cout << RED << "Client <" << fd << "> Disconnected" << WHI << std::endl;
		ClearClients(fd);
		close(fd);
	}

	else{
		buff[bytes] = '\0';
		Client *client = getClientByFd(fd);
        std::cout << YEL << "Client <" << fd << "> Data: " << WHI << buff;
		if (!client)
            return;
		client->AppendBuffer(buff);\
		std::string currentBuffer = client->GetBuffer();
        size_t pos;
		while ((pos = currentBuffer.find("\r\n")) != std::string::npos)
        {
            // extraire la commande (sans le \r\n parce que nique le RN)
            std::string message = currentBuffer.substr(0, pos);
            
            std::cout << YEL << "Message complet reçu de <" << fd << "> : " << WHI << message << std::endl;
            
            // TODO : Envoyer la string "message" vers la fonction de parsing ici
            
            currentBuffer.erase(0, pos + 2);

			message = currentBuffer.substr(0, pos);
			std::cout << YEL << "Message complet reçu de <" << fd << "> : " << WHI << message << std::endl;

			ParseMessage(message, client);

			currentBuffer.erase(0, pos + 2);
			//here you can add your code to process the received data: parse, check, authenticate, handle the command, etc...
        }
        client->SetBuffer(currentBuffer);
	}
}


void Server::run()
{
    if ( poll( &this->_fds[0], this->_fds.size(), -1 ) == -1 )
    {
        if ( errno == EINTR && this->_sig )
            std::cout << YEL << "Info: Server interrupted by signal." << WHI << std::endl;
		else
        	throw std::runtime_error("Error: Polling failed.");
    }
    for ( size_t i = 0; i < this->_fds.size(); i++ )
	{
		if ( this->_fds[i].revents & POLLIN )
		{
			if ( this->_fds[i].fd == _SerSocketFd )
				AcceptNewClient();
			else
			{
				Client	*client = getClientByFd( this->_fds[i].fd );
				if ( !client )
				{
					this->_fds.erase( this->_fds.begin() + i );
					if ( i > 0 )
						i--;
					continue ;
				}
				ReceiveNewData( client->GetFd() );
			}
		}
		// GESTION DU POLLOUT ICI
		if ( this->_fds[i].revents & POLLOUT )
		{
			Client *client = getClientByFd(this->_fds[i].fd);
			if (client && !client->GetOutBuffer().empty())
			{
				std::string out = client->GetOutBuffer();
				ssize_t bytes = send(client->GetFd(), out.c_str(), out.size(), 0);
				if (bytes > 0)
				{
					// On efface seulement ce qui a été envoyé
					client->EraseOutBuffer(bytes); 
				}
			}
		}
	}
}

