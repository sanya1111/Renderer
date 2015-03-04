#include "Renderer/Exception.h"

Renderer::Exception::Exception(const Exception& other) {
	msg = other.msg;
}

Renderer::Exception::Exception(const std::string& str) {
	msg = str;
}

const char* Renderer::Exception::what() const noexcept{
	return msg.c_str();
}
