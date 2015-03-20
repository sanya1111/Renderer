#ifndef DEVICE2_H_
#define DEVICE2_H_

#include "Renderer/Buffer.h"
#include "Renderer/Connector.h"
#include "Renderer/Log.h"
#include "Renderer/Crtc.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unordered_set>

namespace Renderer{

class Device{
	//flags
	bool loop_finit;
	int8_t page_flip_pending;
	//
	drmModeRes * drm_mode_res;
	int32_t fd;
	std::unordered_set<uint32_t> used_connectors;
	std::unordered_set<uint32_t> used_crts;
	//enums
	enum DriverVersion {
			i915, radeon, nouveau, unknown
	} driver_version;
	//methods
	void initModeRes();
	void openDevice(const std::string &dev_path);
	static void onPageFlipped(int32_t fd, uint32_t frame, uint32_t sec,
			uint32_t usec, void *data);
public:
	class DeviceException: public std::runtime_error {
	public:
		using std::runtime_error::runtime_error;
	};
	Device(const std::string &);
	~Device();
	std::vector<Connector> getConnectorVec();
	std::vector<Crtc> getCrtcVec();
	void useCrtc(Crtc &item);
	void useConnector(Connector &item);
	Renderer::Buffer createBuffer(Crtc &crtc);
	void destroyBuffer(Renderer::Buffer &);
	bool isCompatible(Connector& conn, Crtc &crtc);
	void startLoop(int32_t loop_timeout = 1000, int32_t loop_end = -1);
	void setCrtcBuffer(Connector &conn, Crtc &crtc, Buffer &buf);
	void setCrtc(Connector &conn, Crtc &crtc);
	void pageFlip(Crtc &crtc, Buffer &buf, void * params, bool is_init = false);
	void finishLoop();
	std::pair<Connector, Crtc> getPossiblePair();
};

}



#endif /* DEVICE2_H_ */
