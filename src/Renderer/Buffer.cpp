#include "Renderer/Buffer.h"

bool Renderer::Buffer::dumbMapping() {
	Loader & loader = Loader::getInstance();
	drm_mode_create_dumb cr_dumb;
	drm_mode_map_dumb mp_dumb;
	memset(&cr_dumb, 0, sizeof(cr_dumb));
	cr_dumb.width = width;
	cr_dumb.height = height;
	cr_dumb.bpp = 32;
	int ret = drmIoctl(loader.fd, DRM_IOCTL_MODE_CREATE_DUMB, &cr_dumb);
	if(ret < 0){
		buffer_log() << "cant create dumb buffer";
		return false;
	}
	stride = cr_dumb.pitch;
	size = cr_dumb.size;
	handle = cr_dumb.handle;
	ret = drmModeAddFB(loader.fd, width, height, 24, 32 , stride,
			handle, &fb);
	if(ret){
		throw BufferException("cannot create framebuffer from dumb buffer");
	}

	memset(&mp_dumb, 0, sizeof(mp_dumb));
	mp_dumb.handle = handle;
	ret = drmIoctl(loader.fd, DRM_IOCTL_MODE_MAP_DUMB, &mp_dumb);
	if (ret) {
		throw BufferException("cannot map dumb buffer (IOCTL) ");
	}

	/* perform actual memory mapping */
	map = (uint8_t*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
				loader.fd, mp_dumb.offset);
	if (map == MAP_FAILED) {
		fprintf(stderr, "cannot mmap dumb buffer (MMAP)");
	}

	memset(map, 0, size);
	return true;
}

void Renderer::Buffer::dumbDestroy() {
	Loader & loader = Loader::getInstance();
	drm_mode_destroy_dumb ds_dumb;
	memset(&ds_dumb, 0, sizeof(ds_dumb));
	ds_dumb.handle = handle;
	drmIoctl(loader.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &ds_dumb);
}

void Renderer::Buffer::createMapping()
{
	buffer_log() << buf_info.mapping_info << std::endl;
	if(buf_info.mapping_info == BufferInfo::MappingInfo::DUMB_MAPPING){
		bool status = dumbMapping();
		if(!status){
			throw BufferException("cant create dumb_buffer");
		}
	}
}



Renderer::Buffer::Buffer(const BufferInfo& _buf_info, drmModeConnector* conn):buf_info(_buf_info),
		buffer_log("buffer : ", std::cerr){
	//TODO : change to CRTC
	buffer_log() << "creating buffer...";
	width = conn->modes[0].hdisplay;
	height = conn->modes[0].vdisplay;
	createMapping();
}

Renderer::Buffer::~Buffer() {
	Loader & loader = Loader::getInstance();
	munmap(map, size);
	drmModeRmFB(loader.fd, fb);
	dumbDestroy();
}

Renderer::BufferInfo::BufferInfo(MappingInfo _mapping_info) : mapping_info(_mapping_info) {
}
