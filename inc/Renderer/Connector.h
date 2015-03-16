#ifndef CONNECTOR_H_
#define CONNECTOR_H_
#include <xf86drm.h>
#include <memory>
#include <xf86drmMode.h>

namespace Renderer{

class Connector{
protected:
	std::shared_ptr<drmModeConnector> conn;
public:
	Connector() = default;
	Connector(int32_t fd, int32_t id) ;
	drmModeConnectorPtr getDrmInstance();
};

}



#endif /* CONNECTOR_H_ */
