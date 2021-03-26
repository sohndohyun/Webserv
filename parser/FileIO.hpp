#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
namespace jachoi
{
	class FileIO
	{
	private:
		std::string _path;
		std::vector<char> _buf;
	public:
		FileIO(const std::string& path);
		std::string read(int n);
		bool write(const std::string& content);
	};
} // namespace jachoi
