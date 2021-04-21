#include "Request.hpp"
#include "Utils.hpp"
#include <iostream>

Request::Request()
{
	init();
}

void Request::init()
{
	isHeaderMade = false;
	isChunk = false;
	isBodyMade = false;
	method = "";
	errorCode = 200;
	path = "";
	bodysize = 0;

	leftStr.clear();
	body.clear();
	header.clear();
}

void Request::add(std::string const &str)
{
	leftStr.append(str);
	if (!isHeaderMade)
		parseHeader();
	if (isHeaderMade && !isBodyMade)
		parseBody();
}

bool Request::valid() const
{
	return (errorCode == 200) && isBodyMade;
}

bool Request::needRecv() const
{
	return !isHeaderMade || !isBodyMade;
}

MethodType Request::methodType() const
{
	const char* _known_methods[] = {
		"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE" 
	};
	for (size_t i = 0 ; i < sizeof(*_known_methods) ; i++)
	{
		if (_known_methods[i] == method)
			return (MethodType)i;
	}
	return INVLAID;
}

void Request::parseFirstLine(std::string const &str)
{
	const char* _known_methods[] = {
		"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE" 
	};
	const char* _supported_version[] = {
		"HTTP/1.1", "HTTP/1.0"
	};
	size_t first = str.find_first_of(' ');
	size_t last = str.find_last_of(' ');
	if (first == last)
	{
		errorCode = 400;
		return;
	}
	method = str.substr(0, first);
	if (std::find(_known_methods, _known_methods + 8, method) == _known_methods + 8)
	{
		errorCode = 400;
		return;
	}
	std::string httpVersion = str.substr(last + 1, str.size());
	if (!httpVersion.size() || (std::find(_supported_version, _supported_version + 2, httpVersion) == _supported_version + 2))
	{
		errorCode = 505;
		return;
	}
	path = str.substr(first + 1, last - first -1);
	size_t i;
	for (i = 0 ; (i < path.size()) && path[i] != '#' && path[i] != '?'; i++);
	path = path.substr(0, i);
}

void Request::parseHeader()
{
	if (leftStr.find("\r\n\r\n") == std::string::npos)
		return;

	size_t begin = leftStr.find("\r\n");
	std::string line = leftStr.substr(0, begin);
	parseFirstLine(line);
	begin += 2;
	while (true)
	{
		size_t end = leftStr.find("\r\n", begin);
		line = leftStr.substr(begin, end - begin);
		if (line.size())
		{
			size_t colon_pos = line.find(":");
			if (colon_pos == std::string::npos)
				errorCode = 400;
			else
				header[line.substr(0, colon_pos)] = jachoi::ltrim(line.substr(colon_pos + 1));
			begin = end + 2;
		}
		else
		{
			if (leftStr.substr(begin, 2) == "\r\n")
			{
				isHeaderMade = true;

				leftStr = leftStr.substr(begin + 2);
				isChunk = false;
				std::map<std::string, std::string>::const_iterator hit = header.find("Transfer-Encoding");
				isChunk = (hit != header.end() && hit->second.find("chunked") != std::string::npos);

				if (!isChunk)
				{
					bodysize = 0;
					hit = header.find("Content-Length");
					if (hit != header.end())
						bodysize = std::atoi(hit->second.c_str());
				}
				break;
			}
			else
				begin += 2;
		}
	}
}

void Request::parseBody()
{
	if (isChunk)
	{
		size_t begin = 0;
		size_t end = leftStr.find("\r\n");

		while (jachoi::isHex(leftStr, begin, end))
		{
			size_t blockSize = jachoi::htoi(leftStr.substr(begin, end - begin));

			if (blockSize == 0) // 찐막
			{
				if (leftStr.substr(end) == "\r\n\r\n")
				{
					isBodyMade = true;
					leftStr.clear();
				}
				else
					leftStr = leftStr.substr(begin);
				break;
			}
			else 
			{
				size_t tbegin = end + 2;
				end = tbegin + blockSize;
				if (leftStr.size() - tbegin < blockSize + 2)
				{
					leftStr = leftStr.substr(begin);
					break;
				}
				else if (leftStr.size() - tbegin == blockSize + 2)
				{
					body.append(leftStr.substr(tbegin, blockSize));
					leftStr.clear();
					break;
				}
				else if (leftStr.find("\r\n", end + 2) == std::string::npos)
				{
					body.append(leftStr.substr(tbegin, blockSize));
					leftStr = leftStr.substr(end + 2);
					break;
				}
				begin = tbegin;
				body.append(leftStr.substr(begin, end - begin));
			}
			begin = end + 2;
			end = leftStr.find("\r\n", begin);
		}

	}
	else
	{
		if (static_cast<size_t>(bodysize) < leftStr.size())
			std::cout << "ERROR!!" << std::endl;
		body.append(leftStr);
		leftStr.clear();
		if (body.size() == static_cast<size_t>(bodysize))
			isBodyMade = true;
	}
}
