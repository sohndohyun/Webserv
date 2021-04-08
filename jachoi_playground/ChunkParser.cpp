#include "ChunkParser.hpp"
#include "Utils.hpp"
#define debug
#ifdef debug
#include <iostream>
#include <ctime>
using namespace std;
#endif
ChunkParser::ChunkParser(const std::string& body)
{
	size_t idx = 0;
	int len;
	const char* data = body.data();
	_data.reserve(0x10000000);
	while ((len = jachoi::htoi(&data[idx]))) // hex to int
	{
		while (data[idx++] != '\n');
		_data.append(body.substr(idx + 1, len));
		idx += len + 2;
	}
}

ChunkParser::~ChunkParser()
{
}

const std::string& ChunkParser::getData()
{
	return _data;
}
