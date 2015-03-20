#include "Renderer/Crtc.h"

void deleteCrtc(drmModeCrtcPtr crtc)
{
	drmModeFreeCrtc(crtc);
}

Renderer::Crtc::Crtc(int32_t fd, int32_t id) : crtc(drmModeGetCrtc(fd, id),deleteCrtc){

}

drmModeCrtcPtr Renderer::Crtc::getDrmInstance() {
	return crtc.get();
}

size_t Renderer::Crtc::getUsed() {
	return crtc.use_count();
}
