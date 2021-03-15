#include "fileio.hpp"

jachoi::FileIO::FileIO(const std::string& path):_path(path){}

std::string jachoi::FileIO::read(int n = std::string::npos)
{
	char buf[1];
	bool eof = false;
	int fd = open(_path.c_str(), O_RDONLY);
	if (fd == -1)
		throw "file not exist";
	_buf.clear();
	for (int i = 0 ; i < n  && !eof; i++)
	{
		if (::read(fd, buf, 1) == 0)
			eof = true;
		else
			_buf.push_back(buf[0]);
	}
	close(fd);
	return std::string(_buf.begin(), _buf.end());
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
