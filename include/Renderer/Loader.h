#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <vector>
#include <memory>
#include <time.h>
#include <unordered_set>
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "Renderer/Exception.h"
#include "Renderer/Log.h"

namespace Renderer{

	class LoaderInfo{
	public:
	};

	class Loader{
	private:

		int32_t next_connector_pair_ptr;
		std::string getDevice();
		std::unordered_set<int32_t> used_crtc;

		void openDevice(const std::string &path);
		void initModeRes();
		std::pair<drmModeConnector *, int32_t> getNextConnectorPair();
	protected:
		LoaderInfo l_info;
		Log loader_log;
		drmModeRes * drm_mode_res;
		int fd;

		friend class Buffer;
		friend class Device;
	public:
		class LoaderException : public Exception{
		public:
			using Exception::Exception;
		};
		Loader(const LoaderInfo &);
	};
}
