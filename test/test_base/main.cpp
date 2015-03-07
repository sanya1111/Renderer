#include "Renderer/Loader.h"
#include "Renderer/Device.h";

int main(){
	using namespace Renderer;
	Loader &loader = Loader::getInstance();
	Device dev;
	Buffer::DrawBuffer get = dev.getDrawBuffer();
	int r = rand() % 0xff,
	g = rand() % 0xff,
	b = rand() % 0xff;
	for(int i = 0; i < get.height; i++){
		for(int j = 0; j < get.width; j++){
				get.at(i, j).set(255, 123, 56, 0);
		}
	}
	dev.applyDrawBuffer(get);
	return 0;
}
