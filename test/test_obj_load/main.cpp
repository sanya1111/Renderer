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
	MyDraw(){
		count = 0;
		white = Rgba(255, 255, 255, 0);
		black = Rgba(0, 0, 0, 0);
	}
	virtual void onDraw(uint32_t frame, uint32_t sec,
			uint32_t usec, Buffer & buf){
		count++;
		if(count > 2)
			return;
		drawer.setBuffer(&buf);
		drawer.fill(white);
		using namespace Geom;
//		drawer.drawMash("../test/test_obj_load/obj/Wraith Raider Starship.obj", black);
		drawer.drawMash("../test/test_obj_load/obj/african_head.obj", black);

	}
};



int main(){
//	/*Device dev("/dev/dri/card0");
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
