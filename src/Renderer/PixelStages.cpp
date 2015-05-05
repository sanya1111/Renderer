#include "Renderer/PixelStages.h"
#include "Renderer/Utils.hpp"
#include "Renderer/Graphics.h"

using namespace Renderer;
using namespace Geom;
using namespace std;

static inline V3f barycentric2(TriangleF4 a, V2<int> p){
	V3f s[2];
	FOR(i, 2){
		s[i][0] = a.vs[2][i]-a.vs[0][i];
		s[i][1] = a.vs[1][i]-a.vs[0][i];
		s[i][2] = a.vs[0][i] - p[i];
	}
	V3f u = s[0].vMul(s[1]);
	if (std::abs(u[2]) < 1) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return V3f(-1,1,1);
	return V3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

void Renderer::DefaultPixelStage::process(std::tuple<Geom::TriangleF4,Geom::TriangleF>& result){
	tr = &get<0>(result);
	other = &get<1>(result);
}

bool Renderer::DefaultPixelStage::apply(Geom::V2<int>& pt) {
	last = &pt;
	V3f bc_screen = barycentric2(*tr, pt);
	//maybe if here
	bc_clip = V3f(bc_screen.x/ (*tr)[0][3],
					  bc_screen.y/ (*tr)[1][3],
					  bc_screen.z/ (*tr)[2][3]);
	bc_clip = bc_clip * (1.0 / (bc_clip.x+bc_clip.y+bc_clip.z));
}

int Renderer::DefaultPixelStage::getZ(){
	return 10000 * (V3f(tr->vs[0][2], tr->vs[1][2], tr->vs[2][2]).scMul(bc_clip));
}

Rgba Renderer::DefaultPixelStage::getColor(){
	float inten = ((*other)[0].scMul(bc_clip));
	int c1 = ((*other)[1].scMul(bc_clip)) * tex->width;
	int c2 = tex->height - ((*other)[2].scMul(bc_clip)) * tex->height;
	inten = max(inten, 0.0f);
	return tex->at(c2, c1) * inten;
}