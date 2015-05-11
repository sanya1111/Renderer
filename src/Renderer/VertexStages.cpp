#include "Renderer/VertexStages.h"

using namespace Renderer;
using namespace Geom;
using namespace std;

DefaultVertexStage::result Renderer::DefaultVertexStage::process(
		std::tuple<Geom::TriangleF4, Geom::TriangleF, Geom::V3f, Geom::V3f>& tu,
		bool& ret) {
	 result res;
	 Geom::TriangleF4 &tr = std::get<0>(tu);
	 Geom::Matrix44f pt_world = Geom::Matrix44f{
		 tr[0].x, tr[0].y, tr[0].z, tr[0].w,
		 tr[1].x, tr[1].y, tr[1].z, tr[1].w,
		 tr[2].x, tr[2].y, tr[2].z, tr[2].w,
		 0,       0,       0,       0
	 } * to_world;
	 Geom::Matrix44f pt = pt_world * to_cam;
	 ret = false;
	 FOR(i, 3){
		 if(main_view->inProjection(Geom::V4f(pt[i]))){
			 ret = true;
			 break;
		 }
	 }
	 if(!ret)
		 return res;
	 Geom::V3f intensity;
	 Geom::TriangleF& tr_o = std::get<1>(tu);
	 Geom::Matrix44f pt_int = Geom::Matrix44f{
		 tr_o[0].x, tr_o[0].y, tr_o[0].z, 1,
		 tr_o[1].x, tr_o[1].y, tr_o[1].z, 1,
		 tr_o[2].x, tr_o[2].y, tr_o[2].z, 1,
		 0,       0,       0,       0
	 } * to_world;

	 FOR(i, 3) {
		 V3f pt = (V4f(pt_world[i]).norm());
		 V3f to_eye = main_view->get_center() - pt;
		 intensity[i] = light.getForce(Geom::V4f(pt_int[i]).norm().norm(), to_eye, pt);
		 intensity[i] = std::max(0.0f, intensity[i]);
		 intensity[i] = std::min(1.0f, intensity[i]);
	 }
	 Geom::TriangleF4 &f = std::get<0>(res);
	 Geom::TriangleF &s = std::get<1>(res);
	 FOR(i, 3){
		 FOR(j, 4){
			 f[i][j] = pt[i][j];
		 }
		 s[i][0] = intensity[i];
		 s[i][1] = std::get<2>(tu)[i];
		 s[i][2] = std::get<3>(tu)[i];
	 }
	 return res;
}


Geom::Matrix44f Renderer::DefaultVertexStage::getMatrix(){
	return to_world * to_cam;
}
