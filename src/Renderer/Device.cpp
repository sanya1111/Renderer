#include "Renderer/Device.h"

Renderer::Device::Device() : buffer_vec(), device_log("device : ", std::cerr), current_buffer(0){
	device_log() << "Device init...";
	Loader & loader = Loader::getInstance();
	std::pair<drmModeConnector *, drmModeCrtc *> init_pair = loader.getNextConnectorPair();
	conn = init_pair.first;
	old_crtc = init_pair.second;
	addBuffer();
	//===test==
//	unsigned int b = 0x01,
//	g = rand() % 0xff,
//	r = rand() % 0x01;
//	int32_t * buf = (int32_t *)buffer_vec.back()->map;
//	int width = buffer_vec.back()->buf_info.width;
//	int height = buffer_vec.back()->buf_info.height;
//	for (int32_t j = 0; j < height; ++j) {
//	for (int32_t k = 0; k < width; ++k) {
//	int32_t off = j * width + k;
//	(buf + off)[k] =
//	(r << 16) | (g << 8) | b;
//	}
//	}
	//===========
	int ret = drmModeSetCrtc(loader.fd, old_crtc->crtc_id, buffer_vec.back()->fb, 0, 0,
				 &conn->connector_id, 1, conn->modes);
	if (ret){
		throw DeviceException("cant set old_crtc");
	}

}

Renderer::Device::~Device() {
	for(size_t i = 0; i < buffer_vec.size(); i++){
		delete buffer_vec[i];
	}
	drmModeSetCrtc(Loader::getInstance().fd,
				       old_crtc->crtc_id,
				       old_crtc->buffer_id,
				       old_crtc->x,
				       old_crtc->y,
				       &conn->connector_id,
				       1,
				       &old_crtc->mode);
	drmModeFreeCrtc(old_crtc);
	drmModeFreeConnector(conn);
}

void Renderer::Device::addBuffer() {
	Buffer * add_buffer = new Buffer(BufferInfo(old_crtc->width, old_crtc->height));
	buffer_vec.push_back(add_buffer);
}

Renderer::Buffer::DrawBuffer Renderer::Device::getDrawBuffer() {
	return buffer_vec[current_buffer]->getDrawBuffer();
}

void Renderer::Device::applyDrawBuffer(Renderer::Buffer::DrawBuffer & draw_buffer) {
	buffer_vec[current_buffer]->applyDrawBuffer(draw_buffer);
}

void Renderer::Device::switchBuffer() {
	current_buffer = (current_buffer + 1) % buffer_vec.size();
}
