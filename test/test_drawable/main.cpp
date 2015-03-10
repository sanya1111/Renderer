#include "Renderer/Loader.h"
#include "Renderer/Device.h"
#include "Renderer/Drawable.h"
#include "Renderer/Buffer.h"

using namespace Renderer;

class MyDraw : public Drawable{
	public:
	int count  = 0;
		virtual void onDraw(Buffer::DrawBuffer * get, DrawableInfo *dev){
//			std::cerr << "onDraw()!!" << std::endl;
			count ++;
			if(count == 59900){
				dev->flag_finit = true;
			}
			Loader * loader = Loader::getInstance();

			int r = rand() % 0xff,
				g = rand() % 0xff,
				b = rand() % 0xff;
			Buffer::DrawBuffer::RgbPixel temp;
			for(int i = 0; i < get->height; i++){
				for(int j = 0; j < get->width; j++){
						temp = get->at(i, j);
						temp.r() = r;
						temp.g() = g;
						temp.b() = b;
				}
			}
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
