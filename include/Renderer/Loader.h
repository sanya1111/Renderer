#ifndef LOADER_H_
#define LOADER_H_
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

#include <libdrm/i915_drm.h>
//struct drm_nouveau_grobj_alloc only declare here
#include <Wrapper/nouveau_drm.h>
#include <libdrm/radeon_drm.h>

namespace Renderer{

	class LoaderInfo{
	public:
		enum {
			DUMB_BUFFERS = 0x1,
			GEM_BUFFERS = 0x2,
			BOTH_BUFFERS = DUMB_BUFFERS | GEM_BUFFERS
		}buf_info;
		LoaderInfo();
	};

	class Loader{
	private:
		int32_t next_connector_pair_ptr;
		std::unordered_set<int32_t> used_crtc;
		std::string getDevice();
		void openDevice(const std::string &path);
		void checkDriver();
		void initModeRes();
		std::pair<drmModeConnector *, drmModeCrtc * > getNextConnectorPair();
		//for singleton
		Loader(const LoaderInfo *);
		Loader () {}
		Loader (const Loader &);
		Loader & operator=(const Loader &) {}
	protected:
		LoaderInfo  l_info;
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
		//if not start yet - init new Loader;
		static Loader & getInstance(const LoaderInfo * params = NULL, bool force_restart = false);
		~Loader();
	};
}

#endif
