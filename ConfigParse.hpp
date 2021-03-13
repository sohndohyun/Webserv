/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParse.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinkim <jinkim@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/13 16:15:29 by jinkim            #+#    #+#             */
/*   Updated: 2021/03/13 20:04:22 by jinkim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSE_HPP
# define CONFIGPARSE_HPP

# include <iostream>
# include <string>
# include <map>
# include <fstream>

# define CONFIG_PATH "./config.ini"

typedef struct s_location
{
    std::string root;
    std::string *index;
    std::string *method;
    std::string cgi;
    bool        autoindex;

}t_location;

typedef struct s_server
{
    int         port;
    std::string host;
    std::string name;
    std::string clientMaxBodySize;
    std::string errorRoot;
    std::map<int, std::string> errorPage;
    t_location  server;
}t_server;

class ConfigParse{
private:
    void createLocation(std::string type, std::string str);
    
public:
    ConfigParse();
    virtual ~ConfigParse();
    ConfigParse(ConfigParse const &copy);
    ConfigParse &operator=(ConfigParse const &ref);

    std::map<std::string ,t_location> location;
    t_server server;
    

};

#endif