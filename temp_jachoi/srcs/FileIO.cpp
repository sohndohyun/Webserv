#include "FileIO.hpp"
#include "Exception.hpp"
#include <string>

jachoi::FileIO::FileIO(const std::string& path):_path(path){}

bool jachoi::FileIO::read(std::string& str)
{
	int fd = open(_path.c_str(), O_RDONLY);
	char buf[1000000];

	while (true)
	{
		int ret = ::read(fd, buf, 999999);
		if (ret == -1)
			return false;
		else if (ret == 0)
			break;
		buf[ret] = 0;
		str.append(buf);
	}
	close(fd);
	// return std::string(_buf.begin(), _buf.end());
	return true;
}

bool jachoi::FileIO::write(const std::string& content)
{
	int fd = open(_path.c_str(), O_CREAT | O_WRONLY, 0644);
	if (fd == -1)
		return false;
	::write(fd, content.c_str(), content.size());
	close(fd);
	return true;
}

bool jachoi::FileIO::append(const std::string& content)
{
	int fd = open(_path.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0644);
	if (fd == -1)
		return false;
	::write(fd, content.c_str(), content.size());
	close(fd);
	return true;
}
