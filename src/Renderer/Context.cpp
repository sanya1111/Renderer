#include "Renderer/Context.h"
#include <algorithm>

Renderer::Context::Context(size_t buffers_num, Connector& conn, Crtc& crtc,  std::unique_ptr<Drawable> &&drawable,
		Device& dev) : dev(dev), buffers_num(buffers_num), connectors(), saved_crtc(crtc), drawable(std::move(drawable)), buffers(buffers_num){
	try{
		std::generate(buffers.begin(), buffers.end(), [&dev, this](){return dev.createBuffer(saved_crtc);});
	}catch(const std::exception&){
		throw ContextException("cannot create buffers");
	}
	current_buffer = 0;
	dev.useCrtc(saved_crtc);
	addConnector(conn);
}

Renderer::Context::~Context() {
	for_each(buffers.begin(), buffers.end(), [this](Buffer &cur){dev.destroyBuffer(cur);});
	for_each(connectors.begin(), connectors.end(), [this](Connector & conn) {dev.setCrtc(conn, saved_crtc);});
}

void Renderer::Context::addConnector(Connector & conn) {
	if(!dev.isCompatible(conn, saved_crtc)){
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

void Renderer::Context::onPageFlipped(uint32_t, uint32_t sec, uint32_t usec) {
	fps_counter.frame(sec * 1000 + usec / 1000);
	drawable->onDraw(*this);
	dev.pageFlip(saved_crtc, buffers[current_buffer], (void *)this);
	changeBuffer();
}

void Renderer::Context::FpsCounter::frame(int32_t delta) {
	frames_total++;
	if(last){
		time_total += delta - last;
	}
	last = delta;
}

float Renderer::Context::getFps() {
	return float(fps_counter.frames_total) / (float(fps_counter.time_total) / 1000.0);
}

int64_t Renderer::Context::getTime() {
	return fps_counter.time_total;
}

int64_t Renderer::Context::getFrameCount() {
	return fps_counter.frames_total;
}

Renderer::Buffer& Renderer::Context::getCurrentBuffer() {
	return buffers[current_buffer];
}

void Renderer::Context::quitProcess() {
	dev.finishLoop();
}
