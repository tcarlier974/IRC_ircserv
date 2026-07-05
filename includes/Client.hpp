/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcarlier <tcarlier@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 14:51:49 by tcarlier          #+#    #+#             */
/*   Updated: 2026/07/05 19:06:09 by tcarlier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include	<algorithm>
# include	<arpa/inet.h>
# include	<cctype>
# include	<csignal>
# include	<cstring>
# include	<errno.h>
# include	<exception>
# include	<fcntl.h>
# include	<iomanip>
# include	<iostream>
# include	<netdb.h>
# include	<netinet/in.h>
# include	<poll.h>
# include	<set>
# include	<sstream>
# include	<stdexcept>
# include	<string>
# include	<sys/socket.h>
# include	<sys/types.h>
# include	<unistd.h>
# include	<utility>
# include	<vector>

#define RED "\e[1;31m" //-> for red color
#define WHI "\e[0;37m" //-> for white color
#define GRE "\e[1;32m" //-> for green color
#define YEL "\e[1;33m" //-> for yellow color

class Client
{
	private :
		int _Fd;
		std::string		_ipAddress;
		std::string		_username;
		std::string		_nickname;
		std::string		_hostname;
		std::string		_password;
		bool			_registered;
		bool			_isAuth;
		bool			_isLog;
		std::string		_buffer;

	public :
		Client(void);
		Client(std::string IPadd);
		Client(const Client &other);
		Client &operator=(const Client &other);
		~Client(void);

		void SetFd(int Fd);
		void SetIPadd(std::string IPadd);
		void SetUsername(std::string username);																			
		void SetNickname(std::string nickname);
		void SetHostname(std::string hostname);
		void SetPassword(std::string password);
		void SetRegistered(bool registered);
		void SetAuth(bool auth);
		void SetLog(bool log);
		int GetFd(void);
		std::string GetIPadd(void);
		std::string GetUsername(void);
		std::string GetNickname(void);
		std::string GetHostname(void);
		std::string GetPassword(void);
		bool GetRegistered(void);
		bool GetAuth(void);
		bool GetLog(void);
};

#endif