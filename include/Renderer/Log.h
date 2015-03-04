#ifndef LOG_H_
#define LOG_H_
#include <string>
#include <iostream>

namespace Renderer{
	class Log{
	private:
		std::ostream & log_stream;
		std::string special_prefix;
	public:
		Log(const std::string _special_prefix = "",
				std::ostream & from = std::cerr);
		std::ostream & operator()();
	};
}


#endif /* LOG_H_ */
