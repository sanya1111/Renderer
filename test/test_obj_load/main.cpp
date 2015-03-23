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
	float count = 0;
	Drawer drawer;
	MeshModel model;
	MyDraw() {
		count = 0;
		model.loadObj("../test/test_obj_load/obj/african_head.obj");
		count = 0;
		white = Rgba(255, 255, 255, 0);
		black = Rgba(0, 0, 0, 0);
	}
	virtual void onDraw(uint32_t frame, uint32_t sec,
			uint32_t usec, Buffer & buf){
		count += 0.08;
		CameraView cam(V3f(0, 0, count), V3f(0, 1, 0), V3f(0, 0, 1),
						(60.0)/180.0 * 3.14, buf.width, buf.height, 1, 100);
		drawer.drawBegin(&buf, cam);
		drawer.fill(white);
		drawer.drawTranslateTriangle(TriangleF(V3f(30, 30, 50 ),
				   V3f(50, -30, 50),
				   V3f(-30, 0, 50 )), black);
//		drawer.drawModel(model, V3f(0, 0, 1.7), V3f(1 + count, 1 + count, 1 + count), V3f(0, 3.14, 1.0/2.0* 3.14));
		using namespace Geom;
		drawer.drawEnd();
	}
};



int main(){
	Device dev("/dev/dri/card0");
	Connector connector;
	Crtc crtc;
	std::tie(connector, crtc) = dev.getPossiblePair();
	Context context(2, connector, crtc, unique_ptr<Drawable>(new MyDraw()), dev);
	context.startListenning();
	dev.startLoop();
//	DEB("%d\n", count);
	return 0;
}
