#ifndef FILEIO_HPP
#define FILEIO_HPP
#include <string>
#include <vector>

namespace utils
{
	class FileIO
	{
	private:
		std::string _path;
		std::vector<char> _buf;
	public:
		FileIO(const std::string& path);
		bool read(std::string& str);
		bool write(const std::string& content);
		bool append(const std::string& content);
	};
}
#endif
