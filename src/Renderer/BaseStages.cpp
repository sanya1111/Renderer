#include "Renderer/BaseStages.h"

using namespace Renderer;
using namespace Geom;
using namespace std;

bool Renderer::MeshModelStage::have() {
        return ptr < model->faces.size();
}


Renderer::MeshModelStage::result Renderer::MeshModelStage::process ( bool& fin ) {
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



Renderer::MeshModelStage::MeshModelStage(const MeshModel& model)  : model(&model), norm_v(model.verts), ptr(0){
//        V3f m(0, 0, 0);
//        for_each(model.verts.begin(), model.verts.end(), [&m](const V3f &v){
//                FOR(i, 3){
//                        m[i] = max(m[i], fabs(v[i]));
//                }
//        });
////        DEB("here ");
////        m.print();
//        for_each(norm_v.begin(), norm_v.end(), [&m](V3f &v){
//                FOR(i, 3){
//                        v[i] /= m[i];
//                }
//        });
}

void Renderer::MeshModelStage::start() {
        ptr = 0;
}

void Renderer::MeshModelStage::operator=(MeshModelStage &&mstage) {
         model = mstage.model;
         ptr = mstage.ptr;
         norm_v = move(mstage.norm_v);
}

//==================================================
bool Renderer::ModelStage::have() {
	return ptr < mstage.size();
}

void Renderer::ModelStage::next(){
	ptr++;
	if(ptr < mstage.size())
		mstage[ptr].start();
	while(ptr < mstage.size() && !mstage[ptr].have()){
		ptr++;
	}
}

Renderer::ModelStage::result Renderer::ModelStage::process ( bool& fin ) {
	result res = mstage[ptr].process(fin);
	if(!mstage[ptr].have()){
		next();
	}
	return res;
}



Renderer::ModelStage::ModelStage(const Model& model) : mstage(model.meshs.size()) {
	FOR(i, model.meshs.size()){
		mstage[i] = move(MeshModelStage(model.meshs[i]));
	}
	DEB("OK");
}

void Renderer::ModelStage::start() {
	ptr = -1;
	next();
}

void Renderer::ModelStage::operator=(ModelStage &&with) {
	mstage = move(with.mstage);
	ptr = with.ptr;
}
