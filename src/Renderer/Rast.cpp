#include "Renderer/Rast.h"

using namespace Renderer;
using namespace Geom;
using namespace std;

void Renderer::DefaultRast::changeT(){
	now = 1;
	p1 = p3;
	p2 = p4;
	bleft = bright;
	dleft = dright;
}

void Renderer::DefaultRast::moveP(){
	p1.x++;
	p2.x++;
	p1.y += dleft[0];
	p2.y += dleft[1];
	aty = p1.y;
	toy = p2.y;
	if(aty > toy){
		swap(aty, toy);
	}
	toy = min(toy, width - 1);
	aty = max(aty, 0.0f);
}
void Renderer::DefaultRast::change(){
	while(1){
		if(p1.x > bleft){
			if(now)
				return;
			changeT();
			continue;
		}
		if(aty > toy){
			moveP();
			continue;
		}
		break;
	}
}

void Renderer::DefaultRast::init(Geom::TriangleF4 tr){
	sort(tr.vs, tr.vs + 3);
	FOR(i, 3){
		tr[i].x = floor(tr[i].x);
		tr[i].y = floor(tr[i].y);
	}
	//L
	dleft[0] = tr[1].x == tr[0].x ? 0 : (tr[1].y - tr[0].y) / (tr[1].x - tr[0].x);
	dleft[1] = tr[2].x == tr[0].x ? 0 : (tr[2].y - tr[0].y) / (tr[2].x - tr[0].x);

	bleft = min((float)height - 1, tr[1].x);
	p1.x = tr[0].x; p1.x = tr[0].y;
	p2 = p1;
	float delta = max(0.0f, 0 - tr[0].x);
	p1.x += delta;
	p1.y += delta * dleft[0];
	p2.x += delta;
	p2.y += delta * dleft[1];
	if(tr[0].x == tr[1].x){
		p1 = tr[0];
		p2 = tr[1];
	}
	//R
	dright[0] = tr[2].x == tr[1].x ? 0 : (tr[2].y - tr[1].y) / (tr[2].x - tr[1].x);
	dright[1] = tr[2].x == tr[0].x ? 0 : (tr[2].y - tr[0].y) / (tr[2].x - tr[0].x);

	bright = min((float)height - 1, tr[2].x);
	p3.x = tr[0].x; p3.x = tr[0].y;
	p4 = p3;
	float delta = (tr[1].x - tr[0].x) + max(0.0f, 0 - tr[1].x);
	p3.x += delta;
	p3.y += delta * dright[0];
	p4.x += delta;
	p4.y += delta * dright[1];
	if(tr[1].x == tr[2].x) {
		p3 = tr[1];
		p4 = tr[2];
	}
	//now
	now = 0;
	change();
}
void Renderer::DefaultRast::process(
		std::tuple<Geom::TriangleF4, Geom::TriangleF>& inp, bool& ret) {
	Geom::TriangleF4& tr = &get<0>(inp);
	Geom::Matrix44f pt = Geom::Matrix44f{
	 tr[0].x, tr[0].y, tr[0].z, tr[0].w,
	 tr[1].x, tr[1].y, tr[1].z, tr[1].w,
	 tr[2].x, tr[2].y, tr[2].z, tr[2].w,
	 0,       0,       0,       0
	} * screen_ma;
	FOR(i, 3) FOR(j, 4){
		tr[i][j] = pt[i][j];
	}
	//TODO test here MUST be
//	saved_tr = tr;
	init(tr);
}

bool Renderer::DefaultRast::have() {
	return !(now == 1 && p1.x > bleft);
}

Geom::V2<int> Renderer::DefaultRast::next() {

}

Renderer::DefaultRast::DefaultRast(int height, int width) : width(width), height(height) {
	screen_ma = {
				height / 2.0f, 0, 0, 0,
				0, width / 2.0f, 0, 0,
				0, 0, 1, 0,
				height / 2.0f, width / 2.0f, 0, 1
	};
}
