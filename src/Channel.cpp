/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chaidel <chaidel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/19 08:47:29 by root              #+#    #+#             */
/*   Updated: 2023/05/31 18:01:16 by chaidel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel(User& user, std::string const& name, std::string const& pass)
{
	std::cout << "---------------------------------------" << std::endl;
	std::cout << "Creation d'un nouveau Canal: "<< GREEN << "<" << name << ">" << END << std::endl;
	std::cout << "---------------------------------------" << std::endl;

	std::cout << pass.size() << " | " << pass << std::endl;

	if (pass.size() == 0 || pass == "x")
	{
		std::cout << "key false\n";
		this->_key = false;
		this->_pass = "";
	}
	else
	{
		std::cout << "key true\n";
		this->_key = true;
		this->_pass = pass;
	}
	this->setName(name);
	this->AddUser(user, true);
	this->_InvOnly = false;
	this->_bans = true;
	this->_limit = false;
	this->_capacity = 50;
	this->_top = false;
}

Channel::Channel(Channel const& cpy)
{
	*this = cpy;
}

Channel&	Channel::operator=(Channel const& obj)
{
	if (this != &obj)
	{
		_name			= obj._name;
		_topic			= obj._topic;
		_pass			= obj._pass;
		_users			= obj._users;
		_privilege		= obj._privilege;
		_ban			= obj._ban;
		_invited		= obj._invited;
		_capacity		= obj._capacity;
		_InvOnly		= obj._InvOnly;
		_bans			= obj._bans;
		_limit			= obj._limit;
		_key			= obj._key;
		_top			= obj._top;
	}
	return (*this);
}

Channel::~Channel() // Kick les users toujours present avant de fermer le serveur
{

	std::map<int, User*>::iterator	it = this->_users.begin();
	std::map<int, User*>::iterator	ite = this->_users.end();

	std::string	reason = "Channel closing !";

	if (!this->Is_OpePresent())
	{
		while (it != ite)
		{
			User*	user = it->second;
			this->PartUser(*user, reason);
			user->setRmCnlMembership(this);
			it = this->_users.begin();
		}
	}
	this->_users.clear();
	this->_privilege.clear();
	this->_ban.clear();
	this->_invited.clear();
}

void	Channel::Privmsg(User& user, std::string const& msg)
{
	std::cout << YELLOW << "-Sending Message To All Users On Channel-" << END << std::endl;
	std::map<int, User*>::iterator	it = this->_users.begin();
	std::map<int, User*>::iterator	ite = this->_users.end();

	std::cout << GREEN << "|" << msg << "|" << END << std::endl;
	std::string	privmsg = ":" + user.getNames() + " PRIVMSG " + this->_name + " " + msg + "\r\n";

	// std::cout << "|" << privmsg << "|" << std::endl;
	while (it != ite)
	{
		if (it->first != user.getFd())
			send(it->first, privmsg.c_str(), privmsg.size(), MSG_NOSIGNAL);
		it++;
	}
}

// Send le msg a tout les users du Canal
void	Channel::Broadcast(std::string const& msg)
{
	std::map<int, User*>::iterator	it = this->_users.begin();
	std::map<int, User*>::iterator	ite = this->_users.end();

	while (it != ite)
	{
		send(it->first, msg.c_str(), msg.size(), MSG_NOSIGNAL);
		std::cout << RED<< "<" << this->_name << ">: BROADCAST TO " << it->first << ", " << it->second->getNickname()  << END << std::endl;
		it++;
	}
}

// Send le topic a tout les users du Canal, Command /topic
void	Channel::Broadcast_topic()
{
	std::map<int, User*>::iterator	it = this->_users.begin();
	std::map<int, User*>::iterator	ite = this->_users.end();

	std::string	topic = this->getTopic();
	std::string	str;

	while (it != ite)
	{
		User *user = it->second;
		std::cout << RED<< "<" << this->_name << ">: BROADCAST TO " << it->first << ", " << it->second->getNickname() << " TOPIC" << END << std::endl;
		if (topic.empty())
			str = RPL_NOTOPIC((*user), this->_name);
		else
			str = RPL_TOPIC((*user), this->_name, topic);
		std::cout << GREEN << topic << END << std::endl;
		send(it->first, str.c_str(), str.size(), MSG_NOSIGNAL);
		it++;
	}
}

// Add un user au channel avec ses privileges (ope ou standard)
void	Channel::AddUser(User& new_user, bool priv) // check user existant
{
	this->_users.insert(std::pair<int, User *>(new_user.getFd(), &new_user));
	this->_privilege.insert(std::pair<int, bool>(new_user.getFd(), priv));

	if (this->Is_Inv(new_user)) // New_user a accepté l'invitation, il est supp de la liste des invités
		this->UnInvUser(new_user);
	std::string msg = ":" + new_user.getNames() + " JOIN " + this->_name + "\r\n";

	std::cout << GREEN << "< " << this->_name << " >: " << msg << END << std::endl;
	this->Broadcast(msg);

	if (!this->_topic.empty())
	{
		std::string	str = RPL_TOPIC(new_user, this->getName(), this->_topic);
		send(new_user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);
	}
}

// Ajoute un nouvel User en tant qu'inviter dans le channel
// 341    RPL_INVITING "<channel> <nick>"
// Returned by the server to indicate that the
// attempted INVITE message was successful and is
// being passed onto the end client.
// -------
// Only the user inviting and the user being invited will receive
//	notification of the invitation.  Other channel members are not notified.
void	Channel::InvUser(User& user, User& new_user)
{
	// this->_invited.insert(std::pair<int, bool>(new_user.getFd(), true));
	this->_invited.push_back(new_user.getFd());

	std::string str = RPL_INVITE(user, new_user, this->_name); // Invitation au new_user
	send(new_user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);

	str = RPL_INVITING(user, new_user, this->_name); // Message de confirmation a l'inviteur
	send(user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL);
}

// Retire l'user de la liste des invites
void	Channel::UnInvUser(User& target)
{
	this->_invited.erase(std::find(this->_invited.begin(), this->_invited.end(), target.getFd()));
}

// Ajoute un nouvel operateur de Canal
void	Channel::AddOpe(User& new_oper)
{
	this->setUserPrivilege(new_oper.getFd(), true);
	std::cout << RED << "<" << this->_name << ">: " << new_oper.getNickname() << " est devenu opetareur !" << END << std::endl;
}

// Retire les droits operateurs du Canal a l'user
void	Channel::RmOpe(User& user)
{
	this->setUserPrivilege(user.getFd(), false);
	std::cout << RED << "<" << this->_name << ">: " << user.getNickname() << " n'est plus opetareur !" << END << std::endl;
}

//Commande part
// :<user> PART <channel> :<reason>
// :John!~user@host PART #channel :Je reviendrai plus tard
void	Channel::PartUser(User& user, std::string const& reason)
{
	this->_users.erase(user.getFd()); // Supp de la list des users

	this->_privilege.erase(user.getFd()); // Supp de la list des priv

	std::cout << YELLOW << "PART " << user.getUsername() << " has been deleted from " << this->_name << END << std::endl;
	
	std::string	cast = ":" + user.getNames() + " PART " + this->_name + " :" + reason + "\r\n";
	this->Broadcast(cast); // Display a tout les users que l'user est parti

	std::string	str = ":" + user.getNames() + " PART " + this->_name + " :" + reason + "\r\n";
	send(user.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL); // Confirmation a l'user que la commande est reussi
}

void	Channel::BanUser(User& user, User& target)
{
	std::cout << YELLOW << "-Ban User-" << END << std::endl;
	this->_ban.push_back(target.getFd());

	std::cout << GREEN << "<" << this->_name << ">: " << target.getNickname() << " has been banned by " << user.getNickname() << END << std::endl;

	std::string	reason = ":You have been Banned !";

	this->KickUser(user, target, reason);
	this->setChanModes(user, std::string("+b"), std::string(""));
}

void	Channel::UnBanUser(User& user, User& target)
{
	std::cout << YELLOW << "-Unban User-" << END << std::endl;
	if (this->Is_Ban(target))
	{
		this->_ban.erase(std::find(this->_ban.begin(), this->_ban.end(), target.getFd()));
		std::cout << GREEN << "<" << this->_name << ">: " << target.getNickname() << " has been unbanned by " << user.getNickname() << END << std::endl;
		std::string	str = RPL_UNBANUSER(target, this, user);
		send(target.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL); //	La target est prevenue qu'elle est unban
	}
}

void	Channel::KickUser(User& user, User& target, std::string const& reason)
{
	// La target est retiré des maps
	this->_users.erase(target.getFd());
	this->_privilege.erase(target.getFd());

	std::string	str = RPL_KICK(user, this->_name, target, reason);

	send(target.getFd(), str.c_str(), str.size(), MSG_NOSIGNAL); //	La target est prevenue qu'elle est kick

	Broadcast(str); // Les users du channel sont prevenue que la target a ete kick
}

bool	Channel::Is_PassValid(std::string const& key)
{
	std::string	pass(key);

	RmNewLine(pass, '\n');
	RmNewLine(pass, '\r');

	if (pass.size() == 1 || !str_is_alpha(pass))
		return (false);
	return (true);
}

bool	Channel::Is_Ban(User& user)
{
	std::list<int>::iterator it = std::find(this->_ban.begin(), this->_ban.end(), user.getFd());
	// std::list<int>::iterator it = this->_ban.find(user.getFd());
	if (it == this->_ban.end())
	{
		std::cout << GREEN << "-" << user.getNickname() << " is not banned-" << END << std::endl;
		return (false);
	}
	else
	{
		std::cout << RED << "-" << user.getNickname() << " is banned-" << END << std::endl;
		return (true);
	}
}

bool	Channel::Is_Ope(User& user)
{
	std::map<int, bool>::iterator it = this->_privilege.find(user.getFd());
	if (it == this->_privilege.end())
		std::cout << RED << "-" << user.getNickname() << " is not operator-" << END << std::endl;
	else
		std::cout << GREEN << "-" << user.getNickname() << " is operator-" << END << std::endl;
	return (it->second);
}

bool	Channel::Is_Inv(User& user)
{
	std::list<int>::iterator it = std::find(this->_invited.begin(), this->_invited.end(), user.getFd());
	// std::map<int, bool>::iterator it = this->_invited.find(user.getFd());
	if (it == this->_invited.end())
	{
		std::cout << RED << "-" << user.getNickname() << " is not invited-" << END << std::endl;
		return (false);
	}
	else
	{
		std::cout << GREEN << "-" << user.getNickname() << " is invited-" << END << std::endl;
		return (true);
	}
}

bool	Channel::Is_Present(std::string const& user)
{
	if (this->getUser(user) == NULL)
		std::cout << RED << "-" << user << " is not  present-" << END << std::endl;
	else
		std::cout << GREEN << "-" << user << " is present-" << END << std::endl;
	return (this->getUser(user) ? true : false);
}

bool	Channel::Is_OpePresent()
{
	std::map<int, bool>::iterator	it = this->_privilege.begin();
	std::map<int, bool>::iterator	ite = this->_privilege.end();

	while (it != ite)
	{
		if (it->second)
			return (true);
		it++;
	}
	std::cout << RED << "<" << this->_name << ">: No Operator Present !" << END << std::endl;
	return (false);
}

bool	Channel::Is_InvOnly() const
{
	return (this->_InvOnly);
}

bool	Channel::Is_BanOnly() const
{
	return (this->_bans);
}

bool	Channel::Is_Private() const
{	return (this->_key); }

bool	Channel::Is_PassOnly() const
{	return (this->_key); }

bool	Channel::Is_limitSet() const
{	return (this->_limit); }

bool	Channel::Is_TopicLock() const
{	return (this->_top); }

		/*	Getters */

std::string	Channel::getName() const
{	return (this->_name); }

std::string	Channel::getTopic() const
{	return (this->_topic); }

std::string	Channel::getModes() const
{
	std::string	modes;

	if (this->Is_InvOnly())
		modes.push_back('i');
	if (this->Is_BanOnly())
		modes.push_back('b');
	if (this->Is_limitSet())
		modes.push_back('l');
	if (this->Is_PassOnly())
		modes.push_back('k');
	if (this->Is_TopicLock())
		modes.push_back('t');
	std::cout << GREEN << "<" << this->_name << "> modes: [" << modes << "]" << END << std::endl;
	return (modes);
}

void	Channel::getWho(User& user)
{
	(void)user;
}

std::string	Channel::getPass() const
{	return (this->_pass); }

std::vector<User *>	Channel::getUsers()
{
	std::vector<User *>	list_user;

	for (std::map<int, User *>::iterator it = this->_users.begin(); it != this->_users.end(); it++)
	{
		list_user.push_back(it->second);
		std::cout << it->second->getUsername() << std::endl;
	}
	return (list_user);
}

int	Channel::getCapacity() const
{	return (this->_capacity); }

int	Channel::getNumUsers() const
{	return (this->_users.size()); }

std::string	Channel::getSNumUsers() const
{
	std::stringstream	ss;
	ss << this->_users.size();
	std::string	num = ss.str();
	return (num);
}

User*	Channel::getUser(std::string const& user)
{
	for (std::map<int, User*>::iterator	it = this->_users.begin(); it != this->_users.end(); it++)
	{
		User *tmp = it->second;
		if (!user.compare(tmp->getNickname()))
			return(tmp);
	}
	return (NULL);
}

bool	Channel::getUserPrivilege(int user_fd) const
{	return ((this->_privilege.find(user_fd)->second)); }

		/*	Setters */

void	Channel::setName(std::string name)
{	this->_name = name; }

void	Channel::setChanModes(User& user, std::string const& mode, std::string const& arg)
{
	bool		sign;
	std::string	cast;

	size_t		i(0);
	while (i < mode.size())
	{
		if (mode[i] == '+')
			sign = true;
		else if (mode[i] == '-')
			sign = false;
		else if (mode[i] == 'i' && sign)
		{
			this->_InvOnly = true;
			this->getModes();
			cast = CHANMODE(this->_name, "+i");
			std::cout << RED << cast << END << std::endl;
			this->Broadcast(cast);
		}
		else if (mode[i] == 'i' && !sign)
		{
			this->_InvOnly = false;
			this->getModes();
			cast = CHANMODE(this->_name, "-i");
			this->Broadcast(cast);
		}
		else if (mode[i] == 'b' && sign)
		{
			this->_bans = true;
			this->getModes();
			cast = CHANMODE(this->_name, "+b");
			this->Broadcast(cast);
		}
		else if (mode[i] == 'b' && !sign)
		{
			this->_bans = false;
			this->_ban.clear();
			this->getModes();
			cast = CHANMODE(this->_name, "-b");
			this->Broadcast(cast);
		}
		else if (mode[i] == 'l' && sign)
		{
			
			int					size;
			std::stringstream	ss(arg);
			ss >> size;
			std::cout << "arg: " << arg.size() << "|" << arg.empty() << std::endl;
			if (arg.empty())
				size = 50;
			std::cout << "size: " << size << " | numUsers: " << this->getNumUsers() << std::endl;
			if (size > 0 && this->getNumUsers() < size && size <= 50)
			{
				this->_limit = true;
				this->setChanLimit(size);
				this->getModes();
				cast = CHANMODE(this->_name, std::string("+l ") + arg);
				this->Broadcast(cast);
			}
		}
		else if (mode[i] == 'l' && !sign)
		{
			this->_limit = false;
			this->setChanLimit(50);
			this->getModes();
			cast = CHANMODE(this->_name, "-l");
			this->Broadcast(cast);

		}
		else if (mode[i] == 't' && sign)
		{
			this->_top = true;
			this->getModes();
			cast = CHANMODE(this->_name, "+t");
			this->Broadcast(cast);
		}
		else if (mode[i] == 't' && !sign)
		{
			this->_top = false;
			this->getModes();
			cast = CHANMODE(this->_name, "-t");
			this->Broadcast(cast);
		}
		else if (mode[i] == 'k' && sign)
		{
			std::stringstream	ss(arg);
			std::string			key;
			ss >> key;
			std::cout << "mdp: " << key << std::endl;
			if (!arg.empty())
			{
				if (this->Is_PassOnly())
				{
					cast = ERR_KEYSET(user, this->_name);
					send(user.getFd(), cast.c_str(), cast.size(), MSG_NOSIGNAL);
				}
				else
				{
					if (this->Is_PassValid(key))
					{
						this->_key = true;
						this->setChanPass(key);
						this->getModes();
						cast = CHANMODE(this->_name, std::string("+k ") + arg);
						this->Broadcast(cast);
					}
				}
			}
		}
		else if (mode[i] == 'k' && !sign)
		{
			this->_key = false;
			this->_pass.clear();
			this->getModes();
			cast = CHANMODE(this->_name, "-k");
			this->Broadcast(cast);
		}
		else
		{

		}
		i++;
	}
}

// /mode <channel> <mode> <nickname>(target)
void	Channel::setUserModes(User& user, User& target, std::string const& mode)
{
	bool	sign;

	size_t	i(0);
	while (i < mode.size())
	{
		if (mode[i] == '+')
			sign = true;
		else if (mode[i] == '-')
			sign = false;
		else if (mode[i] == 'o' && sign)
		{
			if (!this->Is_Ope(target))
			{
				this->AddOpe(target);
				this->Broadcast(RPL_CHANNELMODEIS(target, this->_name, "+o"));
			}
		}
		else if (mode[i] == 'o' && !sign)
		{
			if (this->Is_Ope(target))
			{
				this->RmOpe(target);
				this->Broadcast(RPL_CHANNELMODEIS(target, this->_name, "-o"));
			}
		}
		else if (mode[i] == 'b' && sign)
		{
			if (!this->Is_Ban(target))
			{
				this->BanUser(user, target);
				this->Broadcast(RPL_CHANNELMODEIS(target, this->_name, "+b"));
			}
		}
		else if (mode[i] == 'b' && !sign)
		{
			if (this->Is_Ban(target))
			{
				this->UnBanUser(user, target);
				this->Broadcast(RPL_CHANNELMODEIS(target, this->_name, "-b"));
			}
		}
		i++;
	}
}

void	Channel::setTopic(std::string const& topic)
{
	std::cout << YELLOW << "-Set New Topic-" << END << std::endl;
	this->_topic = topic;
}

void	Channel::setTopicClear()
{
	std::cout << YELLOW << "-Clear Topic-" << END << std::endl;
	this->_topic.clear();
}

void	Channel::setUserPrivilege(int user_fd, bool priv)
{	(this->_privilege.find(user_fd))->second = priv; }

void	Channel::setChanLimit(int capacity)
{
	this->_capacity = capacity;
	std::cout << GREEN << "-Chan Capacity set at ["<< capacity << "]-" << std::endl;
}

void	Channel::setChanPass(std::string const& pass)
{	this->_pass = pass; }

void	Channel::setTopLock(bool lock)
{	this->_top = lock; }

		/*	Displa Operator Overload */

/*	Affiche toutes les informations du Canal */
std::ostream&	operator<<(std::ostream& flux, Channel& cnl)
{
	std::vector<User *>	list_user = cnl.getUsers();
	flux << "---------------------------------------" << std::endl;
	flux << "Canal " << cnl.getName() << std::endl;
	flux << "Topic: " << cnl.getTopic() << std::endl;
	flux << "list_user:" << std::endl;
	for (std::vector<User *>::iterator it = list_user.begin(); it != list_user.end(); it++)
		flux << (*it)->getFd() << "\t" << (*it)->getNickname() << cnl.getUserPrivilege((*it)->getFd()) << std::endl;
	flux << "---------------------------------------" << std::endl;
	return (flux);
}


		/*	Comparison Operator Overload */

bool	operator==(Channel const& cnl1, Channel const& cnl2)
{
	if (!cnl1.getName().compare(cnl2.getName()))
		return (true);
	return (false);
}
