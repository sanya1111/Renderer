#include "Renderer/Device.h"
#include "Renderer/Drawable.h"
#include "Renderer/Buffer.h"
#include "Renderer/Context.h"
#include "Renderer/Graphics.h"

#include <iostream>

using namespace Renderer;
using namespace std;
int count = 0;
class MyDraw : public Drawable{
	public:
	Rgba white, black;
	Drawer drawer;
	MyDraw(){
		count = 0;
		white = Rgba(255, 255, 255, 0);
		black = Rgba(0, 0, 0, 0);
	}
	virtual void onDraw(uint32_t frame, uint32_t sec,
			uint32_t usec, Buffer & buf){
		count++;
//		if(count > 2)
//			return;
//		drawer.setBuffer(&buf);
//		drawer.fill(white);
//		using namespace Geom;
//		drawer.drawTriangle(Triangle2D<int32_t>(
//		Point2D<int32_t>(78, 120),
//		Point2D<int32_t>(771, 310),
//		Point2D<int32_t>(800, 60)), black);

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
	DEB("%d\n", count);
	return 0;
}
