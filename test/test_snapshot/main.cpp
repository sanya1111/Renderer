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
	MeshModel model;
	ModelStage model_stage;
	float count = 0;
	MyDraw() {
//		model.loadObj2("../test/test_snapshot/obj/floor/floor.obj");
//		model.loadObj2("../test/test_obj_load/obj/output.obj");
//		model.loadObj2("../test/test_obj_load/obj/reconstructed_head.obj");
//		model.loadObj2("../test/test_obj_load/obj/pone/telefone.obj");
//		model.loadObj2("../test/test_snapshot/obj/eye/eyeball_obj.obj");
//		model.loadObj2("../test/test_snapshot/obj/eyea/Realistic Eye.obj");
//		model.loadObj2("../test/test_snapshot/obj/an1/glasses2.obj");
		model.loadObj2("../test/test_snapshot/obj/gl_pack/Glass Pack.obj");
//		model.mat_index = 4;
		DEB("OK\n");
		model_stage = std::move(ModelStage(model));
		white = Rgba(255, 255, 255, 0);
		black = Rgba(0, 0, 0, 0);
		count = 0;
	}
	virtual void onDraw(uint32_t frame, uint32_t sec,
			uint32_t usec, Buffer & buf){
		counter++;
		if(counter > 2){
//			return;
//			Texture saved = drawer.saveSnapshot();
//			saved.writeBmp("saved.bmp");
//			saved.writePng("saved.png");
//			saved.writeTga("saved.tga");
		}
		count += 0.025;
//		CameraView cam(V3f(-10, 0, -100), V3f(0, 1, 0), V3f(0, 0, 1),
//						(60.0)/180.0 * 3.14, buf.width, buf.height, 0.1, 100); //phone
//		CameraView cam(V3f(0, 0, 0), V3f(0, 1, 0), V3f(0, 0, 1),
//						(60.0)/180.0 * 3.14, buf.width, buf.height, 0.1, 100); //phone
//		DEB("%f\n", count);
		CameraView cam(V3f(0, 0, -0.75 ), V3f(0, 1, 0), V3f(0, 0, 1),
					(60.0)/180.0 * 3.14, buf.width, buf.height, 0.000001f, 100); //phone
		drawer.drawBegin(&buf);
		drawer.fill2(black);


//		DefaultVertexStage vstage(cam, V3f(0, 0, 1), V3f(1.0/2, 1.0/2 , 1.0/2 ), V3f(3.14/5.0 , 3.14, 3.14/2.0 ), V3f(-1, 1, 1 ));
//		DefaultVertexStage vstage(cam, V3f(0, 0, 1.7), V3f(1/4.0, 1 / 1.25 , 1/5.0 ), V3f(0, 3.14 , 3.14/2.0), V3f(0, 0, 1 ));
		DefaultVertexStage vstage(cam, V3f(0, 0, 3), V3f(1 , 1/2.0  , 1/3.0 ), V3f(3.14 / 2, 3.14/2 + count , 0 ), V3f(1, 0, 1 ));
		DefaultRast rast(buf.height, buf.width);
//		DefaultPixelStage pstage(model.mats[model.mat_index].col[1][0]);
		DefaultPixelStage pstage;
//		DefaultPixelStage pstage2(model.mats[2].col[1][0]);
//		DEB("%d\n", model.mat_index);
//		DEB("%d\n", model.mats[4].col[1].size());
//		DEB("TEXTURE EPT %d\n", model.mats[model.mat_index].col[1][0].height);
//		drawer.drawModel_new(model_stage, vstage, rast, pstage);
		drawer.drawModel_new(model_stage, vstage, rast, pstage);
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
