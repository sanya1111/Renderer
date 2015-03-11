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
//			std::cerr << "onDraw()!!" << std::endl;
			count ++;
//			if(count == 1)
//				return;
			if(count == 59900){
				dev->flag_finit = true;
			}
			Loader * loader = Loader::getInstance();
			Buffer::DrawBuffer::Rgb pr_color(0, 0, 0, 0);
			Graphics::fill(get, Buffer::DrawBuffer::Rgb(255, 255, 255, 0));
//			Geom::Triangle<int> temp(
//								{Geom::Point<int>(100, 100),
//								 Geom::Point<int>(500, 500),
//								 Geom::Point<int>(221, 123)});
			Graphics::draw_triangle(get, Geom::Triangle<int>(
					{Geom::Point<int>(100, 100),
					 Geom::Point<int>(500, 500),
					 Geom::Point<int>(221, 123),
					 Geom::Point<int>(100, 100)}), pr_color);
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
	delete dr;
	return 0;
}
