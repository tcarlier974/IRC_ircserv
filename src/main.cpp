/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tcarlier <tcarlier@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/10 16:02:10 by tcarlier          #+#    #+#             */
/*   Updated: 2026/07/05 19:06:09 by tcarlier         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << "Usage : ./ircserv [Port] [Password]" << std::endl;
		return(0);
	}
	Server server(av);
	try
	{
		signal(SIGINT, Server::SigHandler);
		signal(SIGQUIT, Server::SigHandler);
		server.ServerInit();
		while(!server.getSig())
		{
			server.run();
		}
	}
	catch (const std::exception &e)
	{
		server.Closefds();
		std::cerr << RED << e.what() << WHI << std::endl;
		return (1);
	}
	return (0);
}