#include <string>
#include <map>
#include <algorithm>
#include "Utils.hpp"
#include "Exception.hpp"
#define debug
#ifdef debug
#include <iostream>
using namespace std;
#endif


extern std::map<std::string, std::string> g_envp;

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

	void	*memset(void *s, int c, unsigned long n)
	{
		unsigned char	*ptr;

		ptr = static_cast<unsigned char*>(s);
		while (n-- > 0)
			*ptr++ = c;
		return (s);
	}

	bool islittelendian()
	{
		int n = 1;
		if (((char*)(&n))[0] == 1)
			return true;
		return false;
	}

	short htons(short n)
	{
		if (islittelendian())
		{
			for (int i = 0 ; i < 2 / 2; i++)
			{
				char tmp = ((char*)&n)[1 - i];
				((char*)&n)[1 - i] = ((char*)&n)[i];
				((char*)&n)[i] = tmp;
			}
		}
		return n;
	}

	long htonl(long n)
	{
		if (islittelendian())
		{
			for (int i = 0 ; i < 8 / 2; i++)
			{
				char tmp = ((char*)&n)[7 - i];
				((char*)&n)[7 - i] = ((char*)&n)[i];
				((char*)&n)[i] = tmp;
			}
		}
		return n;
	}

	std::string makeGMT(const char *tm_zone, time_t tv_sec)
	{
		struct tm time;
		char buf[64];
		std::map<std::string, int> time_diff;
		time_diff["KST"] = 9 * 60 * 60;
		time_diff["JST"] = 9 * 60 * 60;
		time_diff["CTT"] = 8 * 60 * 60;
		time_diff["ECT"] = 1 * 60 * 60;
		time_diff["PST"] = -7 * 60 * 60;

		time_t gmt_time = tv_sec - time_diff[std::string(tm_zone)];
		strptime(std::to_string(gmt_time).c_str(), "%s", &time);
		strftime(buf, sizeof(buf), "%a, %d %b %G %T GMT", &time);
		return (buf);
	}

	int htoi(const std::string& num)
	{
		int ret = 0;
		const std::string hex = "0123456789abcdef";
		for (size_t i = 0 ; i < num.size(); i++)
		{
			size_t idx = hex.find(num[i]);
			if (idx == std::string::npos)
				return ret;
			ret *= 16;
			ret += idx;
		}
		return ret;
	}

	std::string to_string(long num)
	{
		(void)num;
		// TODO
		return "";
	}

	std::map<std::string, std::string> make_envp(char** envp)
	{
		for (size_t i = 0 ; envp[i]; i++)
		{
			std::string env(envp[i]);
			size_t eq_pos = env.find('=');
			if (eq_pos == std::string::npos)
				throw Exception("invalide envp");
			g_envp[env.substr(0, eq_pos)] = env.substr(eq_pos + 1);
		}
		return g_envp;
	}

	void set_env(const std::string key, const std::string value)
	{
		// cerr <<  key << "=" << value << endl;
		g_envp[key] = value;
	}

	void memcpy(char *dst, const char* src, size_t len)
	{
		for (size_t i = 0 ; i < len ; i++)
			dst[i] = src[i];
	}

	void print_2darr(char **envp)
	{
		for (int i = 0 ; envp[i] ; i++)
			cout << envp[i] << endl;
	}
	char** get_envp()
	{
		char** ret = new char*[g_envp.size() + 1];
		size_t i = 0;
		for (std::map<std::string, std::string>::iterator it = g_envp.begin(); it != g_envp.end() ; it++, i++)
		{
			size_t sz = it->first.size() + it->second.size() + 1;
			ret[i] = new char[sz + 1];
			memcpy(ret[i], it->first.data(), it->first.size());
			ret[i][it->first.size()] = '=';
			memcpy(&ret[i][it->first.size() + 1], it->second.data(), it->second.size());
			ret[i][sz] = 0;
		}
		ret[g_envp.size()] = 0;
		// print_2darr(ret);
		return ret;
	}
}
