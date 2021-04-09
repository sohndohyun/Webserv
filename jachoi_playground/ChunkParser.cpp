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
	// _data.reserve(0x10000000);
	// cout << body.size() << endl;
	// cout <<" ===body=== "<< endl;
	// cout << body << endl;
	cout << "chunking..." << endl;
	while ((len = jachoi::htoi(&body[idx]))) // hex to int
	{
		while (body[idx++] != '\n');
		_data.append(body.substr(idx, len));
		idx += len + 2;
		// cout << body << endl;
	}
	cout << _data.size() << endl;
}

ChunkParser::~ChunkParser()
{
}

const std::string& ChunkParser::getData()
{
	return _data;
}
