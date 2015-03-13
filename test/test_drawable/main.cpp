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
				uint32_t usec, Buffer &){
		}
};

int main(){
	Device dev("/dev/dri/card0");
	pair<Connector, Crtc> pa = dev.getPossiblePair();
	MyDraw draw;
	Context context(2, pa.first, pa.second,draw, dev);
	return 0;
}
