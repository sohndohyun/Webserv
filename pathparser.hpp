#pragma once
#include <string>
#include <vector>
#include <map>
class PathParser
{
public:
	std::string path;
	std::map<std::string, std::string>* query;
	std::string fragment;
public:
	PathParser(const std::string& s);
	virtual ~PathParser();
private:
	std::map<std::string, std::string>* splitIntoMap(std::string s, char sp1, char sp2);
};
