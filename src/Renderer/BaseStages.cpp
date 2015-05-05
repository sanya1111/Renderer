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
			get<0>(res)[j][k] = model->verts[model->faces[ptr][j]][k];
		}
		get<0>(res)[j][3] = 1;
		get<1>(res)[j] = model->normals[model->faces[ptr][j]];
		get<2>(res)[j] = model->verts_tex[model->faces[ptr][j]][0]; //u
		get<3>(res)[j] = model->verts_tex[model->faces[ptr][j]][1]; //v
	}
	return res;
}


Renderer::ModelStage::ModelStage(const MeshModel& model)  : model(&model), ptr(0){
}

