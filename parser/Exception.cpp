#include "Exception.hpp"

Exception::Exception(const std::string& msg): msg(msg){}
const char* Exception::what()const _NOEXCEPT{return msg.c_str();}
Exception::~Exception(){}