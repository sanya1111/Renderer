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

class MyDraw : public Drawable{
public:
	Rgba white, black;
	Drawer drawer;
	Model model;
	static const int nummer = 1;
	MeshModelStage model_stage[nummer];
	float count = 0;
	bool now = 0;
	float count2 = 0;
	MyDraw() {
		model.loadFile("../test/test_phong/obj/output.obj");
		for(int i = 0; i < nummer; i++){
			model_stage[i] = move(MeshModelStage(model.meshs[i]));
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
		count2 += 0.25;
		if(count > 0.8f)
			now = 1;
		if(count < -0.2f)
			now = 0;
		CameraView cam(V3f(0, 0, 0.75 ), V3f(0, 1, 0), V3f(0, 0, 1),
					(60.0)/180.0 * 3.14, buf.width, buf.height, 0.000001f, 100);
		drawer.drawBegin(&buf);
		{
			drawer.fill2(black);
			V3f light_dir(0, count, 1/3.0);
			Phong light(AmbientLight(1.0), 	DiffuseLight (light_dir, 1.0), SpecularLight(light_dir, 1.5, 4.0), 5 / 12.0, 1/12.0, 9/12.0);
			DefaultVertexStage vstage1(cam, V3f(0, 0, 1.87), V3f(1 /2.0 , 1   , 1 /2.0 ), V3f(0, 3.14   , 3.14/2 ), light);
			DefaultRast rast(buf.height, buf.width);
			DefaultPixelStage pstage(model.mats[model.mat_index[0]].getTextureVec(Material::DIFFUSE_TID)[0]);
			drawModel(model_stage[0], vstage1, rast, pstage, drawer);
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
