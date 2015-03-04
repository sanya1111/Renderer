#ifndef EXCEPTION_H_
#define EXCEPTION_H_
#include <string>

namespace Renderer{
	class Exception : public std::exception{
	protected:
		std::string msg;
	public:
		Exception(const Exception & );
		Exception(const std::string &);
		const char * what() const noexcept;
	};
}


#endif /* EXCEPTION_H_ */
