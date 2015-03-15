#include "Renderer/Context.h"

Renderer::Context::Context(size_t buffers_num, Connector& conn, Crtc& crtc, Drawable &drawable,
		Device& dev) : dev(dev), buffers_num(buffers_num), connectors(), saved_crtc(crtc), drawable(drawable){
	try{
		buffers = new Buffer[buffers_num];
		for(uint32_t i = 0 ; i < buffers_num; i++){
			buffers[i] = dev.createBuffer(saved_crtc);
		}
	}catch(std::exception&){
		throw ContextException("cannot create buffers");
	}
	current_buffer = 0;
	dev.useCrtc(saved_crtc);
	addConnector(conn);
}

Renderer::Context::~Context() {
	for(uint32_t i = 0; i < buffers_num; i++){
		dev.destroyBuffer(buffers[i]);
	}
	delete [] buffers;
	for(auto &conn : connectors){
			dev.setCrtc(conn, saved_crtc);
	}
}

void Renderer::Context::addConnector(Connector & conn) {
	if(!dev.isPossiblePair(conn, saved_crtc)){
		throw ContextException("impossible pair : connector - crtc");
	}
	connectors.push_back(conn);
	dev.useConnector(conn);
	dev.setCrtcBuffer(conn, saved_crtc, buffers[current_buffer]);
}

void Renderer::Context::startListenning() {
	dev.pageFlip(saved_crtc, buffers[current_buffer], (void *)this, true);
}

void Renderer::Context::changeBuffer() {
	current_buffer = (current_buffer + 1) % buffers_num;
}

void Renderer::Context::onPageFlipped(uint32_t frame, uint32_t sec,
		uint32_t usec) {
	drawable.onDraw(frame, sec, usec, buffers[current_buffer]);
	dev.pageFlip(saved_crtc, buffers[current_buffer], (void *)this);
	changeBuffer();
};
