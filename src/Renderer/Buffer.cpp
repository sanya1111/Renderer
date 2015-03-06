#include "Renderer/Buffer.h"
#include <sys/ioctl.h>

bool Renderer::Buffer::dumbMapping() {
	Loader & loader = Loader::getInstance();
	drm_mode_create_dumb cr_dumb;
	drm_mode_map_dumb mp_dumb;
	memset(&cr_dumb, 0, sizeof(cr_dumb));
	cr_dumb.width = width;
	cr_dumb.height = height;
	cr_dumb.bpp = bpp;
	int ret = drmIoctl(loader.fd, DRM_IOCTL_MODE_CREATE_DUMB, &cr_dumb);
	if(ret < 0){
		buffer_log() << "cant create dumb buffer";
		return false;
	}
	stride = cr_dumb.pitch;
	size = cr_dumb.size;
	handle = cr_dumb.handle;
	buffer_log() << handle;
	ret = drmModeAddFB(loader.fd, width, height, depth, bpp , stride,
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
	map = (uint8_t*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
				loader.fd, mp_dumb.offset);
	if (map == MAP_FAILED) {
		throw BufferException("cannot mmap dumb buffer (MMAP)");
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
	if(buf_info.mapping_info == BufferInfo::MappingInfo::DUMB_MAPPING){
		bool status;
		try{
			status = dumbMapping();
		}catch(BufferException){
			dumbDestroy();
			throw;
		}
		if(!status){
			throw BufferException("cannot create dumb buffer");
		}
	}
	else{
		bool status;
		try{
			status = intelGemMapping();
		}catch(BufferException){
			dumbDestroy();
			throw;
		}
		if(!status){
			throw BufferException("cannot create buffer");
		}
	}
}

Renderer::Buffer::Buffer(const BufferInfo& _buf_info, uint32_t _width, uint32_t _height) : buf_info(_buf_info),
		buffer_log("buffer : ", std::cerr), width(_width), height(_height), bpp(buf_info.DEFAULT_BPP),
		depth(buf_info.DEFAULT_DEPTH){
	buffer_log() << "creating buffer...";
	createMapping();
}

bool Renderer::Buffer::intelGemMapping() {

	Loader & loader = Loader::getInstance();
	struct drm_i915_gem_create create;
	struct drm_i915_gem_mmap mmapi;
#define OBJECT_SIZE 16 * 1024
	memset(&create, 0, sizeof(create));
	create.size = OBJECT_SIZE;
	int ret = ioctl(loader.fd, DRM_IOCTL_I915_GEM_CREATE, &create);
	if(ret){
		buffer_log() << "cant create  buffer";
		return false;
	}
	handle = create.handle;
	mmapi.handle = create.handle;
	mmapi.offset = 0;
	mmapi.size = OBJECT_SIZE;
	ret = ioctl(loader.fd, DRM_IOCTL_I915_GEM_MMAP, &mmapi);
	if (ret) {
		throw BufferException("cannot map buffer (IOCTL) ");
	}
	map = (uint8_t *)(uintptr_t)mmapi.addr_ptr;

//	map = (uint8_t*)mmap(0, size, PROT_READ | P/ROT_WRITE, MAP_SHARED,
//				loader.fd, mmapi.offset);
	if (map == MAP_FAILED) {
		throw BufferException("cannot mmap dumb (MMAP)");
	}
	/*
	stride = create.pad;
	*/
	stride = 7680;
	size = mmapi.size;
	handle = mmapi.handle;
	depth = BufferInfo::DEFAULT_DEPTH;
	bpp = BufferInfo::DEFAULT_BPP;

	ret = drmModeAddFB(loader.fd, width, height, depth, bpp , stride,
			handle, &fb);
	if(ret){
		throw BufferException("cannot create framebuffer from buffer");
	}



	memset(map, 0, size);
	return true;
}

void Renderer::Buffer::intelGemDestroy() {
	Loader & loader = Loader::getInstance();
	drm_i915_gem_create ds;
	memset(&ds, 0, sizeof(ds));
//	ds_dumb.handle = handle;
//	drmIoctl(loader.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &ds_dumb);
}

Renderer::Buffer::~Buffer() {
	Loader & loader = Loader::getInstance();
	munmap(map, size);
	drmModeRmFB(loader.fd, fb);
	if(buf_info.mapping_info == BufferInfo::MappingInfo::DUMB_MAPPING){
		dumbDestroy();
	}else{
		intelGemDestroy();
	}
}

Renderer::BufferInfo::BufferInfo(MappingInfo _mapping_info) : mapping_info(_mapping_info) {
}
