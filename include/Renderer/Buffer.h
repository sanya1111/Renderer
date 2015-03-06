#ifndef BUFFER_H_
#define BUFFER_H_
#include <stdint.h>
#include "Renderer/Exception.h"
#include "Renderer/Loader.h"
#include "Renderer/Log.h"
#include <drm_fourcc.h>

namespace Renderer{

	class BufferInfo{
	public:
		enum MappingInfo{
			DUMB_MAPPING = 0x1,
			GEM_INTEL_MAPPING = 0x2,
		}mapping_info;
		static const int DEFAULT_DEPTH = 24;
		static const int DEFAULT_BPP = 32;
		static const int DEFAULT_PIX_FORMAT = DRM_FORMAT_RGB565;
//		BufferInfo(MappingInfo = GEM_INTEL_MAPPING);
		BufferInfo(MappingInfo = DUMB_MAPPING);
	};

	class Buffer{
	private:
		//return false, if cant create_dumb, throw at older states
		bool dumbMapping();
		bool intelGemMapping();
		//
		void dumbDestroy();
		void intelGemDestroy();
	protected:
		Log buffer_log;
		uint32_t width;
		uint32_t height;
		uint32_t stride;
		uint32_t size;
		uint32_t handle;
		uint32_t bpp;
		uint32_t depth;
		uint8_t *map;
		uint32_t fb;
		BufferInfo buf_info;
		friend class Device;
	void createMapping();
	public:
		class BufferException :public Exception{
		public:
			using Exception::Exception;
		};
		Buffer(const BufferInfo &, uint32_t width, uint32_t height);
		~Buffer();
	};
}


#endif /* BUFFER_H_ */
