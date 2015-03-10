#include "Renderer/Buffer.h"
#include <sys/ioctl.h>
#include <libdrm/intel_bufmgr.h>
#include <drm_fourcc.h>

bool Renderer::Buffer::dumbMapping() {
	Loader * loader = Loader::getInstance();
	drm_mode_create_dumb cr_dumb;
	drm_mode_map_dumb mp_dumb;
	memset(&cr_dumb, 0, sizeof(cr_dumb));
	cr_dumb.width = buf_info.width;
	cr_dumb.height = buf_info.height;
	cr_dumb.bpp = buf_info.bpp;
	int ret = drmIoctl(loader->fd, DRM_IOCTL_MODE_CREATE_DUMB, &cr_dumb);
	if (ret < 0) {
		buffer_log() << "cant create dumb buffer";
		return false;
	}
	buf_info.stride = cr_dumb.pitch;
	buf_info.size = cr_dumb.size;
	handle = cr_dumb.handle;
	ret = drmModeAddFB(loader->fd, buf_info.width, buf_info.height,
			buf_info.depth, buf_info.bpp, buf_info.stride, handle, &fb);
	if (ret) {
		throw BufferException("cannot create framebuffer from dumb buffer");
	}
	memset(&mp_dumb, 0, sizeof(mp_dumb));
	mp_dumb.handle = handle;
	ret = drmIoctl(loader->fd, DRM_IOCTL_MODE_MAP_DUMB, &mp_dumb);
	if (ret) {
		throw BufferException("cannot map dumb buffer (IOCTL) ");
	}
	map = (uint8_t*) mmap64(0, buf_info.size, PROT_READ | PROT_WRITE,
	MAP_SHARED, loader->fd, mp_dumb.offset);
	if (map == MAP_FAILED) {
		throw BufferException("cannot mmap dumb buffer (MMAP)");
	}

	memset(map, 0, buf_info.size);
	return true;
}

void Renderer::Buffer::dumbDestroy() {
	Loader * loader = Loader::getInstance();
	drm_mode_destroy_dumb ds_dumb;
	memset(&ds_dumb, 0, sizeof(ds_dumb));
	ds_dumb.handle = handle;
	drmIoctl(loader->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &ds_dumb);
}

void Renderer::Buffer::createMapping() {
	bool status = false;
	try {
		switch (buf_info.mapping_info) {
		case (BufferInfo::DUMB_MAPPING):
			status = dumbMapping();
			break;
		case (BufferInfo::GEM_INTEL_MAPPING):
			status = intelGemMapping();
		}
	} catch (BufferException) {
		switch (buf_info.mapping_info) {
		case (BufferInfo::DUMB_MAPPING):
			dumbDestroy();
			break;
		case (BufferInfo::GEM_INTEL_MAPPING):
			intelGemDestroy();
		}
		throw;
	}
	if (!status) {
		throw BufferException("cannot create buffer");
	}

}

Renderer::Buffer::Buffer(const BufferInfo& _buf_info) :
		buf_info(_buf_info), buffer_log("buffer : ", std::cerr) {
	buffer_log() << "creating buffer...";
	createMapping();
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

bool Renderer::Buffer::intelGemMapping() {

	Loader * loader = Loader::getInstance();
	struct drm_i915_gem_create create;
	struct drm_i915_gem_mmap_gtt mmap_arg;

	std::pair<int32_t, uint32_t> params = getParamsForIntelGem(buf_info.width,
			buf_info.height, buf_info.bpp, 0);
	memset(&create, 0, sizeof(create));
	create.size = buf_info.size = params.first;
	buf_info.stride = params.second;

	int ret = ioctl(loader->fd, DRM_IOCTL_I915_GEM_CREATE, &create);
	if (ret) {
		buffer_log() << "cant create  buffer";
		return false;
	}
	handle = create.handle;

	ret = drmModeAddFB(loader->fd, buf_info.width, buf_info.height,
			BufferInfo::Defaults::DEPTH, BufferInfo::BPP, buf_info.stride,
			create.handle, &fb);
	if (ret) {
		throw BufferException("cannot create framebuffer");
	}

	memset(&mmap_arg, 0, sizeof(mmap_arg));
	mmap_arg.handle = handle;
	if (drmIoctl(loader->fd, DRM_IOCTL_I915_GEM_MMAP_GTT, &mmap_arg)) {
		throw "cannot create GTT(IOCTL)";
	}
	map = (uint8_t*) mmap64(0, create.size, PROT_READ | PROT_WRITE, MAP_SHARED,
			loader->fd, mmap_arg.offset);
	if (map == MAP_FAILED) {
		throw "cannot create GTT(MMAP)";
	}
	return true;
}

void Renderer::Buffer::intelGemDestroy() {
	Loader * loader = Loader::getInstance();
	drm_i915_gem_sw_finish ds;
	memset(&ds, 0, sizeof(ds));
	ds.handle = handle;
	drmIoctl(loader->fd, DRM_I915_GEM_SW_FINISH, &ds);
}

Renderer::Buffer::DrawBuffer * Renderer::Buffer::getDrawBuffer() {
	int32_t * result = NULL;
	if (buf_info.pixel_format == DRM_FORMAT_XRGB8888
			|| buf_info.pixel_format == DRM_FORMAT_RGB888) {
		result = (int32_t *) map;
	}
	return new DrawBuffer(result, buf_info.width, buf_info.height);
}

Renderer::Buffer::~Buffer() {
	buffer_log() << "buffer removed...";
	Loader * loader = Loader::getInstance();
	munmap(map, buf_info.size);
	drmModeRmFB(loader->fd, fb);
	if (buf_info.mapping_info == BufferInfo::MappingInfo::DUMB_MAPPING) {
		dumbDestroy();
	} else if (buf_info.mapping_info
			== BufferInfo::MappingInfo::GEM_INTEL_MAPPING) {
		intelGemDestroy();
	}
}

Renderer::BufferInfo::BufferInfo(uint32_t _width, uint32_t _height,
		MappingInfo _mapping_info, uint32_t _stride, uint32_t _size,
		uint32_t _bpp, uint32_t _depth) :
		width(_width), height(_height), mapping_info(_mapping_info), stride(
				_stride), size(_size), bpp(_bpp), depth(_depth) {

	if (mapping_info == MappingInfo::DUMB_MAPPING) {
		pixel_format = BufferInfo::Defaults::DUMB_PIXEL_FORMAT;
	} else if (mapping_info == MappingInfo::GEM_INTEL_MAPPING) {
		pixel_format = BufferInfo::Defaults::INTEL_GEM_PIXEL_FORMAT;
	}
}

Renderer::Buffer::DrawBuffer::RgbPixel Renderer::Buffer::DrawBuffer::at(
		const uint32_t& x, const uint32_t& y) {
	return RgbPixel((int8_t *) (inner + x * width + y));
}

Renderer::Buffer::DrawBuffer::DrawBuffer(int32_t* _inner, int32_t _width,
		int32_t _height) :
		inner(_inner), width(_width), height(_height) {
}

void Renderer::Buffer::applyDrawBuffer(DrawBuffer* ok) {
	if (buf_info.pixel_format == DRM_FORMAT_XRGB8888
			|| buf_info.pixel_format == DRM_FORMAT_RGB888) {
		map = (uint8_t *) ok->inner;

	}
}
