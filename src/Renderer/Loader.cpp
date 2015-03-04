#include "Renderer/Loader.h"

Renderer::Loader::Loader(const LoaderInfo& _l_info) : l_info(_l_info)
{
	openDevice(getDevice());
}

std::string Renderer::Loader::getDevice(){
	//TODO
	return "/dev/dri/card0";
}

void Renderer::Loader::openDevice(const std::string &dev_path){
	fd = open(dev_path.c_str(), O_RDWR | O_CLOEXEC);
	if (fd < 0) {
		throw LoaderException("cant open");
	}


	uint64_t has_dumb;
	if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
		close(fd);
		throw LoaderException("drm device '%s' does not support dumb buffers\n");
	}
}

