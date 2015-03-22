#include "Renderer/Device.h"
#include "Renderer/Drawable.h"
#include "Renderer/Buffer.h"
#include "Renderer/Context.h"
#include "Renderer/Graphics.h"

using namespace Renderer;
using namespace std;
using namespace Renderer::Geom;
#include <sstream>
#include <iostream>
#include <istream>
#include <fstream>
#include <memory>


class MyDraw : public Drawable{
public:
	Rgba white, black;
	int count ;
	Drawer drawer;
	MeshModel model;
	MyDraw() {

		model.loadObj("../test/test_obj_load/obj/african_head.obj");
		count = 0;
		white = Rgba(255, 255, 255, 0);
		black = Rgba(0, 0, 0, 0);
	}
	virtual void onDraw(uint32_t frame, uint32_t sec,
			uint32_t usec, Buffer & buf){
		count++;
		if(count > 1)
			return;
		CameraView cam(V3f(0, 0, 0), V3f(0, 1, 0), V3f(0, 0, 1),
						60.0/180 * 3.14, buf.width, buf.height, 1, 100);
		drawer.drawBegin(&buf, cam);
		drawer.fill(white);
		drawer.drawModel(model, V3f(0, 0, 10), V3f(500, 500, 500), V3f(0, 0, 0));
		using namespace Geom;
		drawer.drawEnd();
	}
};



int main(){
//	/*Device dev("/dev/dri/card0");
	Device dev("/dev/dri/card0");
	Connector connector;
	Crtc crtc;
	std::tie(connector, crtc) = dev.getPossiblePair();
	Context context(1, connector, crtc, unique_ptr<Drawable>(new MyDraw()), dev);
	context.startListenning();
	dev.startLoop();
//	DEB("%d\n", count);
	return 0;
}
