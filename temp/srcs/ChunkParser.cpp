#include "ChunkParser.hpp"
#include "Utils.hpp"

ChunkParser::ChunkParser(const std::string& body)
{
	size_t idx = 0;
	int len;
	const char* data = body.data();
	while ((len = jachoi::htoi(&data[idx])))
	{
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
