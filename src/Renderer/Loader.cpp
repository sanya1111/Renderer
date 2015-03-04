#include "Renderer/Loader.h"



Renderer::Loader::Loader(const LoaderInfo& _l_info) : l_info(_l_info),
next_connector_pair_ptr(0), loader_log("loader: ", std::cerr)
{
	openDevice(getDevice());
	initModeRes();
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
		close(fd);
		throw LoaderException("drm device does not support dumb buffers");
	}
}

void Renderer::Loader::initModeRes() {
	drm_mode_res = drmModeGetResources(fd);
	if(!drm_mode_res){
		throw LoaderException("cannot retrieve DRM resources");
	}
}

std::pair<drmModeConnector *, int32_t> Renderer::Loader::getNextConnectorPair() {
	drmModeConnector * conn = NULL;
	int32_t crtc;
	while(next_connector_pair_ptr != drm_mode_res->count_connectors){
		drmModeConnector * conn = drmModeGetConnector(fd, drm_mode_res->connectors[next_connector_pair_ptr++]);
		if(!conn){
			loader_log() << "cannot retrieve DRM connector " << next_connector_pair_ptr ;
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
			crtc = enc->crtc_id;
			if(!used_crtc.count(crtc)){
				drmModeFreeEncoder(enc);
				used_crtc.insert(crtc);
				return std::make_pair(conn, crtc);
			}
		}
	}


	for(int32_t i = 0; i < conn->count_encoders; i++){
		enc = drmModeGetEncoder(fd, conn->encoders[i]);
		if(!enc){
			loader_log() << "cannot retrieve encoder " << i;
		}
		for(int32_t j = 0; j < drm_mode_res->count_crtcs; j++){
			if(!(enc->possible_crtcs & (1<<j)))
				continue;
			crtc = drm_mode_res->crtcs[j];
			if(!used_crtc.count(crtc)){
				drmModeFreeEncoder(enc);
				used_crtc.insert(crtc);
				return std::make_pair(conn, crtc);
			}
		}
	}
	throw LoaderException("cant create new connector pair");
}

