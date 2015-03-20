#ifndef CRTC_H_
#define CRTC_H_

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <memory>
namespace Renderer{

class Crtc{
protected:
	std::shared_ptr<drmModeCrtc> crtc;
public:
	Crtc() = default;
	Crtc(int32_t fd, int32_t id);
	drmModeCrtcPtr getDrmInstance();
	size_t getUsed();
};

}


#endif /* CRTC_H_ */
