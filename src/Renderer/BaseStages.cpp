#include "Renderer/BaseStages.h"

using namespace Renderer;
using namespace Geom;
using namespace std;

bool Renderer::ModelStage::have() {
	return current_mesh < model->meshs.size();
}

void Renderer::ModelStage::next_mesh(){
	current_mesh++;
	ptr_face = 0;
	if(current_mesh >= model->meshs.size())
		return;
	norm_v = model->meshs[current_mesh].verts;
	V3f m(0, 0, 0);
	for_each(norm_v.begin(), norm_v.end(), [&m](V3f &v){
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

ModelStage::result Renderer::ModelStage::process ( bool& fin ) {
	result res;
	if(ptr_face < model->meshs[current_mesh].faces.size()){
		FOR(j,3){
			size_t v_id =model->meshs[current_mesh].faces[ptr_face][j];
			FOR(k,3){
				get<0>(res)[j][k] = norm_v[v_id][k];
			}
			get<0>(res)[j][3] = 1;
			get<1>(res)[j] = model->meshs[current_mesh].normals[v_id];
			get<2>(res)[j] = model->meshs[current_mesh].verts_tex[v_id][0]; //u
			get<3>(res)[j] = model->meshs[current_mesh].verts_tex[v_id][1]; //v
		}
	} else {
		fin = false;
	}
	ptr_face++;
	if(ptr_face >= model->meshs[current_mesh].faces.size()){
		next_mesh();
	}
	return res;
}



Renderer::ModelStage::ModelStage(const Model& model)  : model(&model){
}

void Renderer::ModelStage::start(){
	current_mesh = -1;
	next_mesh();
}

void Renderer::ModelStage::operator=(ModelStage &&mstage) {
	 model = mstage.model;
	 current_mesh = mstage.current_mesh;
	 ptr_face = mstage.ptr_face;
	 norm_v = move(mstage.norm_v);
}
