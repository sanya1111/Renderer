#ifndef CONTEXT_H_
#define CONTEXT_H_

#include "Renderer/Buffer.h"
#include "Renderer/Device.h"
#include "Renderer/Drawable.h"

namespace Renderer{

class Context{
protected:
	Buffer * buffers;
	int32_t current_buffer;
	size_t buffers_num;
	std::vector<Connector> connectors;
	Crtc saved_crtc;
	Device &dev;
	Drawable & drawable;
	void changeBuffer();
public:
	class ContextException : public std::runtime_error{
	public:
		using std::runtime_error::runtime_error;
	};
	Context(size_t buffers_num, Connector &conn, Crtc & crtc, Drawable &drawable	, Device & dev);
	~Context();
	void addConnector(Connector & );
	void startListenning();
	void onPageFlipped(uint32_t frame, uint32_t sec, uint32_t usec);
};

}



#endif /* CONTEXT_H_ */
