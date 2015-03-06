#include "Renderer/Buffer.h"
#include <sys/ioctl.h>
#include <libdrm/intel_bufmgr.h>
#include <drm_fourcc.h>

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

void Renderer::Buffer::createMapping() {
	bool status;
	try{
		switch(buf_info.mapping_info){
			case(BufferInfo::DUMB_MAPPING):
				status = dumbMapping();
				break;
			case(BufferInfo::GEM_INTEL_MAPPING):
				status = intelGemMapping();
		}
	}catch(BufferException){
		switch(buf_info.mapping_info){
			case(BufferInfo::DUMB_MAPPING):
				dumbDestroy();
				break;
			case(BufferInfo::GEM_INTEL_MAPPING):
				intelGemDestroy();
		}
		throw;
	}
	if(!status){
		throw BufferException("cannot create buffer");
	}

}

Renderer::Buffer::Buffer(const BufferInfo& _buf_info, uint32_t _width, uint32_t _height) : buf_info(_buf_info),
		buffer_log("buffer : ", std::cerr), width(_width), height(_height), bpp(buf_info.DEFAULT_BPP),
		depth(buf_info.DEFAULT_DEPTH){
	buffer_log() << "creating buffer...";
	createMapping();
}

static std::pair<int32_t, uint32_t> getParamsForIntelGem(int width, int height, int bpp,
			 int tiling){
	int size;
	unsigned stride;

	if (tiling) {
		int v;
		v = width * bpp / 8;
		for (stride = 512; stride < v; stride *= 2)
			;
		v = stride * height;
		for (size = 1024*1024; size < v; size *= 2)
			;
	} else {
		stride = (width * (bpp / 8) + 63) & ~63;
		size = stride * height;
	}
	return std::make_pair(size, stride);
}

bool Renderer::Buffer::intelGemMapping() {

	Loader & loader = Loader::getInstance();
	struct drm_i915_gem_create create;
	struct drm_i915_gem_mmap mmapi;
	bpp = BufferInfo::DEFAULT_BPP;
	depth = BufferInfo::DEFAULT_DEPTH;

	std::pair<int32_t, uint32_t> params = getParamsForIntelGem(width, height, bpp, 0);
	memset(&create, 0, sizeof(create));
	create.size = size = params.first;
	stride = params.second;

	int ret = ioctl(loader.fd, DRM_IOCTL_I915_GEM_CREATE, &create);
	if(ret){
		buffer_log() << "cant create  buffer";
		return false;
	}
	handle = create.handle;
	uint32_t handles[4];
	uint32_t pitches[4];
	uint32_t offsets[4];
	uint32_t ff[4];

	memset(handles, 0, sizeof(handles));
	memset(pitches, 0, sizeof(pitches));
	memset(offsets, 0, sizeof(offsets));
	memset(ff, 0, sizeof ff);
	ff[0] = width;
	handles[0] = create.handle;
	pitches[0] = stride;
	offsets[0] = 0;
	uint32_t format = DRM_FORMAT_RGB565;
	ret = drmModeAddFB2(loader.fd, width, height,  format,
					handles, pitches, offsets,
					&fb, 0);
	if(ret){
		throw BufferException("cannot create framebuffer");
	}

	memset(&mmapi, 0, sizeof(mmapi));
	mmapi.handle = create.handle;
	mmapi.offset = 0;
	mmapi.size = create.size;
	ret = ioctl(loader.fd, DRM_IOCTL_I915_GEM_MMAP, &mmapi);
	if (ret) {
		throw BufferException("cannot map buffer (IOCTL) ");
	}
	map = (uint8_t *)(uintptr_t)mmapi.addr_ptr;

	return true;
}

void Renderer::Buffer::intelGemDestroy() {
//	Loader & loader = Loader::getInstance();
//	drm_i915_gem_ ds;
//	memset(&ds, 0, sizeof(ds));
//	ds.handle = handle;
//	DRM_i915_
//	drmIoctl(loader.fd, DRM_I915_GEM_SW_FINISH, &ds);
}

Renderer::Buffer::~Buffer() {
	Loader & loader = Loader::getInstance();
	if(buf_info.mapping_info == BufferInfo::MappingInfo::DUMB_MAPPING){
		munmap(map, size);
		drmModeRmFB(loader.fd, fb);
		dumbDestroy();
	}else{
		drmModeRmFB(loader.fd, fb);
		intelGemDestroy();
	}
}

Renderer::BufferInfo::BufferInfo(MappingInfo _mapping_info) : mapping_info(_mapping_info) {
}
