/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcarlier <tcarlier@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 14:51:49 by tcarlier          #+#    #+#             */
/*   Updated: 2026/05/10 17:42:21 by tcarlier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <vector>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>
# include <arpa/inet.h> 
# include <poll.h>
# include <csignal>

class Client
{
	private :
		int Fd;
		std::string IPadd;

	public :
		Client(void);
		Client(std::string IPadd);
		Client(const Client &other);
		Client &operator=(const Client &other);
		~Client(void);

		void SetFd(int Fd);
		void SetIPadd(std::string IPadd);
		int GetFd(void);
		std::string GetIPadd(void);
};

#endif