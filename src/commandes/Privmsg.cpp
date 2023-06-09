/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/08 10:51:30 by marvin            #+#    #+#             */
/*   Updated: 2023/05/08 10:51:30 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void RmNewLine(std::string& str, char val)
{
    size_t pos = 0;
    while ((pos = str.find(val, pos)) != std::string::npos)
        str.erase(pos, 1);
}

//   Command: PRIVMSG
// Parameters: <msgtarget> <text to be sent>
// -----
// PRIVMSG is used to send private messages between users, as well as to
// send messages to channels.  <msgtarget> is usually the nickname of
// the recipient of the message, or a channel name.
// -----
//  ERR_NORECIPIENT				ERR_NOTEXTTOSEND	ERR_NOSUCHNICK
//	ERR_CANNOTSENDTOCHAN(+b)	ERR_TOOMANYTARGETS	
void	Server::cmd_Privmsg(User& user, std::string const& rest)
{
	std::string			str;		// Reply
	std::stringstream	ss(rest);
	std::string			msgtarget;	// Channel or User target
	std::string			msg;		// Msg to be sent
	ss >> msgtarget;				// Extrait uniquement le recipient
	std::getline(ss, msg, ':'); 	// Positionne sur le ':'
	std::getline(ss, msg);			// Extrait le msg a partir du ':'

	std::cout << YELLOW << "-PRIVMSG-" << END << std::endl;
	std::cout << RED << "<" << msgtarget << ">\n<" << msg << ">" << END << std::endl;


	if (msgtarget.size() == 0)
	{
		str = ERR_NORECIPIENT(user, "PRIVMSG");
		send(user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);
		return ;
	}

	if (msgtarget[0] == '#')
	{
		std::cout << YELLOW << "-Message To Channel-" << END << std::endl;
		Chan_iter	it = this->get_Channel(msgtarget);
		if (it != this->_channel.end())
		{
			Channel *channel = (*it);
			if (!channel->Is_Ban(user))
			{
				if (channel->Is_Present(user.getNickname()))
				{
					if (msg.size() == 0) // Pas de msg a envoyé
					{
						std::cout << RED << "NO TEXT TO SEND" << END << std::endl;
						str = ERR_NOTEXTTOSEND(user);
						send(user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);
						return ;
					}
					RmNewLine(msg, '\n');
					RmNewLine(msg, '\r');
					channel->Privmsg(user, msg);
				}
				else
				{
					str = ERR_NOTONCHANNEL(user, channel->getName());
					send(user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);
					return ;
				}
			}
			else
			{
				str = ERR_CANNOTSENDTOCHAN(user, channel->getName());
				send(user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);
				return ;
			}
		}
		else
		{
			str = ERR_NOSUCHCHANNEL(user, msgtarget);
			send(user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);
			return ;
		}
	}
	else
	{
		std::cout << YELLOW << "-Message To User-" << END << std::endl;
		User_iter itu = this->get_User(msgtarget);
		if (itu != this->_client_socket_v.end())
		{
			std::cout << GREEN << "USER FOUND" << END << std::endl;
			User *target = (*itu);
			std::cout << GREEN << msg.size() << END << std::endl;
			if (msg.size() == 0)
			{
				std::cout << RED << "NO TEXT TO SEND" << END << std::endl;
				str = ERR_NOTEXTTOSEND(user);
				send(user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);
				return ;
			}
			RmNewLine(msg, '\n');
			RmNewLine(msg, '\r');
			this->send_privmsg(user, *target, msg);
		}
		else
		{
			std::cout << RED << "USER NOT FOUND" << END << std::endl;
			str = ERR_NOSUCHNICK(user, msgtarget);
			send(user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);
			return ;
		}
	}
}