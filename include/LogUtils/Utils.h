#pragma once
#include <iostream>
#include <string>

class LogWriter{
	std::ostream & stream;
public:
	LogWriter(std::ostream &_stream) : stream(_stream) {}
	void print_string(std::string & str);
};


