#include "pathparser.hpp"
#include <vector>
#include <algorithm>

std::map<std::string, std::string>* PathParser::splitIntoMap(std::string s, char sp1, char sp2)
{
	std::vector<std::string> v;
	std::map<std::string, std::string>* ret = new std::map<std::string, std::string>();
	size_t last = 0;
	for (size_t i = 0 ; i < s.size(); i++)
	{
		if (s[i] == sp2 || i == s.size() - 1)
		{
			if (i != s.size() - 1)
				v.push_back(s.substr(last, i - last));
			else
				v.push_back(s.substr(last));
			last = i + 1;
		}
	}
	for (size_t i = 0 ; i < v.size(); i++)
	{
		size_t j;
		std::string _s = v[i];
		for (j = 0 ; (j < _s.size()) && _s[j] != sp1; j++);
		(*ret)[_s.substr(0, j)] = _s.substr(j + 1);
	}
	return ret;
}

PathParser::PathParser() {}
PathParser::PathParser(const std::string& s)
{
	size_t i;
	for (i = 0 ; (i < s.size()) && s[i] != '#' && s[i] != '?'; i++);
	path = s.substr(0, i);
	if (path[0] != '/')
		throw "invalid path";
	if (s[i++] == '?')
	{
		size_t j = i;
		for (;(i < s.size()) && s[i] != '#'; i++);
		std::string querystr = s.substr(j, i - j);
		query = splitIntoMap(querystr, '=', '&');
	}
	if (s[i++] == '#')
		fragment = s.substr(i);
}

PathParser::~PathParser()
{
	if (query)
		delete query;
}
