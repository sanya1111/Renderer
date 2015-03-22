#include "Renderer/Device.h"
#include "Renderer/Drawable.h"
#include "Renderer/Buffer.h"
#include "Renderer/Context.h"
#include "Renderer/Graphics.h"

#include <iostream>

//using namespace Renderer;
//using namespace std;
//int count = 0;
//class MyDraw : public Drawable{
//	public:
//	Rgba white, black;
//	Drawer drawer;
//	Geom::Triangle dr;
//	MyDraw() : dr(
//			Geom::V3i(700, 1024, 0),
//			Geom::V3i(727 , 1431, 0),
//			Geom::V3i(727, 432, 0)){
//		count = 0;
//		white = Rgba(255, 255, 255, 0);
//		black = Rgba(0, 0, 0, 0);
//	}
//	virtual void onDraw(uint32_t frame, uint32_t sec,
//			uint32_t usec, Buffer & buf){
//		using namespace Geom;
//		count++;
//		if(count > 2)
//			return;
//		drawer.drawBegin(&buf);
//		drawer.fill(white);
//		drawer.drawTriangle(dr, black);
//		if(count == 1){
//			for(int i = 0; i < 3; i++){
//				dr.vs[i].y += 300;
//			}
//		}
//		drawer.drawFilledTriangle(dr, black);
//		drawer.drawEnd();
//	}
//};

int main(){
//	srand(clock() ^ rand() * rand());
//	Device dev("/dev/dri/card0");
//	Connector connector;
//	Crtc crtc;
//	std::tie(connector, crtc) = dev.getPossiblePair();
//	Context context(2, connector, crtc, unique_ptr<Drawable>(new MyDraw()), dev);
//	context.startListenning();
//	dev.startLoop();
//	DEB("%d\n", count);
	return 0;
}
