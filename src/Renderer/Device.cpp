#include "Renderer/Device.h"

Renderer::Device::Device() :
		loop_finit(false), page_flip_pending(false), buffer_vec(), device_log(
				"device : ", std::cerr), current_buffer(0) {
	device_log() << "Device init...";
	Loader * loader = Loader::getInstance();
	std::pair<drmModeConnector *, drmModeCrtc *> init_pair =
			loader->getNextConnectorPair();
	conn = init_pair.first;
	old_crtc = init_pair.second;
	addBuffer();
	int ret = drmModeSetCrtc(loader->fd, old_crtc->crtc_id,
			buffer_vec.back()->fb, 0, 0, &conn->connector_id, 1, conn->modes);
	if (ret) {
		throw DeviceException("cant set old_crtc");
	}

}

Renderer::Device::~Device() {
	device_log() << "device removed ...";
	for (size_t i = 0; i < buffer_vec.size(); i++) {
		delete buffer_vec[i];
		delete draw_buffer_vec[i];
	}
	for(size_t i = 0; i < drawable_vec.size(); i++){
		delete drawable_vec[i].second;
	}
	drmModeSetCrtc(Loader::getInstance()->fd, old_crtc->crtc_id,
				old_crtc->buffer_id, old_crtc->x, old_crtc->y,
				&conn->connector_id, 1, &old_crtc->mode);
	drmModeFreeCrtc(old_crtc);
	drmModeFreeConnector(conn);
}

void Renderer::Device::addBuffer() {
	Buffer * add_buffer = new Buffer(
			BufferInfo(old_crtc->width, old_crtc->height));
	buffer_vec.push_back(add_buffer);
	draw_buffer_vec.push_back(add_buffer->getDrawBuffer());
}

Renderer::Buffer::DrawBuffer * Renderer::Device::getDrawBuffer() {
	return buffer_vec[current_buffer]->getDrawBuffer();
}

void Renderer::Device::applyDrawBuffer(
		Renderer::Buffer::DrawBuffer * draw_buffer) {
	buffer_vec[current_buffer]->applyDrawBuffer(draw_buffer);
}

void Renderer::Device::pageFlipped(int fd, uint32_t frame, uint32_t sec,
		uint32_t usec, void *data) {
	Device *dev = (Device *) data;
	dev->page_flip_pending = false;
	if(dev->drawable_vec.empty()) {
		dev->loop_finit = true;
	}
	if (dev->loop_finit) {
		return;
	}
	Buffer::DrawBuffer * cur_draw_buffer =
			dev->draw_buffer_vec[dev->current_buffer];
	Buffer * cur_buffer = dev->buffer_vec[dev->current_buffer];
	for(int32_t i = dev->drawable_vec.size() - 1; i >= 0; i--){
		auto & drawable = dev->drawable_vec[i];
		drawable.first->onDraw(cur_draw_buffer, drawable.second);
		if(drawable.second->flag_finit)
			std::swap(dev->drawable_vec[i], dev->drawable_vec.back());
	}

	while(!dev->drawable_vec.empty() && dev->drawable_vec.back().second->flag_finit){
		delete dev->drawable_vec.back().second;
		dev->drawable_vec.pop_back();
	}

	cur_buffer->applyDrawBuffer(cur_draw_buffer);
	int ret = drmModePageFlip(fd, dev->old_crtc->crtc_id, cur_buffer->fb,
	DRM_MODE_PAGE_FLIP_EVENT, dev);
	if (ret) {
		dev->device_log() << "failed_to_page_flip";
	} else {
		dev->switchBuffer();
		dev->page_flip_pending = true;
	}
}

void Renderer::Device::switchBuffer() {
	current_buffer = (current_buffer + 1) % buffer_vec.size();
}

void Renderer::Device::startLoop(int32_t loop_timeout, int32_t loop_end) {
	int32_t ret;
	fd_set fds;
	time_t start, cur;
	struct timeval v;
	drmEventContext ev;

	/* init variables */
	Loader * loader = Loader::getInstance();
	FD_ZERO(&fds);
	srand(time(&start));
	memset(&v, 0, sizeof(v));
	memset(&ev, 0, sizeof(ev));
	ev.version = DRM_EVENT_CONTEXT_VERSION;
	ev.page_flip_handler = pageFlipped;
	loop_finit = false;
	/* first page flip	 */
	drmModePageFlip(loader->fd, old_crtc->crtc_id, buffer_vec[0]->fb,
	DRM_MODE_PAGE_FLIP_EVENT, (void *) this);
	/* basic version */
	while (!loop_finit &&( loop_end == -1 || time(&cur) < start + loop_end)) {
		FD_SET(0, &fds);
		FD_SET(loader->fd, &fds);
		v.tv_usec = loop_timeout;
		ret = select(loader->fd + 1, &fds, NULL, NULL, &v);
		if (ret < 0) {
			device_log() << "select failed";
			break;
		} else if (FD_ISSET(0, &fds)) {
			device_log() << "user_interrupted";
			break;
		} else if (FD_ISSET(loader->fd, &fds)) {
			drmHandleEvent(loader->fd, &ev);
		}
	}
	loop_finit = true;
	while (page_flip_pending) {
		ret = drmHandleEvent(loader->fd, &ev);
	}
}



void Renderer::Device::addDrawable(Drawable* what) {
	drawable_vec.push_back(std::make_pair(what, (new DrawableInfo)));
}
