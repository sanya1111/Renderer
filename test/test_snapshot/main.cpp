#include "Renderer/Device.h"
#include "Renderer/Drawable.h"
#include "Renderer/Buffer.h"
#include "Renderer/Context.h"
#include "Renderer/Graphics.h"
#include "Renderer/BaseStages.h"
#include "Renderer/VertexStages.h"
#include "Renderer/PixelStages.h"
#include "Renderer/Rast.h"

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
	Model model;
	static const int nummer = 1;
	MeshModelStage model_stage[nummer];
	float count = 0;
	MyDraw() {
//		model.loadObj2("../test/test_snapshot/obj/floor/floor.obj");
		model.loadObj2("../test/test_snapshot/obj/output2.obj");
//		model.loadObj2("../test/test_obj_load/obj/reconstructed_head.obj");
//		model.loadObj2("../test/test_obj_load/obj/pone/telefone.obj");
//		model.loadObj2("../test/test_snapshot/obj/eye/eyeball_obj.obj");
//		model.loadObj2("../test/test_snapshot/obj/eyea/Realistic Eye.obj");
//		model.loadObj2("../test/test_snapshot/obj/an1/glasses2.obj");
//		model.loadObj2("../test/test_snapshot/obj/gl_pack/Glass Pack.obj");
//		model.mat_index = 4;
		FOR(i, nummer){
			model_stage[i] = move(MeshModelStage(model.meshs[i]));
		}
		white = Rgba(255, 255, 255, 0);
		black = Rgba(0, 0, 0, 0);
		count = 0;
	}
	virtual void onDraw(uint32_t frame, uint32_t sec,
			uint32_t usec, Buffer & buf){
		counter++;
		if(counter > 1){
//			return;
//			Texture saved = drawer.saveSnapshot();
//			saved.writeBmp("saved.bmp");
//			saved.writePng("saved.png");
//			saved.writeTga("saved.tga");
		}
		count += 0.025;
		CameraView cam(V3f(0, 0, -0.75 ), V3f(0, 1, 0), V3f(0, 0, 1),
					(60.0)/180.0 * 3.14, buf.width, buf.height, 0.000001f, 100); //phone
		drawer.drawBegin(&buf);
		drawer.fill2(black);
		V3f light_dir(0, 0, 1);
		DefaultVertexStage vstage1(cam, V3f(0, 0, 2.2), V3f(1 /2.0 , 1   , 1 /2.0 ), V3f(0 , 3.14  + 3.14/3 + count, 3.14/2 ), light_dir);
//		DefaultVertexStage vstage1(cam, V3f(0, 0, 2.8), V3f(1/1036.623291 , 1/381.681671  , 1/290.129395), V3f(3.14 / 2 , 3.14/2  + count, 0 ), light_dir);
//		DefaultVertexStage vstage2(cam, V3f(0, 0, 3), V3f(1/1740.142212  , 1/381.681671   , 1/290.129395), V3f(3.14 / 2 , 3.14/2 + count , 0 ), light_dir);
//		DefaultVertexStage vstage3(cam, V3f(0, 0, 3), V3f(1/(211.544144 * 2.0) , 1/(211.544144  *2.0)  , 1.0/(4 * 49.822578 )), V3f(3.14 / 2 , 3.14/2 + count , 0 ), light_dir);
//		DefaultVertexStage vstage4(cam, V3f(0, 0, 10), V3f(1 , 1  , 1 ), V3f(3.14 / 2 + count, 3.14/2 + count , 0 ), light_dir);
		DefaultRast rast(buf.height, buf.width);
		DefaultPixelStage pstage(model.mats[model.mat_index[0]].col[1][0]);
		drawer.drawModel_new(model_stage[0], vstage1, rast, pstage);
//		drawer.drawModel_new(model_stage[1], vstage2, rast, pstage);
//		drawer.drawModel_new(model_stage[2], vstage3, rast, pstage);
//		drawer.drawModel_new(model_stage[3], vstage4, rast, pstage);
		using namespace Geom;
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
