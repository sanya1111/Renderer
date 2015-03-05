#ifndef BUFFER_H_
#define BUFFER_H_
#include <stdint.h>
#include "Renderer/Exception.h"
#include "Renderer/Loader.h"
#include "Renderer/Log.h"

namespace Renderer{

	class BufferInfo{
	public:
		enum MappingInfo{
			DUMB_MAPPING = 0x1,
			GEM_MAPPING = 0x2,
		}mapping_info;

		BufferInfo(MappingInfo = DUMB_MAPPING);
	};

	class Buffer{
	private:
		//return false, if cant create_dumb, throw at older states
		bool dumbMapping();
		void dumbDestroy();
	protected:
		Log buffer_log;
		uint32_t width;
		uint32_t height;
		uint32_t stride;
		uint32_t size;
		uint32_t handle;
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
		Buffer(const BufferInfo &, drmModeConnector *);
		~Buffer();
	};
}


#endif /* BUFFER_H_ */
