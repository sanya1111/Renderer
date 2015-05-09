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
	V2<int> cnt(0, 0);
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
			fix_y();
		}
		if(x > finish_x){
			break;
		}
		if(it_y > bound_y){
			x++;
			fix_y();
			continue;
		}
		break;
	}
}

void Renderer::DefaultRast::init(Geom::TriangleF4 tr, bool &ret){
	sort(tr.vs, tr.vs + 3);
	FOR(i, 3){
	}
    if (tr[0].x == tr[2].x){
    	ret = false;
    	return;
    }
    start_x = max(0, (int)tr[0].x + ((tr[0].x - (int)tr[0].x) > eps ? 1 : 0));
	finish_x = min(height - 1, tr[2].x);
	mid = V2<float>(tr[1].x, tr[1].y);
    add1 = (tr[2].y - tr[0].y) / (tr[2].x - tr[0].x);
    add2 = (tr[0].x == tr[1].x ? 0 : (tr[1].y - tr[0].y) / (tr[1].x - tr[0].x));
    add3 = (tr[2].x == tr[1].x ? 0 : (tr[2].y - tr[1].y) / (tr[2].x - tr[1].x));
	x = start_x;
	y2 = tr[0].y + add1 * (x - tr[0].x);
	if(x > tr[1].x){
		half = 1;
		y1 = tr[1].y + add3 * (x - tr[1].x);
	} else {
		half = 0;
		y1 = tr[0].y + add2 * (x - tr[0].x);
	}
	fix_y();
	fix();
    /*for(int i = max(start, 0); i <= min((int)height - 1, finish); i++){
            V4f p2 = tr[0] + (tr[2] - tr[0]) * (float(i - tr[0].x) / (tr[2].x - tr[0].x));
            V4f p1;
            if (i >= tr[1].x){
                    if(tr[2].x == tr[1].x){
                            p1 = tr[1];
                    } else {
                            p1 = tr[1] + (tr[2] - tr[1]) * ((i - tr[1].x) / (tr[2].x - tr[1].x));
                    }
            } else {
                    if(tr[0].x == tr[1].x) {
                            p1 = tr[0];
                    } else {
                            p1 = tr[0] + (tr[1] - tr[0]) * (float(i - tr[0].x) / (tr[1].x - tr[0].x));
                    }
            }
            if(p1.y > p2.y){
                    swap(p1, p2);
            }
            int start_y = p1.y + (p1.y - (int)p1.y > eps ? 1 : 0);
            int end_y = p2.y;
            for(int j = max(start_y, 0); j <= min(end_y, (int) width - 1); j++){
                    V2<int> pt(i, j);
                    if(pstage.apply(pt)){
                            drawPixel(pt.x, pt.y, pstage.getZ(), pstage.getColor());
                    }
            }
    }
    */
}
void Renderer::DefaultRast::process(
		std::tuple<Geom::TriangleF4, Geom::TriangleF>& inp, bool& ret) {
	convert(get<0>(inp), ret);
	FOR(i, 3){
		get<0>(inp)[i].print();
	}
	init(get<0>(inp), ret);
}

bool Renderer::DefaultRast::have() {
	return x <= finish_x;
}

Geom::V2<int> Renderer::DefaultRast::next() {
	V2<int> ret(x, it_y++);
	fix();
	return ret;
}

Renderer::DefaultRast::DefaultRast(int height, int width) : width(width), height(height) {
	screen_ma = {
				height / 2.0f, 0, 0, 0,
				0, width / 2.0f, 0, 0,
				0, 0, 1, 0,
				height / 2.0f, width / 2.0f, 0, 1
	};
}
