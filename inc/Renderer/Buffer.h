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

#include "Renderer/Log.h"

namespace Renderer {


class Buffer {
	bool dumbMapping(int32_t fd);
	bool intelGemMapping(int32_t fd);
	void dumbDestroy(int32_t fd);
	void intelGemDestroy(int32_t fd);
	void destroy(int32_t fd);
	void createMapping(int32_t fd);
	enum MappingInfo {
		DUMB_MAPPING = 0x1, GEM_INTEL_MAPPING = 0x2
	} mapping_info;
	static constexpr struct format_desc_struct {
		uint32_t drm_id;
		int32_t bpp;
		int32_t depth;

		uint32_t bmask;
		uint32_t gmask;
		uint32_t rmask;
		uint32_t amask;
	}format_desc[] = {
			{DRM_FORMAT_RGB565,	16, 16, (1<<5) - 1,
										(1<<11) - (1<<5),
										(1<<16) - (1<<11),
										 0} ,
			{DRM_FORMAT_RGB888,	24, 24, (1<<8) - 1,
										(1<<16) - (1<<8),
										(1<<24) - (1<<16),
										(1ULL<<32) - (1U<<24)},
			{DRM_FORMAT_XRGB8888, 32, 24, (1<<8) - 1,
										  (1<<16) - (1<<8),
									      (1<<24) - (1<<16),
									      (1ULL<<32) - (1U<<24)},
			{DRM_FORMAT_XRGB2101010, 32, 30, 0, 0, 0, 0}, //TODO
			{DRM_FORMAT_ARGB8888, 32, 32, (1<<8) - 1,
										  (1<<16) - (1<<8),
										  (1<<24) - (1<<16),
										  (1ULL<<32) - (1U<<24)}
	};
	uint8_t format_desc_entry;
	uint32_t handle;
	uint8_t *map, *saved_map = NULL;
	uint32_t fb;
	uint32_t pixel_format;
	uint32_t stride;
	uint32_t bpp;
	uint32_t depth;
	uint32_t width;
	uint32_t height;
	uint32_t size;
	friend class Drawer;
	friend class DrawerSimulator;
	friend class Device;
	Buffer(int32_t fd, uint32_t width, uint32_t height, uint32_t format,  MappingInfo mapping_info);
public:
	uint32_t getWidth();
	uint32_t getHeight();

	class BufferException: public std::runtime_error {
	public:
		using std::runtime_error::runtime_error;
	};
	void substitution_on(uint8_t * sub_buffer);
	void substitution_off();
	Buffer() = default;

	~Buffer();
};

}

#endif /* BUFFER_H_ */
