/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chaidel <chaidel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 17:32:55 by aptive            #+#    #+#             */
/*   Updated: 2023/05/31 17:04:53 by chaidel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "ft_irc.hpp"

typedef  std::vector<User*>::iterator 		User_iter;
typedef  std::vector<Channel*>::iterator	Chan_iter;

class Server
{
	public:
			Server(void);
			Server(int port, std::string password);
			Server(Server const & src);
			Server&	operator=(Server const& obj);
			virtual	~Server(void);

		// ** --------------------------------- METHODS ----------------------------------
		void	creation_socket_server( int * server_fd );
		void	configuration_socket_server( int *server_fd );
		void	creation_address_connexion( int port , struct sockaddr_in * addr);
		void	association_socket_to_address (int *server_fd, struct sockaddr_in * addr);
		void	mode_listing_socket(int *server_fd);

		void	boucle_server( void );
		bool	verif_password(User& user, std::string const& pass);
		void	gestion_new_connexion( fd_set * temp, fd_set * _read_sockets, struct sockaddr_in addr);
		void	gestion_activite_client(fd_set * _read_sockets,fd_set * temp);
		void	parsing_cmd( User * user );


		void	send_privmsg(User& user, User& target, std::string const& msg);

		bool	channel_exist(std::string const& cnl_name);


		void	handleCommandServer(std::string const& cmd, std::string const& rest, User& user);

		bool	is_Ope(User& user);
		void	LeaveCnls(User& user);
		// ** --------------------------------- COMMANDE ---------------------------------
		void	cmd_Privmsg(User& user, std::string const& rest);
		void	cmd_Who(User& user, std::string const& rest);
		void	cmd_JoinChannel(std::string const& rest, User& user);
		void	cmd_List(User& user, std::string const& rest);
		void	cmd_Part(User& user, std::string const& rest);
		void	cmd_Nick(User& user, std::string const& nickname);
		bool	is_Used(std::string const& nick);
		bool	is_valid(std::string const& nick);
		void	cmd_Ping(User& user, std::string const& target);
		void	cmd_User(User& user, std::string const& rest);
		void	cmd_Invite(User& user, std::string const& rest);
		bool	is_Present(std::string const& user);
		void	cmd_Mode(User& user, std::string const& rest);
		void	Display_Modes(User& user);
		void	Display_Chan_Modes(User& user, Channel const& channel);
		void	cmd_Kick(User& user, std::string const& rest);
		void	cmd_Topic(User& user, std::string const& rest);
		void	cmd_Notice(User& user, std::string const& rest);
		void	cmd_Quit(User* user);
		
		// ** --------------------------------- ACCESSOR ---------------------------------

		std::string			getHostname() const;
		int					getServer_fd(void) const;
		struct sockaddr_in	getAddr(void) const;
		int					getPort(void) const;
		std::string			getPassword(void) const;
		User_iter			get_User(std::string const& user);
		Chan_iter			get_Channel(std::string	const& channel);
		// ** --------------------------------- SETTER ---------------------------------
		// void				setHostname(std::string const& host);
		void				setServer_fd(const int & server_fd);
		void				setAddr(const struct sockaddr_in & addr);
		void				setPort(const int & port);
		void				setPassword(const std::string & password);
		// void				setClient_socket_v(User user);
		void				setNewChannel(Channel* cnl);
		void				setRmChannel(Channel* cnl);
		void				setRmUser(User* user);

		static	Server* running_serv;
	private:
		std::string				_hostname;
		int						_server_fd;
		struct sockaddr_in		_addr;
		int						_port;
		std::string				_password;
		int						_max_socket_fd;

		std::vector<User*>		_client_socket_v;
		std::vector<Channel*>	_channel;
		fd_set					_read_sockets;

};

std::ostream &			operator<<( std::ostream & o, Server const & i );

std::vector<std::string>	parse_cnl_name(std::string const& line);

#endif
