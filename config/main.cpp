/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinkim <jinkim@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/13 18:58:53 by jinkim            #+#    #+#             */
/*   Updated: 2021/03/31 01:06:00 by jinkim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParse.hpp"
#include <stdio.h>

int main()
{
	try
	{
		ConfigParse conf;
		std::cout << "root : " << conf.server->port << std::endl;
		std::cout << "host : " << conf.server->host << std::endl;
		std::cout << "name : " << conf.server->name << std::endl;
		std::cout << "maxBodySize : " << conf.server->client_max_body_size << std::endl;
		std::cout << "error_root : " << conf.server->error_root << std::endl;
		std::map<int, std::string>::iterator it_ep = conf.server->error_page.begin();
		while (it_ep != conf.server->error_page.end())
		{
			std::cout << "error page(" << it_ep->first << ", " << it_ep->second << ")" << std::endl;
			it_ep++;
		}
		std::cout << "root : " << conf.server->loca.root << std::endl;
		for(int i = 0; i < (int)conf.server->loca.index.size(); i++)
			std::cout << "index : " << conf.server->loca.index[i] << std::endl;
		for(int i = 0; i < (int)conf.server->loca.method.size(); i++)
			std::cout << "method : " << conf.server->loca.method[i] << std::endl;
		std::cout << "autoindex : " << conf.server->loca.autoindex << std::endl;
		std::cout << "cgi : " << conf.server->loca.cgi << std::endl;

		std::cout << "-------------" << std::endl;
		std::map<std::string, ConfigParse::t_location>::iterator it_loca = conf.loca_map.begin();
		while (it_loca != conf.loca_map.end())
		{
			std::cout << it_loca->first << std::endl;
			std::cout << "root : " << it_loca->second.root << std::endl;
			for(int i = 0; i < (int)it_loca->second.index.size(); i++)
				std::cout << "index : " << it_loca->second.index[i] << std::endl;
			for(int i = 0; i < (int)it_loca->second.method.size(); i++)
				std::cout << "method : " << it_loca->second.method[i] << std::endl;
			std::cout << "autoindex : " << it_loca->second.autoindex << std::endl;
			std::cout << "cgi : " << it_loca->second.cgi << std::endl;
			std::cout << std::endl;
			it_loca++;
		}

	}
	catch(std::exception &e){std::cout << e.what() << std::endl;}
}
