#include "ChunkParser.hpp"
#include "Utils.hpp"
#define debug
#ifdef debug
#include <iostream>

using namespace std;
#endif
ChunkParser::ChunkParser(const std::string& body)
{
	size_t idx = 0;
	int len;
	const char* data = body.data();
	while ((len = jachoi::htoi(&data[idx])))
	{
		// cout << "hex : " <<  body.substr(idx, 5) << endl;
		while (data[idx++] != '\n');
		_data += body.substr(idx + 1, len);
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
