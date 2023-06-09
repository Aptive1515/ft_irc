/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tdelauna <tdelauna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 19:19:35 by aptive            #+#    #+#             */
/*   Updated: 2023/05/23 17:59:41 by tdelauna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ft_irc.hpp"

void parsing(int argc, char ** argv, int * port, std::string * password)
{
	check_arg(argc, argv);
	*port = atoi(argv[1]);
	*password = argv[2];
	// std::cout << "Port / Password : " << port << " / " << password << std::endl;

}

void check_arg(int argc, char ** argv)
{
	if (argc != 3)
		throw std::string("Error : Wrong number of arguments !\n");

	for (int i = 0; argv[1][i]; i++)
	{
		if (!isdigit(argv[1][i]))
			throw std::string("Error : Wrong format port !\n");
	}
	if (atoi(argv[1]) < 1 && atoi(argv[1]) > 65635)
		throw std::string("Error : Wrong port !\n");
}
