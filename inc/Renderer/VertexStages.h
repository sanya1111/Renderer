#ifndef VERTEXSTAGES_H_
#define VERTEXSTAGES_H_

#include "Renderer/Camera.h"
#include "Renderer/Geom.hpp"
#include "Renderer/Utils.hpp"

namespace Renderer{

class DefaultVertexStage{
	Geom::Matrix44f transform_matrix;
	Geom::V3f light_dir;
	CameraView * main_view;
public:
	typedef std::tuple<Geom::TriangleF4, Geom::TriangleF> result;
	DefaultVertexStage(CameraView &main_view, Geom::V3f position, Geom::V3f scale, Geom::V3f rot, Geom::V3f light_dir) : main_view(&main_view),light_dir(light_dir){
		transform_matrix = Geom::MatrixFactory::transform(position, rot, scale) * main_view.projection_matrix();
	}
	result process(std::tuple<Geom::TriangleF4, Geom::TriangleF, Geom::V3f, Geom::V3f> &tu, bool &ret){
		 result res;
		 Geom::TriangleF4 &tr = std::get<0>(tu);
		 Geom::Matrix44f pt = Geom::Matrix44f{
			 tr[0].x, tr[0].y, tr[0].z, tr[0].w,
			 tr[1].x, tr[1].y, tr[1].z, tr[1].w,
			 tr[2].x, tr[2].y, tr[2].z, tr[2].w,
			 0,       0,       0,       0
		 } * transform_matrix;
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
		 } * transform_matrix;

		 FOR(i, 3) {
			 intensity[i] = (Geom::V4f(pt_int[i]).norm().norm()).scMul(light_dir.norm());
			 intensity[i] = std::max(0.0f, intensity[i]);
//			 assert(intensity[i] <= 1.0f);
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
};

}

/*Geom::V3f scale, Geom::V3f rot, Geom::V3f light_dir) {
		light_dir = light_dir.norm();
		for (size_t i=0; i < model.faces.size(); i++) {
			V3i mas[3];
			V3i inten;
			V3i u;
			V3i v;
			bool suc = true;
			for (int j=0; j<3; j++) {
	//			V3f res = translationPipeline2(position, model.verts[model.faces[i][j]], scale, rot, suc).norm();
				V3f res = translationPipeline(position, model.verts[model.faces[i][j]], scale, rot, suc);
				mas[j] = V3i(res.x, res.y, scale_float(res.z));
				inten[j] =  scale_float((mainView.moveToCam((const V3f )model.normals[model.faces[i][j]].norm()).transform(position, rot, scale)).norm().scMul(light_dir));
				u[j] = scale_float(model.verts_tex[model.faces[i][j]][0]);
				v[j] = scale_float(model.verts_tex[model.faces[i][j]][1]);
			}
			FOR(j, 3){
				inten[j] = max(inten[j], 0);
			}
			Rgba color(255, 255, 255, 0);
			if(suc) {
				drawFilledTriangle3(makeTriangle6(Triangle(mas), inten, u, v), model.mats[model.mat_index].col[1][0]);
			}
		}
 */


#endif /* VERTEXSTAGES_H_ */
