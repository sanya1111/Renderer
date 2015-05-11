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
	result process(std::tuple<Geom::TriangleF4, Geom::TriangleF, Geom::V3f, Geom::V3f> &tu, bool &ret);
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
