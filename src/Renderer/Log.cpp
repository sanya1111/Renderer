#include "Renderer/Log.h"
#include <iostream>




Renderer::Log::Log(const std::string _special_prefix,
		std::ostream& from) : special_prefix(_special_prefix),
		log_stream(from){
}

std::ostream& Renderer::Log::operator ()() {
	log_stream << std::endl << special_prefix;
	return log_stream;
}

Renderer::Log& Renderer::Log::operator =(const Renderer::Log&other) {
	special_prefix = other.special_prefix;
	return *this;
}
