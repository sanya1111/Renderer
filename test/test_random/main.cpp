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


int counter = 0;
class MyDraw : public Drawable{
public:
	Rgba white, black;
	Drawer drawer;
	float count = 0;
	MyDraw() {
		white = Rgba(255, 255, 255, 0);
		black = Rgba(0, 0, 0, 0);
		count = 0;
	}
	virtual void onDraw(uint32_t frame, uint32_t sec,
			uint32_t usec, Buffer & buf){
		if(!counter){
			DEB("%d %d\n", buf.width, buf.height);
		}
		counter++;
		count += 0.025;

		CameraView cam(V3f(0, 0, count * 20), V3f(0, 1, 0), V3f(0, 0, 1),
						(60.0)/180.0 * 3.14, buf.width, buf.height, 1, 100);
		drawer.drawBegin(&buf, cam);
		drawer.fill2(white);
//		for(int i = 0; i < 100; i++){
//			drawer.drawPixel(100, 400 + i, 0, black);
//		}
		drawer.drawLine(V3i(100, 400, 100), V3i(100, 500, 100), black);
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
	DEB("%d\n", counter);
	return 0;
}
