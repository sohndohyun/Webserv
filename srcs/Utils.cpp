#include <fcntl.h>
#include <string>
#include <map>
#include <algorithm>
#include <sys/types.h>
#include <dirent.h>
#include "Utils.hpp"
#include "Exception.hpp"
#include <stack>
#include <string>
#include <iostream>

namespace utils
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

	std::string makeGMT(const std::string &tm_zone, time_t tv_sec)
	{
		struct tm time;
		char buf[64] = {0};
		std::map<std::string, int> time_diff;

		time_diff["KST"] = 9 * 60 * 60;
		time_diff["JST"] = 9 * 60 * 60;
		time_diff["CTT"] = 8 * 60 * 60;
		time_diff["ECT"] = 1 * 60 * 60;
		time_diff["PST"] = -7 * 60 * 60;
		time_t gmt_time = 0;
		if (tm_zone == "KST" || tm_zone ==  "JST" || tm_zone == "CTT" || tm_zone == "ECT" || tm_zone == "PST")
			gmt_time = tv_sec - time_diff[tm_zone];
		strptime(utils::to_string(gmt_time).c_str(), "%s", &time);
		strftime(buf, sizeof(buf), "%a, %d %b %G %T GMT", &time);
		return std::string(buf);
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

	bool isHex(const std::string &num, size_t begin, size_t end)
	{
		if (!num.size() || begin >= end || end == std::string::npos)
			return false;
		const std::string hex = "0123456789abcdef";
		for (size_t i = begin; i < end && i < num.size(); i++)
		{
			size_t idx = hex.find(num[i]);
			if (idx == std::string::npos)
				return false;
		}
		return true;
	}

	int stoi(const std::string& str)
	{
		int result = 0;
		for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
		{
			result *= 10;
			result += *it - '0';
		}
		return result;
	}
	void memcpy(char *dst, const char* src, size_t len)
	{
		for (size_t i = 0 ; i < len ; i++)
			dst[i] = src[i];
	}

	char** mtostrarr(std::map<std::string, std::string> map_env)
	{
		char** ret = new char*[map_env.size() + 1];
		size_t i = 0;
		for (std::map<std::string, std::string>::iterator it = map_env.begin(); it != map_env.end() ; it++, i++)
		{
			size_t sz = it->first.size() + it->second.size() + 1;
			ret[i] = new char[sz + 1];
			memcpy(ret[i], it->first.data(), it->first.size());
			ret[i][it->first.size()] = '=';
			memcpy(&ret[i][it->first.size() + 1], it->second.data(), it->second.size());
			ret[i][sz] = 0;
		}
		ret[map_env.size()] = 0;
		return ret;
	}

	std::vector<std::string> getDirNames(std::string path)
	{
		DIR *dir_info;
		struct dirent *dir_entry;
		std::vector<std::string> names;

		dir_info = opendir(path.c_str());
		if (NULL != dir_info)
		{
			while((dir_entry = readdir(dir_info)))
				names.push_back(dir_entry->d_name);
			closedir(dir_info);
		}
		return (names);
	}

	std::string to_string(long l)
	{
		std::string s;
		std::stack<char> stk;

		if (l < 0)
		{
			s.append("-");
			l *= -1;
		}
		while (l)
		{
			stk.push('0' + l % 10);
			l /= 10;
		}
		if (stk.empty())
			return "0";
		while (stk.size())
		{
			s.append(&stk.top(), 1);
			stk.pop();
		}
		return s;
	}

	std::vector<std::string> splitString(std::string str, char c)
	{
		std::vector<std::string> rtn;
		int start, end;
		for(start = 0; str[start] == c && start < (int)str.length(); start++) ;
		for(end = str.length() - 1; str[end] == c && end >= 0; end--) ;
		str = str.substr(start, end - start + 1);

		std::string tmp;
		tmp += str[0];
		for(int i = 1; i < (int)str.length(); i++)
		{
			if (str[i] == c && str[i - 1] == c)
				continue ;
			tmp += str[i];
		}
		str = tmp;

		int count = 0;
		for(int idx = 0; str[idx]; idx++)
		{
			if (str[idx] == c)
				count++;
		}
		if (count != 0 || (count == 0 && str != ""))
			count++;

		for(int i = 0; i < count; i++)
		{
			int len = str.find(c);
			rtn.push_back(str.substr(0, len));
			str = str.substr(len + 1, str.size() - len);
		}
		return (rtn);
	}

	int base64Decode(const std::string& str, int numBytes, std::string &dst)
	{
		const static int DecodeMimeBase64[256] = {
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
			52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
			-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
			15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
			-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
			41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
		};

		std::string cp = str;
		int d, prev_d = 0;
		unsigned char c;
		int phase = 0;
		for (int idx = 0; idx < (int)str.length(); ++idx) {
			d = DecodeMimeBase64[(int)cp[idx]];
			if (d != -1) {
				switch (phase) {
				case 0:
					++phase;
					break;
				case 1:
					c = ((prev_d << 2) | ((d & 0x30) >> 4));
					if ((int)dst.length() < numBytes)
						dst += c;
					++phase;
					break;
				case 2:
					c = (((prev_d & 0xf) << 4) | ((d & 0x3c) >> 2));
					if ((int)dst.length() < numBytes)
						dst += c;
					++phase;
					break;
				case 3:
					c = (((prev_d & 0x03) << 6) | d);
					if ((int)dst.length() < numBytes)
						dst += c;
					phase = 0;
					break;
				}
				prev_d = d;
			}
		}
		return (int)dst.length();

	}

	int open(const char* path, int flag, int chmod)
	{
		int fd = ::open(path, flag, chmod);
		fcntl(fd, F_SETFL, O_NONBLOCK);
		return fd;
	}

	std::map<std::string, std::string> set_cgi_enviroment(ConfigParse::t_conf& conf ,Request& req, const std::string& path, int port)
	{
		std::map<std::string, std::string> map_env;
		map_env["AUTH_TYPE"] = "null";
		map_env["CONTENT_LENGTH"] = "-1";
		map_env["CONTENT_TYPE"] = "null";
		map_env["GATEWAY_INTERFACE"] = "CGI/1.1";
		map_env["PATH_INFO"] = req.path;
		map_env["PATH_TRANSLATED"] = path;
		map_env["QUERY_STRING"] = req.querystring;
		map_env["REMOTE_ADDR"] = "127.0.0.1";
		map_env["REMOTE_IDENT"] = "null";
		map_env["REMOTE_USER"] = "null";
		map_env["REQUEST_METHOD"] = req.method;
		map_env["REQUEST_URI"] = req.path;
		map_env["SCRIPT_NAME"] = req.path;
		map_env["SERVER_NAME"] = conf.server.name;
		map_env["SERVER_PORT"] = to_string(port);
		map_env["SERVER_PROTOCOL"] = "HTTP/1.1";
		map_env["SERVER_SOFTWARE"] = "Webserv";
		for (std::map<std::string ,std::string>::iterator it = req.header.begin();
				it != req.header.end(); it++)
		{
			std::string key;
			if (it->first.size())
			{
				key.append("HTTP_");
				for (size_t i = 0 ; i < it->first.size(); i++)
				{
					if (it->first[i] == '-')
						key += '_';
					else
						key += std::toupper(it->first[i]);
				}
				map_env[key] = it->second;
			}
		}
		return map_env;
	}
}