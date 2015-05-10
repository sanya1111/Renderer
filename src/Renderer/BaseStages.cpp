#include "Renderer/BaseStages.h"

using namespace Renderer;
using namespace Geom;
using namespace std;

bool Renderer::ModelStage::have() {
	return ptr < model->faces.size();
}


ModelStage::result Renderer::ModelStage::process ( bool& fin ) {
	result res;
	FOR(j,3){
		FOR(k,3){
			get<0>(res)[j][k] = norm_v[model->faces[ptr][j]][k];
		}
		get<0>(res)[j][3] = 1;
		get<1>(res)[j] = model->normals[model->faces[ptr][j]];
		get<2>(res)[j] = model->verts_tex[model->faces[ptr][j]][0]; //u
		get<3>(res)[j] = model->verts_tex[model->faces[ptr][j]][1]; //v
	}
	ptr++;
	return res;
}



Renderer::ModelStage::ModelStage(const MeshModel& model)  : model(&model), norm_v(model.verts), ptr(0){
	V3f m(0, 0, 0);
	for_each(model.verts.begin(), model.verts.end(), [&m](const V3f &v){
		FOR(i, 3){
			m[i] = max(m[i], fabs(v[i]));
		}
	});
	for_each(norm_v.begin(), norm_v.end(), [&m](V3f &v){
		FOR(i, 3){
			v[i] /= m[i];
		}
	});
}

void Renderer::ModelStage::start() {
	ptr = 0;
}

void Renderer::ModelStage::operator=(ModelStage &&mstage) {
	 model = mstage.model;
	 ptr = mstage.ptr;
	 norm_v = move(mstage.norm_v);
}
