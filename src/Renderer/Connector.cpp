#include "Renderer/Connector.h"

void deleteConnector(drmModeConnectorPtr conn){
	drmModeFreeConnector(conn);
}

Renderer::Connector::Connector(int32_t fd, int32_t id) : conn(drmModeGetConnector(fd, id), deleteConnector) {
}

drmModeConnectorPtr Renderer::Connector::getDrmInstance() {
	return conn.get();
}

size_t Renderer::Connector::getUsed() {
	return conn.use_count();
}
