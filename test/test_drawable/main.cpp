#include "Renderer/Device.h"
#include "Renderer/Drawable.h"
#include "Renderer/Buffer.h"
#include "Renderer/Context.h"
#include "Renderer/Graphics.h"

using namespace Renderer;
using namespace std;
class MyDraw : public Drawable{
	public:
	int count  = 0;
		virtual void onDraw(uint32_t frame, uint32_t sec,
				uint32_t usec, Buffer & buf){
			/*count++;
			uint32_t * temp = (uint32_t *)buf.map;
			for(size_t i = 0; i < buf.height; i++)
				for(size_t j = 0; j < buf.width; j++){
					if(count & 1)
						*(temp + i * buf.width + j) = ~0;
					else
						*(temp + i * buf.width + j) = 0;
				}
			*/
			count++;
			Rgba color_a(255, 255, 255, 0);
			Rgba color_b(0, 0, 0, 0);
			if(count & 1)
				drawer.fill(color_a);
			else
				drawer.fill(color_b);
		}
};

int main(){
	Device dev("/dev/dri/card0");
	pair<Connector, Crtc> pa = dev.getPossiblePair();
	MyDraw draw;
	Context context(2, pa.first, pa.second,draw, dev);
	context.startListenning();
	dev.startLoop();
	return 0;
}
