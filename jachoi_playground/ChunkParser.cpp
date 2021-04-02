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
	while ((len = jachoi::htoi(&body.data()[idx])))
	{
		cout << "hex : " <<  body.substr(idx, 5) << endl;
		while (body.data()[idx++] != '\n');
		idx++;
		data += body.substr(idx, len);
		idx += len + 1;
	}
}

ChunkParser::~ChunkParser()
{
}

const std::string& ChunkParser::getData()
{
	return data;
}
