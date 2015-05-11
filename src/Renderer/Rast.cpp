#include "Renderer/Rast.h"

using namespace Renderer;
using namespace Geom;
using namespace std;

static const float eps = 1e-5;

void Renderer::DefaultRast::convert(Geom::TriangleF4 &tr, bool &ret){
	Geom::Matrix44f pt = Geom::Matrix44f{
	 tr[0].x, tr[0].y, tr[0].z, tr[0].w,
	 tr[1].x, tr[1].y, tr[1].z, tr[1].w,
	 tr[2].x, tr[2].y, tr[2].z, tr[2].w,
	 0,       0,       0,       0
	} * screen_ma;
	FOR(i, 3) FOR(j, 4){
		tr[i][j] = pt[i][j] ;
	}
	FOR(i, 3){
		FOR(j, 3)
				tr[i][j] /= tr[i][3];
	}
	V2<int32_t> cnt(0, 0);
	FOR(i, 3){
		V3f tpt = tr[i].norm();
		if(between(0.0f, tpt.x, height))
				cnt.x++;
		if(between(0.0f, tpt.y, width))
				cnt.y++;
	}
	if(!cnt.x|| !cnt.y ){
		ret = false;
	}
}

void Renderer::DefaultRast::fix_y(){
	if(y1 < y2){
		it_y = max(0, int(y1) + (y1 - (int)y1> eps ? 1 : 0));
		bound_y = min(width - 1, y2);
	} else {
		it_y = max(0, int(y2) + (y2 - (int)y2> eps ? 1 : 0));
		bound_y = min(width - 1, y1);
	}
}

void Renderer::DefaultRast::fix(){
	while(true){
		if(x > mid.x && !half){
			half = 1;
			y1 = mid.y + add3 * (x - mid.x);
			add2 = add3;
			fix_y();
		}
		if(x > finish_x){
			break;
		}
		if(it_y > bound_y){
			x++;
			y1 += add2;
			y2 += add1;
			fix_y();
			continue;
		}
		break;
	}
}

void Renderer::DefaultRast::init(Geom::TriangleF4 tr, bool &ret){
	sort(tr.vs, tr.vs + 3);
    if (tr[0].x == tr[2].x){
    	ret = false;
    	return;
    }
    start_x = max(0, (int32_t)tr[0].x + ((tr[0].x - (int32_t)tr[0].x) > eps ? 1 : 0));
	finish_x = min(height - 1, tr[2].x);
	mid = V2<float>(tr[1].x, tr[1].y);
    add1 = (tr[2].y - tr[0].y) / (tr[2].x - tr[0].x);
    add2 = (tr[0].x == tr[1].x ? 0 : (tr[1].y - tr[0].y) / (tr[1].x - tr[0].x));
    add3 = (tr[2].x == tr[1].x ? 0 : (tr[2].y - tr[1].y) / (tr[2].x - tr[1].x));
	x = start_x;
	y2 = tr[0].y + add1 * (x - tr[0].x);
	y1 = tr[0].y + add2 * (x - tr[0].x);
	half = 0;
	fix_y();
	fix();
}

void Renderer::DefaultRast::process(
		std::tuple<Geom::TriangleF4, Geom::TriangleF>& inp, bool& ret) {
	convert(get<0>(inp), ret);
	init(get<0>(inp), ret);
}

bool Renderer::DefaultRast::have() {
	return x <= finish_x;
}

Geom::V2<int32_t> Renderer::DefaultRast::next() {
	V2<int32_t> ret(x, it_y++);
	fix();
	return ret;
}

Renderer::DefaultRast::DefaultRast(int32_t height, int32_t width) : width(width), height(height) {
	screen_ma = {
				height / 2.0f, 0, 0, 0,
				0, width / 2.0f, 0, 0,
				0, 0, 1, 0,
				height / 2.0f, width / 2.0f, 0, 1
	};
}
