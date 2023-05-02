/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/12 18:50:55 by aptive            #+#    #+#             */
/*   Updated: 2023/05/02 20:01:28 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/User.hpp"



/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/
User::User()
{

}

User::User(User const & src) : _set(src.getSet()), _fd(src.getFd()), _id(src.getId()), _username(src.getUsername()), _nickname(src.getNickname()), _hostname(src.getHostname()), _auth_password(src.getAuth_password())
{
}

User::User(int fd) : _set(false), _fd(fd), _username("X"), _nickname("X"), _hostname("X")
{
	// init _id
	static int id = 0;
	id++;
	this->_id = id;

	// init _nickname
	// std::stringstream ss;
	// ss << "Default_Nickname_" << id;
	// this->_nickname = ss.str();

	// init _admin

	this->_admin = false;
	this->_auth_password = false;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

User::~User()
{

}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

User & User::operator=(User const & rhs)
{
	if (this != &rhs && rhs.getFd())
		*this = rhs;
	return *this;
}

std::ostream &			operator<<( std::ostream & o, User const & i )
{
	o << "User		:	" << i.getId() << std::endl;
	o << "fd		:	" << i.getFd() << std::endl;
	o << "Username	:	" << i.getUsername() << std::endl;
	o << "Nickname	:	" << i.getNickname() << std::endl;
	if (i.getAdmin() == true)
		o << "Operator	:	" << "admin" << std::endl;
	else
		o << "Operator	:	" << "user" << std::endl;
	return o;
}



/*
** --------------------------------- METHODS ----------------------------------
*/



// void	User::handleCommand(const std::string& cmd, const std::string& rest)
// {
// 	// std::string levels[] = {
// 	// 							"/NICK",
// 	// 							"/JOIN",
// 	// 							"/NAMES" };

// 	void (User::*f[])(const std::string&) = {
// 		&User::setNickname,
// 		&User::cmd_JoinChannel
// 	};

// 	// for (int i = 0; i < 2; i++) {
// 	// 	if (levels[i] == cmd) {
// 	// 		(this->*f[i])(rest);
// 	// 	}
// 	// }

// 	switch (option(cmd))
// 	{
// 		case Unknown:
// 			throw std::string("Unknown Command !");
// 			break;
// 		case Nick:
// 			(this->*f[Nick - 1])(rest);
// 			break;
// 		case Join:
// 			(this->*f[Join - 1])(rest);
// 			break;
// 	}
// }

void	User::sendMessage(const std::string& message) const
{
	std::cout << "[SEND] : to [" << _nickname << "] : " << message;
	send(this->getFd(), message.c_str(), message.length(), MSG_NOSIGNAL);
}

void	User::setBuf(const std::string& buf)
{
	this->_buf += buf;

	if (this->_buf[this->_buf.size() - 1] == '\n')
		this->_buf[this->_buf.size() - 1] = '\0';

}

void	User::clearBuf( void )
{
	this->_buf = "";
}

/*
 *	Quitte tout les Canals dont l'user fait parti
 *	Parcours la liste des Canals, pour chaque Canal -> RmCnlMembership
*/
// void	User::LeaveCnls()
// {
// 	std::cout << "list part of cnl " << this->getListCnl() << std::endl;
// 	for (std::vector<Channel*>::iterator it = this->_list_cnl.begin(); it != this->_list_cnl.end(); it++)
// 	{
		
// 		(*it)->RmUser(this->_nickname);
// 		this->_list_cnl.erase(it);
// 	}
// }

bool	User::is_set()
{
	if (!this->_username.compare("X") || !this->_nickname.compare("X") || !this->_hostname.compare("X"))
		return (false);
	return (true);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

bool	User::getSet() const
{
	return (this->_set ? true : false);
}

int User::getFd( void ) const
{
	return this->_fd;
}

int User::getId( void ) const
{
	return this->_id;
}

std::string		User::getUsername() const
{
	return (this->_username);
}

std::string		User::getNickname(void) const
{
	return this->_nickname;
}

std::string	User::getHostname() const
{
	return (this->_hostname);
}

std::string		User::getBuf(void) const
{
	return this->_buf;
}

bool		User::getAdmin(void) const
{
	return this->_admin;
}

bool		User::getAuth_password(void) const
{
	// std::cout << "here : " << this->_auth_password << std::endl;
	return this->_auth_password;
}

// Renvoie tout les channels dont est present l'user
std::vector<Channel *>	User::getListCnl()
{
	return (this->_list_cnl);
}

/*
** --------------------------------- SETTER ---------------------------------
*/

void	User::setSetUser()
{
	this->_set = true;
}

void	User::setUsername(std::string const& name)
{
	this->_username = name;
}

void User::setNickname(const std::string& nick)
{
	// std::cout << "setNickname : " << std::endl;
	// std::cout << "setNickname : " << nickname << std::endl;


	// std::string message = "[Server] : " + this->_nickname + " nickname has been changed to " + nick + "\n";
	this->_nickname = nick;
	// this->sendMessage(message);
}

void	User::setHostname(std::string const& host)
{
	this->_hostname = host;
}

void	User::setAdmin(const bool & admin)
{
	this->_admin = admin;
}


void	User::setAuth_passwordOK( void )
{
	this->_auth_password = true;
}

// Ajoute le nouveau channel a la liste des channels dont l'user fait partie
void	User::setAddListCnlMember(Channel* cnl)
{
	this->_list_cnl.push_back(cnl);
}

// Retire le channel de la liste des channels dont l'user fait partie
void	User::setRmCnlMembership(Channel* cnl)
{
	for (std::vector<Channel*>::iterator it = this->_list_cnl.begin(); it != this->_list_cnl.end(); it++)
	{
		if ((*it) == cnl)
		{
			this->_list_cnl.erase(it);
			return ;
		}
	}
	// throw std::string("Channel not found");
}

// // Retire le channel de la liste des channels dont l'user fait partie
// // L'user est par la meme occassion retirer au sein du channel
// void	User::setRmCnlMembership(Channel& cnl)
// {
// 	cnl.RmUser(this->getFd());
// }

/* ************************************************************************** */
