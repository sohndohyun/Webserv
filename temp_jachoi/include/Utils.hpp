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
	std::string makeGMT(const char *time_zone, time_t tv_sec);
	int htoi(const std::string& num);
	bool isHex(const std::string &num, size_t begin, size_t end);

	char** mtostrarr(std::map<std::string, std::string> map_env);
	std::vector<std::string> getDirNames(std::string path);
}