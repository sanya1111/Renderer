#include "Renderer/Loader.h"



Renderer::Loader::Loader(const LoaderInfo * _l_info) :
next_connector_pair_ptr(0), loader_log("loader: ", std::cerr)
{
	l_info = !_l_info ? LoaderInfo() : *_l_info;
	loader_log() << "Loading...";
	openDevice(getDevice());
	initModeRes();
	checkDriver();
}

std::string Renderer::Loader::getDevice(){
	//TODO
	return "/dev/dri/card0";
}

void Renderer::Loader::openDevice(const std::string &dev_path){
	fd = open(dev_path.c_str(), O_RDWR | O_CLOEXEC);
	if (fd < 0) {
		throw LoaderException("cant open device at dev_path");
	}


	uint64_t has_dumb;
	if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
		loader_log() << "device doesnt support dumb buffers";
		l_info.buf_info = LoaderInfo::GEM_BUFFERS;
	}
}

void Renderer::Loader::initModeRes() {
	drm_mode_res = drmModeGetResources(fd);
	if(!drm_mode_res){
		throw LoaderException("cannot retrieve DRM resources");
	}
}

std::pair<drmModeConnector *, drmModeCrtc * > Renderer::Loader::getNextConnectorPair() {
	drmModeConnector * conn = NULL;
	int32_t crtc_id;
	while(next_connector_pair_ptr != drm_mode_res->count_connectors){
		conn = drmModeGetConnector(fd, drm_mode_res->connectors[next_connector_pair_ptr++]);
		if(!conn || conn->connection != DRM_MODE_CONNECTED || !conn->modes){
			loader_log() << "connector passed " << conn->connector_id ;
		}else{
			break;
		}
	}

	if(!conn && next_connector_pair_ptr == drm_mode_res->count_connectors)
			throw LoaderException("all connectors done");
	drmModeEncoder * enc = NULL;
	if(conn->encoder_id){
		enc = drmModeGetEncoder(fd, conn->encoder_id);
		if(!enc){
			loader_log() << "cannot drmModeGetEncoder";
		}else{
			crtc_id = enc->crtc_id;
			if(!used_crtc.count(crtc_id)){

				goto return_result;
			}
		}
		drmModeFreeEncoder(enc);
	}
	for(int32_t i = 0; i < conn->count_encoders; i++){
		enc = drmModeGetEncoder(fd, conn->encoders[i]);
		if(!enc){
			loader_log() << "cannot retrieve encoder " << i;
		}
		for(int32_t j = 0; j < drm_mode_res->count_crtcs; j++){
			if(!(enc->possible_crtcs & (1<<j)))
				continue;
			crtc_id = drm_mode_res->crtcs[j];
			if(!used_crtc.count(crtc_id)){
				goto return_result;
			}
		}
		drmModeFreeEncoder(enc);
	}
	throw LoaderException("cant create new connector pair");
return_result:
	drmModeFreeEncoder(enc);
	drmModeCrtc * crtc = drmModeGetCrtc(fd, crtc_id);
	if(!crtc){
		throw LoaderException("cant get crtc");
	}
	used_crtc.insert(crtc_id);
	return std::make_pair(conn, crtc);
}

Renderer::Loader& Renderer::Loader::getInstance(const LoaderInfo* params, bool force_restart) {
	static Loader sing(params);
	if(force_restart)
		sing = Loader(params);
	return sing;
}

void Renderer::Loader::checkDriver() {
	/*drm_i915_gem_create cr1;
	if(!drmIoctl(fd, DRM_IOCTL_I915_GEM_CREATE, &cr1) ){
		loader_log() << "intel" << cr1.handle << " " << cr1.pad << cr1.size;
		return;
	}
	drm_nouveau_gem_new cr2;
	if(!drmIoctl(fd, DRM_NOUVEAU_GEM_NEW, &cr2) ){
			loader_log() << "nvidia";
			return;
	}
	drm_radeon_gem_create cr3;
	if(!drmIoctl(fd, DRM_IOCTL_RADEON_GEM_CREATE, &cr3) ){
				loader_log() << "adeion";
	}
	*/
}

Renderer::Loader::~Loader() {
}

Renderer::LoaderInfo::LoaderInfo() : buf_info(BOTH_BUFFERS){
}
