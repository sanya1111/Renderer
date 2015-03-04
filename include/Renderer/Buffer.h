#ifndef BUFFER_H_
#define BUFFER_H_
#include <stdint.h>
#include "Renderer/Exception.h"
#include "Renderer/Loader.h"

namespace Renderer{

	class BufferInfo{
	public:
		enum {
			CPU_MAPPING = 0x1,
			GPU_MAPPING = 0x2,
			MIXED_MAPPING = CPU_MAPPING | GPU_MAPPING
		}mapping_info;

		BufferInfo();
	};

	class Buffer{
	protected:
		uint32_t width;
		uint32_t height;
		uint32_t strade;
		uint32_t size;
		uint32_t handle;
		uint8_t *map;
		uint32_t fb;
		BufferInfo buf_info;
	void create_mapping(Loader &from);
	public:
		class BufferException :public Exception{
		public:
			using Exception::Exception;
		};
		Buffer(Loader & from);
	};
}


#endif /* BUFFER_H_ */
