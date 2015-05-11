#include "Renderer/Device.h"
#include "Renderer/Drawable.h"
#include "Renderer/Buffer.h"
#include "Renderer/Context.h"
#include "Renderer/Graphics.h"
#include "Renderer/BaseStages.h"
#include "Renderer/VertexStages.h"
#include "Renderer/PixelStages.h"
#include "Renderer/Rast.h"
#include "Renderer/Light.h"

using namespace Renderer;
using namespace std;
using namespace Renderer::Geom;
#include <sstream>
#include <iostream>
#include <istream>
#include <fstream>
#include <memory>

//class MyVertexStage : public DefaultVertexStage{
//	DefaultPixelStage def;
//	Matrix44f trans;
//public:
//	typedef std::tuple<Geom::TriangleF4, Geom::TriangleF, Geom::V3f> result;
//	MyVertexStage(const DefaultVertexStage&def, Matrix44f trans) : def(def), trans(trans) {}
//};

class MyDraw : public Drawable{
public:
	Rgba white, black;
	Drawer drawer;
	Model model[2];
	static const int nummer = 2;
	MeshModelStage model_stage[nummer];
	float count = 0;
	bool now = 0;
	float count2 = 0;
	MyDraw() {
		model[0].loadFile("../test/test_shadow/obj/output2.obj");
		model[1].loadFile("../test/test_shadow/obj/floor/floor.obj");
		FOR(i, 2){
			model_stage[i] = move(MeshModelStage(model[i].meshs[0]));
		}
		white = Rgba(255, 255, 255, 0);
		black = Rgba(0, 0, 0, 0);
		count = 0;
	}
	virtual void onDraw(Context & context){
		Buffer &buf = context.getCurrentBuffer();
		if(context.getFrameCount() > 10000){
			context.quitProcess();
		}
		count += !now ? 0.025 : -0.025;
		count2 += 0.025;
		if(count > 0.8f)
			now = 1;
		if(count < -0.2f)
			now = 0;
		CameraView cam(V3f(-2, 0, 1.87), V3f(0, 1, 0), V3f(1, 0, 0),
					(60.0)/180.0 * 3.14, buf.getWidth(), buf.getHeight(), 0.000001f, 100);
//		CameraView cam(V3f(0, 0, 0.75 ), V3f(0, 1, 0), V3f(0, 0, 1),
//					(60.0)/180.0 * 3.14, buf.getWidth(), buf.getHeight(), 0.000001f, 100000);
		drawer.drawBegin(&buf);
		{
			drawer.fill2(black);
//			V3f light_dir(0, count, 1/3.0);
			V3f light_dir(-2, 0, 1.87);
			Phong light(AmbientLight(1.0), 	DiffuseLight (light_dir, 1.0), SpecularLight(light_dir, 1.5, 4.0), 5 / 12.0, 1/12.0, 9/12.0);
			Phong light2(AmbientLight(1.0), 	DiffuseLight (light_dir, 4.0), SpecularLight(light_dir, 1.5, 4.0),  0, 1, 0);
			DefaultVertexStage vstage[2];
			vstage[0]= DefaultVertexStage(cam, V3f(-0.3 , 0, 1.87), V3f(1 /2.0 , 1   , 1 /2.0 ), V3f(0, 3.14   , 3.14/2 ), light);
			vstage[1]= DefaultVertexStage(cam, V3f(0.5, 0, 1.87), V3f(2 , 2   , 2 ), V3f(0, 3.14   , 3.14/2 ), light2);
			DefaultRast rast(buf.getHeight(), buf.getWidth());
			FOR(i, 2){
				DefaultPixelStage pstage(model[i].mats[model[i].mat_index[0]].getTextureVec(Material::DIFFUSE_TID)[0]);
				drawModel(model_stage[i], vstage[i], rast, pstage, drawer);
			}
		}
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
	DEB("FPS : %f\n", context.getFps());
	DEB("FRAMES : %lld\n", context.getFrameCount());
	DEB("TIME : %lld\n", context.getTime());
	return 0;
}
