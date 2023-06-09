/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/26 16:19:37 by root              #+#    #+#             */
/*   Updated: 2023/05/02 15:17:54 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// Check le server
// PING <username>
void	Server::cmd_Ping(User& user, std::string const& target)
{
	std::string str;
	if (target.empty())
	{
		str = ERR_NOORIGIN();
		send(user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);
		return ;
	}
	else
	{
		str = PONG(user);
		std::cout << GREEN << "-- Ping response --\n" << str << END << std::endl;
		send(user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);
	}
}