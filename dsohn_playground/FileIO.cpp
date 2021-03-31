#include "FileIO.hpp"
#include "Exception.hpp"

jachoi::FileIO::FileIO(const std::string& path):_path(path){}

std::string jachoi::FileIO::read(size_t n)
{
	char buf[1];
	bool eof = false;
	int fd = open(_path.c_str(), O_RDONLY);
	if (fd == -1)
		throw Exception("FileIO: File not exist");
	_buf.clear();
	for (size_t i = 0 ; i < n  && !eof; i++)
	{
		switch (::read(fd, buf, 1))
		{
		case 1:
			_buf.push_back(buf[0]);
			break;
		case -1:
			throw Exception("FileIO: Read failed");
		case 0:
			eof = true;
			break;
		}
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

bool jachoi::FileIO::append(const std::string& content)
{
	int fd = open(_path.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0644);
	if (fd == -1)
		return false;
	::write(fd, content.c_str(), content.size());
	close(fd);
	return true;
}
