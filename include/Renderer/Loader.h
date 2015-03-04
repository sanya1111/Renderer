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
#include <unistd.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include "Renderer/Exception.h"

namespace Renderer{

	class LoaderInfo{
	public:
	};

	class Loader{
	private:
		std::string getDevice();
		void openDevice(const std::string &path);
		void initModeRes();
	protected:
		drmModeRes res;
		drmModeConnector conn;
		int fd;
		LoaderInfo l_info;
		friend class Buffer;
	public:
		class LoaderException : public Exception{
		public:
			using Exception::Exception;
		};
		Loader(const LoaderInfo &);
	};
}
