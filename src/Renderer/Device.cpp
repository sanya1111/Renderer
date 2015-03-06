#include "Renderer/Device.h"
Renderer::Device::Device() : buffer_vec(), device_log("device : ", std::cerr){
	device_log() << "Device init...";
	Loader & loader = Loader::getInstance();
	std::pair<drmModeConnector *, drmModeCrtc *> init_pair = loader.getNextConnectorPair();
	conn = init_pair.first;
	crtc = init_pair.second;
	addBuffer();
	//===test==
	unsigned int r = rand() % 0xff,
	g = rand() % 0xff,
	b = rand() % 0xff;
	Buffer * buf = buffer_vec.back();
	for (int32_t j = 0; j < buf->height; ++j) {
	for (int32_t k = 0; k < buf->width; ++k) {
	int32_t off = buf->stride * j + k * 4;
	*(uint32_t*)&buf->map[off] =
	(r << 16) | (g << 8) | b;
	}
	}
	//===========
	int ret = drmModeSetCrtc(loader.fd, crtc->crtc_id, buffer_vec.back()->fb, 0, 0,
				 &conn->connector_id, 1, conn->modes);
	if (ret){
		throw DeviceException("cant set crtc");
	}

}

Renderer::Device::~Device() {
	for(size_t i = 0; i < buffer_vec.size(); i++){
		delete buffer_vec[i];
	}
	drmModeSetCrtc(Loader().getInstance().fd,
				       crtc->crtc_id,
				       crtc->buffer_id,
				       crtc->x,
				       crtc->y,
				       &conn->connector_id,
				       1,
				       &crtc->mode);
	drmModeFreeCrtc(crtc);
	drmModeFreeConnector(conn);
}

void Renderer::Device::addBuffer() {
	Buffer * add_buffer = new Buffer(BufferInfo(), crtc->width, crtc->height);
	buffer_vec.push_back(add_buffer);
}
