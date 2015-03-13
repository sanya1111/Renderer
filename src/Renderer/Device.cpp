#include "Renderer/Device.h"
#include "Renderer/Context.h"
#include <unistd.h>

Renderer::Device::Device(const std::string & file_path) {
	openDevice(file_path.c_str());
	initModeRes();
}


Renderer::Device::~Device() {
	drmModeFreeResources(drm_mode_res);
	close(fd);
}

std::vector<Renderer::Connector> Renderer::Device::getConnectorVec() {
	std::vector<Renderer::Connector> outp;
	for(int32_t i = 0; i < drm_mode_res->count_connectors; i++){
		Connector conn_obj(fd, drm_mode_res->connectors[i]);
		drmModeConnectorPtr conn = conn_obj.getDrmInstance();
		if (!conn || conn->connection != DRM_MODE_CONNECTED || !conn->modes){
			continue;
		}
		if(used_connectors.count(conn->connector_id)){
			continue;
		}
		outp.push_back(conn_obj);
	}
	return outp;
}

std::vector<Renderer::Crtc> Renderer::Device::getCrtcVec() {
	std::vector<Crtc> outp;
	for(int32_t i = 0; i < drm_mode_res->count_connectors; i++){
		uint32_t crtc = drm_mode_res->crtcs[i];
		if(used_crts.count(crtc)){
			continue;
		}
		Crtc crtc_obj(fd, crtc);
		if(!crtc_obj.getDrmInstance()){
			continue;
		}
		outp.push_back(crtc_obj);
	}
	return outp;
}


Renderer::Buffer Renderer::Device::createBuffer(Crtc &crtc_o){
	drmModeCrtcPtr crtc= crtc_o.getDrmInstance();
	if(driver_version == DriverVersion::i915){
		return Buffer(fd, crtc->width, crtc->height, DRM_FORMAT_XRGB8888, Buffer::MappingInfo::GEM_INTEL_MAPPING);
	}
	return Buffer(fd, crtc->width, crtc->height, DRM_FORMAT_RGB888, Buffer::MappingInfo::DUMB_MAPPING);
}

void Renderer::Device::useCrtc(Renderer::Crtc &item) {
	used_crts.insert(item.getDrmInstance()->crtc_id);
}

void Renderer::Device::useConnector(Renderer::Connector & item) {
	used_connectors.insert(item.getDrmInstance()->connector_id);
}

void Renderer::Device::startLoop(int32_t loop_timeout, int32_t loop_end) {
	/*int32_t ret;
	fd_set fds;
	time_t start, cur;
	struct timeval v;
	drmEventContext ev;

	Loader * loader = Loader::getInstance();
	FD_ZERO(&fds);
	time(&start);
	memset(&v, 0, sizeof(v));
	memset(&ev, 0, sizeof(ev));
	ev.version = DRM_EVENT_CONTEXT_VERSION;
	ev.page_flip_handler = pageFlipped;
	loop_finit = false;
	drmModePageFlip(loader->fd, old_crtc->crtc_id, buffer_vec[0]->fb,
	DRM_MODE_PAGE_FLIP_EVENT, (void *) this);
	while (!loop_finit &&( loop_end == -1 || time(&cur) < start + loop_end)) {
		FD_SET(0, &fds);
		FD_SET(loader->fd, &fds);
		v.tv_usec = loop_timeout;
		ret = select(loader->fd + 1, &fds, NULL, NULL, NULL);
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
	*/
}

void Renderer::Device::openDevice(const std::string &dev_path) {
	fd = open(dev_path.c_str(), O_RDWR | O_CLOEXEC);
	if (fd < 0) {
		throw DeviceException("cant open device at dev_path");
	}
	drmVersionPtr version = drmGetVersion(fd);
	if (!strcmp(version->name, "i915")) {
		driver_version = DriverVersion::i915;
	} else if (!strcmp(version->name, "nouveau")) {
		driver_version = DriverVersion::nouveau;
	} else if (!strcmp(version->name, "radeon")) {
		driver_version = DriverVersion::radeon;
	} else {
		driver_version = DriverVersion::unknown;
	}

	drmFreeVersion(version);
}

void Renderer::Device::initModeRes() {
	drm_mode_res = drmModeGetResources(fd);
	if (!drm_mode_res) {
		throw DeviceException("cannot retrieve DRM resources");
	}
}

void Renderer::Device::destroyBuffer(Renderer::Buffer & buf) {
	buf.destroy(fd);
}

bool Renderer::Device::isPossiblePair(Connector &conn, Crtc &crtc_o) {
	drmModeEncoderPtr enc = NULL;
	drmModeConnectorPtr conn_obj = conn.getDrmInstance();
	uint32_t crtc = crtc_o.getDrmInstance()->crtc_id;
	if(!conn_obj){
		throw DeviceException("cannot get connector");
	}
	if (conn_obj->encoder_id) {
		enc = drmModeGetEncoder(fd, conn_obj->encoder_id);
		bool result = false;
		if (!enc) {
//			loader_log() << "cannot drmModeGetEncoder";
		} else {
			result = (enc->crtc_id == crtc);
		}
		drmModeFreeEncoder(enc);
		if(result){
			return true;
		}
	}
	int32_t dev_id_crtc = -1;
	for(int32_t i = 0; i < drm_mode_res->count_crtcs; i++){
		if(drm_mode_res->crtcs[i] == crtc){
			dev_id_crtc = i;
			break;
		}
	}
	for (int32_t i = 0; i < conn_obj->count_encoders; i++) {
		bool result = false;
		enc = drmModeGetEncoder(fd, conn_obj->encoders[i]);
		if (!enc) {
//			loader_log() << "cannot retrieve encoder " << i;
		} else {
			result = (enc->possible_crtcs & (1 << dev_id_crtc));
		}
		drmModeFreeEncoder(enc);
		if(result){
			return true;
		}
	}
	return false;
}

void Renderer::Device::setCrtcBuffer(Connector &conn_o, Crtc &crtc_o, Buffer & buf) {
	drmModeConnectorPtr conn = conn_o.getDrmInstance();
	drmModeCrtcPtr crtc = crtc_o.getDrmInstance();
	if(!conn){
		throw DeviceException("cannot get connector");
	}
	int ret = drmModeSetCrtc(fd, crtc->crtc_id, buf.fb, 0, 0, &conn->connector_id, 1, &crtc->mode);
	if(ret){
		throw DeviceException("cannot set crtc");
	}
}

std::pair<Renderer::Connector, Renderer::Crtc> Renderer::Device::getPossiblePair() {
	std::vector<Connector> conn= getConnectorVec();
	std::vector<Crtc> crtc= getCrtcVec();
	for(auto &co : conn){
		for(auto &cr : crtc){
			if(isPossiblePair(co, cr))
				return std::make_pair(co, cr);
		}
	}
	throw DeviceException("no possible pairs");
}

void Renderer::Device::setCrtc(Connector& conn_o, Crtc& crtc_o) {
	drmModeConnectorPtr conn = conn_o.getDrmInstance();
	drmModeCrtcPtr crtc = crtc_o.getDrmInstance();
	if(!conn){
		throw DeviceException("cannot get connector");
	}
	int ret = drmModeSetCrtc(fd, crtc->crtc_id, crtc->buffer_id, crtc->x, crtc->y, &conn->connector_id, 1, &crtc->mode);
	if(ret){
		throw DeviceException("cannot set crtc");
	}
}

void Renderer::Device::pageFlip(Crtc& crtc, Buffer& buf, void * params) {
	int ret = drmModePageFlip(fd, crtc.getDrmInstance()->crtc_id, buf.fb,
			DRM_MODE_PAGE_FLIP_EVENT, params);
	if(ret){
		throw DeviceException("failed to page flip");
	}
}

void Renderer::Device::onPageFlipped(int32_t, uint32_t frame, uint32_t sec,
		uint32_t usec, void* data) {
	Context * context_ptr = (Context * ) data;
	context_ptr->onPageFlipped(frame, sec, usec);
}


