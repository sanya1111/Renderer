#include "Renderer/Buffer.h"
#include <sys/ioctl.h>
#include <libdrm/intel_bufmgr.h>
#include <drm_fourcc.h>

constexpr Renderer::Buffer::format_desc_struct Renderer::Buffer::format_desc[];

bool Renderer::Buffer::dumbMapping(int32_t fd) {
	drm_mode_create_dumb cr_dumb;
	drm_mode_map_dumb mp_dumb;
	memset(&cr_dumb, 0, sizeof(cr_dumb));
	cr_dumb.width = width;
	cr_dumb.height = height;
	cr_dumb.bpp = bpp;
	int ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &cr_dumb);
	if (ret < 0) {
		DEB("cannot create dumb buffer\n");
		return false;
	}

	stride = cr_dumb.pitch;
	size = cr_dumb.size;
	handle = cr_dumb.handle;
	ret = drmModeAddFB(fd, width, height,
			depth, bpp, stride, handle, &fb);
	if (ret) {
		throw BufferException("cannot create framebuffer from dumb buffer");
	}
	memset(&mp_dumb, 0, sizeof(mp_dumb));
	mp_dumb.handle = handle;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mp_dumb);
	if (ret) {
		throw BufferException("cannot map dumb buffer (IOCTL) ");
	}
	map = (uint8_t*) mmap64(0, size, PROT_READ | PROT_WRITE,
	MAP_SHARED, fd, mp_dumb.offset);
	if (map == MAP_FAILED) {
		throw BufferException("cannot mmap dumb buffer (MMAP)");
	}

	memset(map, 0, size);
	return true;
}

void Renderer::Buffer::dumbDestroy(int32_t fd) {
	drm_mode_destroy_dumb ds_dumb;
	memset(&ds_dumb, 0, sizeof(ds_dumb));
	ds_dumb.handle = handle;
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &ds_dumb);
}

void Renderer::Buffer::createMapping(int32_t fd) {
	bool status = false;
	try {
		switch (mapping_info) {
		case (DUMB_MAPPING):
			status = dumbMapping(fd);
			break;
		case (GEM_INTEL_MAPPING):
			status = intelGemMapping(fd);
		}
	} catch (const BufferException&) {
		switch (mapping_info) {
		case (DUMB_MAPPING):
			dumbDestroy(fd);
			break;
		case (GEM_INTEL_MAPPING):
			intelGemDestroy(fd);
		}
		throw;
	}
	if (!status) {
		throw BufferException("cannot create buffer");
	}

}

Renderer::Buffer::Buffer(int32_t fd, uint32_t width, uint32_t height, uint32_t format, MappingInfo mapping_info) :
	width(width), height(height), pixel_format(format), mapping_info(mapping_info){
	for(uint32_t i  = 0; i < (sizeof(format_desc) / sizeof(format_desc_struct)); i++){
		if(format == format_desc[i].drm_id){
			format_desc_entry = i;
		}
	}
	DEB("creating buffer\n");
	bpp = format_desc[format_desc_entry].bpp;
	depth = format_desc[format_desc_entry].depth;
	createMapping(fd);
}

static std::pair<int32_t, uint32_t> getParamsForIntelGem(int width, int height,
		int bpp, int tiling) {
	int size;
	int stride;

	if (tiling) {
		int v;
		v = width * bpp / 8;
		for (stride = 512; stride < v; stride *= 2)
			;
		v = stride * height;
		for (size = 1024 * 1024; size < v; size *= 2)
			;
	} else {
		stride = (width * (bpp / 8) + 63) & ~63;
		size = stride * height;
	}
	return std::make_pair(size, stride);
}

bool Renderer::Buffer::intelGemMapping(int32_t fd) {

	struct drm_i915_gem_create create;
	struct drm_i915_gem_mmap_gtt mmap_arg;

	std::pair<int32_t, uint32_t> params = getParamsForIntelGem(width, height, bpp, 0);
	memset(&create, 0, sizeof(create));
	create.size = size = params.first;
	stride = params.second;

	int ret = ioctl(fd, DRM_IOCTL_I915_GEM_CREATE, &create);
	if (ret) {
		DEB("cannot create gem buffer\n");
		return false;
	}
	handle = create.handle;

	ret = drmModeAddFB(fd, width, height,
			depth, bpp, stride,
			create.handle, &fb);
	if (ret) {
		throw BufferException("cannot create framebuffer");
	}

	memset(&mmap_arg, 0, sizeof(mmap_arg));
	mmap_arg.handle = handle;
	if (drmIoctl(fd, DRM_IOCTL_I915_GEM_MMAP_GTT, &mmap_arg)) {
		throw "cannot create GTT(IOCTL)";
	}
	map = (uint8_t*) mmap64(0, create.size, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd, mmap_arg.offset);
	if (map == MAP_FAILED) {
		throw "cannot create GTT(MMAP)";
	}
	memset(map, 0, size);
	return true;
}

void Renderer::Buffer::intelGemDestroy(int32_t fd) {
	drm_i915_gem_sw_finish ds;
	memset(&ds, 0, sizeof(ds));
	ds.handle = handle;
	drmIoctl(fd, DRM_I915_GEM_SW_FINISH, &ds);
}


void Renderer::Buffer::destroy(int32_t fd) {
	DEB("destroying buffer\n");
	munmap(map, size);
	drmModeRmFB(fd, fb);
	if (mapping_info == MappingInfo::DUMB_MAPPING) {
		dumbDestroy(fd);
	} else if (mapping_info == MappingInfo::GEM_INTEL_MAPPING) {
		intelGemDestroy(fd);
	}
}

void Renderer::Buffer::substitution_on(uint8_t* sub_buffer) {
	saved_map = map;
	map = sub_buffer;
}

void Renderer::Buffer::substitution_off() {
	map = saved_map;
}

uint32_t Renderer::Buffer::getWidth() {
	return width;
}

uint32_t Renderer::Buffer::getHeight() {
	return height;
}

Renderer::Buffer::~Buffer() {
}




