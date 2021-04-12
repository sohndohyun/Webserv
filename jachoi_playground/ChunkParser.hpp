#pragma once
#include <string>

class ChunkParser
{
private:
	std::string _data;
public:
	ChunkParser(std::string& body);
	virtual ~ChunkParser();
	// const std::string& getData();
};
