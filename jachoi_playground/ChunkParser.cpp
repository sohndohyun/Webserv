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
	clock_t start = clock();
	const char* data = body.data();
	while ((len = jachoi::htoi(&data[idx])))
	{
		while (data[idx++] != '\n');
		_data.append(body.substr(idx + 1, len));
		idx += len + 2;
	}
	clock_t end = clock();
	cout << "body size : " << body.size() << endl;
	cout << "chunk time : "<< (double)(end-start)/CLOCKS_PER_SEC<< endl;
}

ChunkParser::~ChunkParser()
{
}

const std::string& ChunkParser::getData()
{
	return _data;
}
