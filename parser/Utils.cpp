#include <string>
#include <algorithm>
namespace jachoi
{
	std::string ltrim(std::string s)
	{
		s.erase(0, s.find_first_not_of(' '));
		return s;
	}

	std::string rtrim(std::string s)
	{
		s.erase(s.find_last_not_of(' '), s.size());
		return s;
	}

	std::string trim(std::string s)
	{
		return ltrim(rtrim(s));
	}
}