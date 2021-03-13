#pragma once
#include <string>
#include <vector>
#include <map>
class pathparser
{
public:
	std::string path;
	std::map<std::string, std::string>* query;
	std::string fragment;
public:
	pathparser(const std::string& s);
	virtual ~pathparser();
private:
	std::map<std::string, std::string>* splitIntoMap(std::string s, char sp1, char sp2);
};
