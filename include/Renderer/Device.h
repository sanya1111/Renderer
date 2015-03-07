#ifndef DEVICE_H_
#define DEVICE_H_

#include "Renderer/Loader.h"
#include "Renderer/Buffer.h"
#include "Renderer/Exception.h"
#include "Renderer/Log.h"

namespace Renderer{
	class Device{
	private:
		Log device_log;
		drmModeModeInfo mode;
		drmModeConnector * conn;
		drmModeCrtc * old_crtc;
		std::vector<Buffer * > buffer_vec;
		size_t current_buffer;
	public:
		class DeviceException:public Exception{
		public:
			using Exception::Exception;
		};
		Device();
		~Device();
		void addBuffer();
		Buffer::DrawBuffer getDrawBuffer();
		void applyDrawBuffer(Buffer::DrawBuffer&);
		void switchBuffer();
	};
}
#endif /* DEVICE_H_ */
