/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinkim <jinkim@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/13 16:15:18 by jinkim            #+#    #+#             */
/*   Updated: 2021/03/13 20:04:21 by jinkim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParse.hpp"

ConfigParse::ConfigParse()
{
    std::ifstream config;
    std::string str;
    std::string type;

    config.open(CONFIG_PATH);
    if (config.is_open())
    {
        do
        {
            config >> str;
            if (str[0] == '[' && str[str.size() - 1] == ']')
            {
                if (str == "[server]")
                    type = "server"; 
                else if (str == "[error_page]")
                    type = "error";
                else
                    type = "location";
                continue ;
            }
            if (type == "server")
                std::cout << str << std::endl;
            else if (type == "error")
                std::cout << str << std::endl;
            else
                std::cout << str << std::endl;
        }while (!config.eof());
        config.close();
    }
}

ConfigParse::~ConfigParse()
{

}

ConfigParse::ConfigParse(ConfigParse const &copy)
{

}

ConfigParse &ConfigParse::operator=(ConfigParse const &ref)
{

}

void ConfigParse::createLocation(std::string type, std::string str)
{
    t_location l;
    
    location.insert(make_pair(type, l));
}