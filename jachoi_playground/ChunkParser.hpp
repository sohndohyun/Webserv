#pragma once
#include <string>

class ChunkParser
{
private:
	std::string data;
public:
	ChunkParser(const std::string& body);
	virtual ~ChunkParser();
	const std::string& getData();
};
