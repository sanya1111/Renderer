#include "Renderer/Loader.h"
#include "Renderer/Device.h"
#include "Renderer/Drawable.h"
#include "Renderer/Buffer.h"
#include "Renderer/Graphics.h"

using namespace Renderer;

class MyDraw : public Drawable{
	public:
	int count  = 0;
		virtual void onDraw(Buffer::DrawBuffer * get, DrawableInfo *dev){
			count ++;
//			if(count == 1)
//				return;
			if(count == 59900){
				dev->flag_finit = true;
			}

			int r = rand() % 0xff,
				g = rand() % 0xff,
				b = rand() % 0xff;
			Graphics::fill(get, Buffer::DrawBuffer::Rgb(r, g, b, 0));
		}
};

int main(){
	Loader * loader = Loader::getInstance();
	Device dev;
	for(int i = 0; i < 5; i++){
	dev.addBuffer();
	dev.addBuffer();
	}
	MyDraw * dr= new MyDraw();
	dev.addDrawable(dr);
	dev.startLoop();
	std::cerr << dr->count;
	delete dr;
	return 0;
}
