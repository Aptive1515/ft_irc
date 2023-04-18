/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aptive <aptive@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 17:32:55 by aptive            #+#    #+#             */
/*   Updated: 2023/04/18 14:30:08 by aptive           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "web_serv.hpp"

class Server
{
	public:
			Server(void);
			Server(int port, std::string password);
			Server(Server const & src);
			~Server(void);

		// ** --------------------------------- METHODS ----------------------------------
		void	creation_socket_server( int * server_fd );
		void	configuration_socket_server( int *server_fd );
		void	creation_address_connexion( int port , struct sockaddr_in * addr);
		void	association_socket_to_address (int *server_fd, struct sockaddr_in * addr);
		void	mode_listing_socket(int *server_fd);

		void	boucle_server( void );
		void	gestion_new_connexion( fd_set * temp, fd_set * read_sockets, struct sockaddr_in addr);
		void	gestion_activite_client(fd_set * read_sockets,fd_set * temp);
		void	parsing_cmd(std::string buffer, User * user);





		// void	handleCommandServer(const std::string& cmd, const std::string& rest, User user);


		// ** --------------------------------- COMMANDE ---------------------------------

		// void	commandeServer_name( void );

		// ** --------------------------------- ACCESSOR ---------------------------------

		int					getServer_fd(void) const;
		struct sockaddr_in	getAddr(void) const;
		int					getPort(void) const;
		std::string			getPassword(void) const;

		// ** --------------------------------- SETTER ---------------------------------
		void				setServer_fd(const int & server_fd);
		void				setAddr(const struct sockaddr_in & addr);
		void				setPort(const int & port);
		void				setPassword(const std::string & password);
		// void				setClient_socket_v(User user);


	private:
		int					_server_fd;
		struct sockaddr_in	_addr;
		int					_port;
		std::string			_password;
		int					_max_socket_fd;

		std::vector<User>	_client_socket_v;


};

std::ostream &			operator<<( std::ostream & o, Server const & i );


#endif
