#include "Renderer/Device.h"
#include "Renderer/Drawable.h"
#include "Renderer/Buffer.h"
#include "Renderer/Context.h"
#include "Renderer/Graphics.h"

using namespace Renderer;
using namespace std;

/*class MyDraw : public Drawable{
	public:
	int count  = 0;
		virtual void onDraw(Buffer::DrawBuffer * get, DrawableInfo *dev){
//			std::cerr << "onDraw()!!" << std::endl;
			count ++;
			if(count == 1)
				return;
			if(count == 59900){
				dev->flag_finit = true;
			}
			Loader * loader = Loader::getInstance();
			Buffer::DrawBuffer::Rgb pr_color(0, 0, 0, 0);
			Graphics::fill(get, Buffer::DrawBuffer::Rgb(255, 255, 255, 0));
//			Graphics::draw_line(get, Geom::Line<int32_t>(Geom::Point<int32_t>(100, 100),
//					Geom::Point<int32_t>(100, 100)), pr_color);
			Graphics::draw_triangle()
		}
};
*/

int main(){
//	Device dev;
//	pair<Connector, Crtc> pa = dev.getPossiblePair();
//	Context context(2, pa.first, pa.second)
	return 0;
}
