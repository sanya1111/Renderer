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

class MyPixelStage : public DefaultPixelStage{
	Matrix44<double> trans;
	int height, width;
	vector<float> z;
	bool flag = false;
public:
	MyPixelStage(Texture &with, const Matrix44<double> &trans, vector<float>&z, int height, int width )
		: DefaultPixelStage(with), trans(trans),
		  z(z), height(height), width(width){
	}
	bool apply(Geom::V2<int> &pt){
		bool ret = DefaultPixelStage::apply(pt);
		flag = true;
		if(ret){
			V2<float> inner = DefaultPixelStage::getXY();
			V4<double> need = V4<double>((V4<double>(inner.x, inner.y, DefaultPixelStage::getZ(), DefaultPixelStage::getW()).rowMatrix() * trans)[0]);
			int sx = need.x / need.w, sy = need.y/need.w;
			flag = false;
			if(sx >= 0 && sy >= 0 && sx < height && sy < width){
				flag = (z[sx * width + sy] - need.z >=0);
			}
		}
		return ret;
	}
	Rgba getColor(){
		if(!flag)
			return Rgba(0, 0, 0, 0);
		Rgba ret = DefaultPixelStage::getColor() ;
		return ret;
	}
};

class MyDraw : public Drawable{
public:
	Rgba white, black;
	Drawer drawer;
	DrawerSimulator drawer_sim;
	Model model[2];
	static const int nummer = 2;
	MeshModelStage model_stage[nummer];
	MyDraw() {
		model[0].loadFile("../test/test_shadow/obj/output2.obj");
		model[1].loadFile("../test/test_shadow/obj/floor/floor.obj");
		FOR(i, 2){
			model_stage[i] = move(MeshModelStage(model[i].meshs[0]));
		}
		white = Rgba(255, 255, 255, 0);
		black = Rgba(0, 0, 0, 0);
	}
	virtual void onDraw(Context & context){
		Buffer &buf = context.getCurrentBuffer();
		if(context.getFrameCount() > 10000){
			context.quitProcess();
		}
		vector<float> zbuf;

		V3f light_dir(-2, 0, 1.87);
		Phong light(AmbientLight(1.0), 	DiffuseLight (light_dir, 1.0), SpecularLight(light_dir, 1.5, 4.0), 5 / 12.0, 1/12.0, 9/12.0);
		Phong light2(AmbientLight(1.0), 	DiffuseLight (light_dir, 4.0), SpecularLight(light_dir, 1.5, 4.0),  0, 1, 0);
		Matrix44<double> trans[2];
		{
			drawer_sim.drawBegin(&buf);
			drawer_sim.fill2(black);
			DefaultVertexStage vstage[2];
			CameraView cam(light_dir, V3f(0, 1, 0), V3f(1, 0, 0),
											(60.0)/180.0 * 3.14, buf.getWidth(), buf.getHeight(), 0.000001f, 100000);
			vstage[0]= DefaultVertexStage(cam, V3f(-0.3 , 0, 1.87), V3f(1 /2.0 , 1   , 1 /2.0 ), V3f(0, 3.14   , 3.14/2 ), light);
			vstage[1]= DefaultVertexStage(cam, V3f(0.5, 0, 1.87), V3f(2 , 2   , 2 ), V3f(0, 3.14   , 3.14/2 ), light2);
			DefaultRast rast(buf.getHeight(), buf.getWidth());
			FOR(i, 2){
				DefaultPixelStage pstage(model[i].mats[model[i].mat_index[0]].getTextureVec(Material::DIFFUSE_TID)[0]);
				drawModel(model_stage[i], vstage[i], rast, pstage, drawer_sim);
			}
			if(context.getFrameCount() == 1){
//				Texture tex= drawer_sim.saveSnapshot();
//				tex.writePng("okey.png");
			}
			drawer_sim.drawEnd();
			zbuf = drawer_sim.getZbuffer();
			FOR(i, 2)
				trans[i] = (Matrix44<double>(vstage[i].getMatrix()) * Matrix44<double>(rast.getMatrix()));
		}

		{
			drawer.drawBegin(&buf);
			CameraView cam(light_dir, V3f(0, 1, 0), V3f(1, 0, 0),
														(60.0)/180.0 * 3.14, buf.getWidth(), buf.getHeight(), 0.000001f, 100000);
//			CameraView cam(V3f(-1, 0, -0.4), V3f(0, 1, 0), V3f(0, 0, 1),
//														(60.0)/180.0 * 3.14, buf.getWidth(), buf.getHeight(), 0.000001f, 100000);
			DefaultVertexStage vstage[2];
			vstage[0]= DefaultVertexStage(cam, V3f(-0.3 , 0, 1.87), V3f(1 /2.0 , 1   , 1 /2.0 ), V3f(0, 3.14   , 3.14/2 ), light);
			vstage[1]= DefaultVertexStage(cam, V3f(0.5, 0, 1.87), V3f(1 , 1   , 1 ), V3f(0, 3.14   , 3.14/2 ), light2);
			DefaultRast rast(buf.getHeight(), buf.getWidth());
			FOR(i, 2){
				MyPixelStage pstage(model[i].mats[model[i].mat_index[0]].getTextureVec(Material::DIFFUSE_TID)[0],
						Matrix44<double>(Matrix44<double>(vstage[i].getMatrix()) * Matrix44<double>(rast.getMatrix())).invert() * Matrix44<double>(trans[i]),
						zbuf,
						buf.getHeight(),
						buf.getWidth());
				drawModel(model_stage[i], vstage[i], rast, pstage, drawer);
			}
			drawer.drawEnd();
		}

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
