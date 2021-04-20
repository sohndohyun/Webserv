#include <string>
#include <map>
#include <algorithm>
#include <vector>
#ifdef htons
#undef htons
#endif
#ifdef htonl
#undef htonl
#endif

namespace jachoi
{
	std::string ltrim(std::string s);
	std::string rtrim(std::string s);
	std::string trim(std::string s);
	void	*memset(void *s, int c, unsigned long n);
	short htons(short n);
	long htonl(long n);
	std::string makeGMT(const std::string& time_zone, time_t tv_sec);
	int htoi(const std::string& num);
	bool isHex(const std::string &num, size_t begin, size_t end);
	std::string to_string(long l);
	char** mtostrarr(std::map<std::string, std::string> map_env);
	std::vector<std::string> getDirNames(std::string path);
	int stoi(const std::string& str);

	std::vector<std::string> splitString(std::string str, char c);
	int base64Decode(const std::string& str, int numBytes, std::string &dst);
	int open(const char* path, int flag, int chmod = 0644);
}