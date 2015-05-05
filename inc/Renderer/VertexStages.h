#ifndef VERTEXSTAGES_H_
#define VERTEXSTAGES_H_

#include "Renderer/Camera.h"
#include "Renderer/Geom.hpp"
#include "Renderer/Utils.hpp"

namespace Renderer{

class DefaultVertexStage{
	Geom::Matrix44f transform_matrix;
	Geom::V3f light_dir;
public:
	typedef std::tuple<Geom::TriangleF4, Geom::TriangleF> result;
	DefaultVertexStage(const CameraView &main_view, Geom::V3f position, Geom::V3f scale, Geom::V3f rot, Geom::V3f light_dir) : light_dir(light_dir){
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
			 float mi = -tr[i].w;
			 float ma = tr[i].w;
			 if(mi > ma)
				 std::swap(mi, ma);
			 bool yep = true;
			 FOR(j, 3){
				 if(!between(mi, tr[i][j], ma)){
					 yep = false;
					 break;
				 }
			 }
			 if(yep){
				 ret = true;
				 break;
			 }
		 }
		 if(!ret)
			 return res;

		 Geom::V3f intensity;
		 FOR(i, 3) {
			 intensity[i] = std::get<1>(tu)[i].scMul(light_dir);
		 }
		 Geom::TriangleF4 &f = std::get<0>(res);
		 Geom::TriangleF &s = std::get<1>(res);
		 FOR(i, 3){
			 FOR(j, 4){
				 f[i][j] = pt[i][j];
			 }
			 s[0][i] = intensity[i];
			 s[1][i] = std::get<2>(tu)[i];
			 s[2][i] = std::get<3>(tu)[i];
		 }
		 f[0].print();
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
