/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcarlier <tcarlier@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 15:49:53 by tcarlier          #+#    #+#             */
/*   Updated: 2026/05/10 16:00:45 by tcarlier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"

class Server
{
	private :
		int Port;
		int SerSocketFd;
		static bool sig;
		std::vector< Client > Clients;
		std::vector< struct pollfd > fds;
	public :
		Server();
		Server(char **av);
		Server( const Server &other);
		~Server();
		Server &operator=(const Server &other);

		void ServerInit();
		void SerSocket();
		void AcceptNewClient();
		void ReceiveNewData(int fd);

		static void SigHandler(int signum);

		void Closefds();
		void ClearClients(int fd);
};

#endif