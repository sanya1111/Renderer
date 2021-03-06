#include "Renderer/PixelStages.h"
#include "Renderer/Utils.hpp"
#include "Renderer/Graphics.h"

using namespace Renderer;
using namespace Geom;
using namespace std;

static inline V3f barycentric(TriangleF4 a, V2<int> &p){
	V3f s[2];
	FOR(i, 2){
		s[i][0] = a.vs[2][i]-a.vs[0][i];
		s[i][1] = a.vs[1][i]-a.vs[0][i];
		s[i][2] = a.vs[0][i] - p[i];
	}
	V3f u = s[0].vMul(s[1]);
	return V3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

void Renderer::DefaultPixelStage::save(std::tuple<Geom::TriangleF4,Geom::TriangleF>& result){
	tr = &get<0>(result);
	other = &get<1>(result);
}

bool Renderer::DefaultPixelStage::apply( Geom::V2<int>& pt) {
	V3f bc_screen = barycentric(*tr, pt);
	bc_clip = V3f(bc_screen.x / tr->vs[0][3],
					  bc_screen.y / tr->vs[1][3],
					  bc_screen.z / tr->vs[2][3]);
	if(bc_clip.x < 0 || bc_clip.y < 0 || bc_clip.z < 0){
			return false;
	}
	bc_clip = bc_clip * (1.0 / (bc_clip.x + bc_clip.y + bc_clip.z));
	return true;
}

float Renderer::DefaultPixelStage::getZ(){
	return  (V3f(tr->vs[0][2] * tr->vs[0][3], tr->vs[1][2] * tr->vs[1][3], tr->vs[2][2] * tr->vs[2][3]).scMul(bc_clip));
}

float Renderer::DefaultPixelStage::getW(){
	return  (V3f(tr->vs[0][3], tr->vs[1][3], tr->vs[2][3]).scMul(bc_clip));
}

Rgba Renderer::DefaultPixelStage::getColor(){
	float inten = V3f(other->vs[0][0], other->vs[1][0], other->vs[2][0]).scMul(bc_clip);
	int c1 = (V3f(other->vs[0][1], other->vs[1][1], other->vs[2][1]).scMul(bc_clip)) * (tex->width - 1);
	int c2 = tex->height - 1 - (V3f(other->vs[0][2], other->vs[1][2], other->vs[2][2]).scMul(bc_clip)) * (tex->height - 1);
	return tex->at(c2, c1) * inten;
}

Geom::V2<float> DefaultPixelStage::getXY(){
	V2<float> res;
	FOR(i, 2){
		res[i] = V3f(tr->vs[0][i] * tr->vs[0][3] , tr->vs[1][i] * tr->vs[1][3], tr->vs[2][i] * tr->vs[2][3]).scMul(bc_clip);
	}
	return res;
}
Geom::V2<float> DefaultPixelStage::getxy(){
	V2<float> res;
	FOR(i, 2){
		res[i] = V3f(tr->vs[0][i]  , tr->vs[1][i] , tr->vs[2][i] ).scMul(bc_clip);
	}
	return res;
}

float Renderer::DefaultPixelStage::getzz(){
	return  (V3f(tr->vs[0][2] , tr->vs[1][2] , tr->vs[2][2] ).scMul(bc_clip));
}

