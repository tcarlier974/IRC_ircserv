/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: igilbert <igilbert@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 15:49:53 by tcarlier          #+#    #+#             */
/*   Updated: 2026/07/22 20:08:58 by igilbert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"

class Server
{
	private :
		int _Port;
		int _SerSocketFd;
		std::string _Password;
		static bool _sig;
		std::vector< Client > _Clients;
		std::vector< struct pollfd > _fds;
		int	_NextChannelId;
		std::string _hostName;
		// std::vector< Channel > _Channels;
		std::set< std::string > _ClientNames;
		std::set< std::string > _Topics;
		// std::vector< Parser > _parsedMessages;
	public :
		Server();
		Server(char **av);
		Server( const Server &other);
		~Server();
		Server &operator=(const Server &other);

		void ParseMessage(std::string message, Client *client);
		void SendMessage(int fd, std::string msg);

		void ServerInit();
		void SerSocket();
		void AcceptNewClient();
		void ReceiveNewData(int fd);
		void run();

		static void SigHandler(int signum);

		void Closefds();
		void ClearClients(int fd);

		bool getSig() const;
		Client *getClientByFd(int fd);

		int setPort(char *port);
		std::string setPassword(char *password);
};

#endif