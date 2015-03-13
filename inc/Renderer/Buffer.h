#ifndef BUFFER_H_
#define BUFFER_H_

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <stdexcept>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <stdint.h>
#include <exception>
#include <libdrm/i915_drm.h>
#include <drm_fourcc.h>
#define DEB(...) fprintf(stderr, __VA_ARGS__)

namespace Renderer {


class Buffer {
protected:
	//methods
	bool dumbMapping(int32_t fd);
	bool intelGemMapping(int32_t fd);
	void dumbDestroy(int32_t fd);
	void intelGemDestroy(int32_t fd);
	void destroy(int32_t fd);
	void createMapping(int32_t fd);
	//enums and contsants
	enum MappingInfo {
		DUMB_MAPPING = 0x1, GEM_INTEL_MAPPING = 0x2
	} mapping_info;
	static constexpr struct format_desc_struct {
		uint32_t drm_id;
		int32_t bpp;
		int32_t depth;
	}format_desc[] = {
			{DRM_FORMAT_RGB565,	16, 16} ,
			{DRM_FORMAT_RGB888,	24, 24},
			{DRM_FORMAT_XRGB8888, 32, 24},
			{DRM_FORMAT_XRGB2101010, 32, 30},
			{DRM_FORMAT_ARGB8888, 32, 32}
	};
	//var
	uint32_t handle;
	uint8_t *map;
	uint32_t fb;
	uint32_t pixel_format;
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint32_t size;
	uint32_t bpp;
	uint32_t depth;
	friend class Device;
	//constructor
	Buffer(int32_t fd, uint32_t width, uint32_t height, uint32_t format,  MappingInfo mapping_info);
public:
	class BufferException: public std::runtime_error {
	public:
		using std::runtime_error::runtime_error;
	};

	Buffer() = default;
	~Buffer();
};

}

#endif /* BUFFER_H_ */
