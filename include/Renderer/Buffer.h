#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>
#include "Renderer/Exception.h"
#include "Renderer/Loader.h"
#include "Renderer/Log.h"
#include <drm_fourcc.h>

namespace Renderer {

class BufferInfo {
public:
	enum MappingInfo {
		DUMB_MAPPING = 0x1, GEM_INTEL_MAPPING = 0x2
	} mapping_info;

	enum Defaults {
		DEPTH = 24,
		BPP = 32,
		INTEL_GEM_PIXEL_FORMAT = DRM_FORMAT_XRGB8888,
		DUMB_PIXEL_FORMAT = DRM_FORMAT_RGB888
	};
	uint32_t pixel_format;
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	uint32_t size;
	uint32_t bpp;
	uint32_t depth;
	BufferInfo(uint32_t _width = 0, uint32_t _height = 0, MappingInfo =
			GEM_INTEL_MAPPING, uint32_t _stride = 0, uint32_t _size = 0,
			uint32_t _bpp = Defaults::BPP, uint32_t depth = Defaults::DEPTH);
};

class Buffer {
private:
	//return false, if cant create_dumb, throw at older states
	bool dumbMapping();bool intelGemMapping();
	//
	void dumbDestroy();
	void intelGemDestroy();
protected:
	Log buffer_log;
	uint32_t handle;
	uint8_t *map;
	uint32_t fb;
	BufferInfo buf_info;
	friend class Device;
	void createMapping();
public:
	class BufferException: public Exception {
	public:
		using Exception::Exception;
	};

	class DrawBuffer {
	protected:
		int32_t * inner;
		friend class Buffer;

	public:
		class RgbPixel {
			int8_t * ptr;
		public:
			RgbPixel() = default;
			RgbPixel(int8_t * _ptr) :
					ptr(_ptr) {
			}
			int8_t & r() {
				return ptr[2];
			}
			int8_t & g() {
				return ptr[1];
			}
			int8_t &b() {
				return ptr[0];
			}
			int8_t &a() {
				return ptr[3];
			}
		};
		int32_t width;
		int32_t height;
		RgbPixel at(const uint32_t& x, const uint32_t &y);
		DrawBuffer(int32_t *_inner, int32_t _width, int32_t _height);
	};
	DrawBuffer * getDrawBuffer();
	void applyDrawBuffer(DrawBuffer *);
	Buffer(const BufferInfo &);
	~Buffer();
};

}

#endif /* BUFFER_H_ */
