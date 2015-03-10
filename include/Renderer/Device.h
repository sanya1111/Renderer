#ifndef DEVICE_H_
#define DEVICE_H_

#include "Renderer/Loader.h"
#include "Renderer/Buffer.h"
#include "Renderer/Exception.h"
#include "Renderer/Log.h"
#include "Renderer/Drawable.h"

namespace Renderer {

class Device {
private:
	//system
	Log device_log;
	drmModeModeInfo mode;
	drmModeConnector * conn;
	drmModeCrtc * old_crtc;
	//Buffers
	std::vector<Buffer *> buffer_vec;
	size_t current_buffer;
	//DrawBuffers
	std::vector<Buffer::DrawBuffer *> draw_buffer_vec;
	//Drawable
	std::vector<std::pair<Drawable *, DrawableInfo*> > drawable_vec;
	//flags
	bool page_flip_pending;
	bool loop_finit;
	//methods
	static void pageFlipped(int fd, uint32_t frame, uint32_t sec, uint32_t usec,
			void *data);
	void switchBuffer();
public:
	class DeviceException: public Exception {
	public:
		using Exception::Exception;
	};
	Device();
	~Device();
	void addBuffer();
	Buffer::DrawBuffer * getDrawBuffer();
	void applyDrawBuffer(Buffer::DrawBuffer*);
	void startLoop(int32_t loop_timeout = 1000, int32_t loop_end = 3);
	void addDrawable(Drawable *);
};

}
#endif /* DEVICE_H_ */
