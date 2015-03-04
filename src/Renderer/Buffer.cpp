#include "Renderer/Buffer.h"

/*
void Renderer::Buffer::create_mapping(Loader &from)
{
	struct drm_mode_create_dumb dumber;
	if(buf_info.mapping_info == BufferInfo::GPU_MAPPING){
		int ret = drmIoctl(from.fd, DRM_IOCTL_MODE_CREATE_DUMB, &dumber);
		if(ret < 0)
			throw BufferException("cant create dumb buffer");
	}
}

Renderer::Buffer::Buffer(Loader& from)
{
	height = from.conn.modes[0].vdisplay;
	width = from.conn.modes[0].hdisplay;
}

Renderer::BufferInfo::BufferInfo() : mapping_info(GPU_MAPPING)
{

}

*/
