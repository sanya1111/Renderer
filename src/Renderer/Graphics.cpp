#include "Renderer/Graphics.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Renderer/Log.h"

//Helpers
static const int INF = 1e9;
struct LineStepper____{
	int dx, error2, derror2, y, delta;
	LineStepper____(const Renderer::Geom::Point2D<int32_t> &begin, const Renderer::Geom::Point2D<int32_t> & end){
		dx = end.x - begin.x;
		derror2 = std::abs(end.y - begin.y) * 2;
		error2 = 0;
		y = begin.y;
		delta = (end.y > begin.y ? 1 : -1);
	}
	int next(){
		error2 += derror2;
		while (dx && error2 > dx) {
			y += delta;
			error2 -= dx * 2;
		}
		return y;
	}
};
//
Renderer::Rgba::Rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {
}

void Renderer::Drawer::setBuffer(Buffer* buf) {
	this->buf = buf;
}

inline void Renderer::Drawer::at(uint8_t* ptr, const Rgba& color) {
	if(buf->format_desc[buf->format_desc_entry].bpp == 32){
		*((uint32_t *) ptr) = ((color.a) << 24) | ((color.r) << 16) |
							   ((color.g) << 8) | (color.b);
	}
}


void Renderer::Drawer::fill(const Rgba& color) {
	uint8_t * ptr  = buf->map;
	for (uint32_t i = 0; i < buf->height; i++) {
		for (uint32_t j = 0; j < buf->width; j++, ptr = ptr + buf->bpp / 8) {
			at(ptr, color);
		}
	}
}

void Renderer::Drawer::drawPixel(const uint32_t& screen_x,const uint32_t& screen_y, const uint32_t &screen_h, const Rgba & color) {
	if()
	uint8_t * ptr = buf->map + screen_x * buf->stride + screen_y * buf->bpp / 8;
	at(ptr, color);
}

void Renderer::Drawer::drawLine(Geom::Point2D<int32_t> begin, Geom::Point2D<int32_t> end, const Rgba & color) {
	bool swapped = false;
	if (std::abs(begin.x - end.x) < std::abs(begin.y - end.y)) {
		std::swap(begin.x, begin.y);
		std::swap(end.x, end.y);
		swapped = true;
	}
	if (begin.x > end.x) {
		std::swap(begin, end);
	}
	LineStepper____ step(begin, end);
	for (int x=begin.x; x<=end.x; x++) {
		if (swapped) {
			drawPixel(step.y, x, color);
		} else {
			drawPixel(x, step.y, color);
		}
		step.next();
	}
}

void Renderer::Drawer::drawTriangle(Geom::Triangle2D<int32_t> triangle,
		const Rgba& color) {
	for(int8_t i = 0; i < 3; i++){
			drawLine(triangle.points[i], triangle.points[(i + 1) % 3], color);
	}
}

void Renderer::Drawer::drawFilledTriangle(Geom::Triangle2D<int32_t> triangle,
		const Rgba& color) {
	drawTriangle(triangle, color);
	int mi_x = INF, ma_x=-INF, mi_y = INF, ma_y = -INF;
	for(size_t i = 0; i < 3; i++) {
		mi_x = std::min(mi_x, triangle.points[i].x);
		ma_x = std::max(ma_x, triangle.points[i].x);
		mi_y = std::min(mi_y, triangle.points[i].y);
		ma_y = std::max(ma_y, triangle.points[i].y);
	}
	bool swapped = false;
	if(ma_y - mi_y > ma_x - mi_x){
		swapped = true;
		for(size_t i = 0; i < 3; i++) {
			std::swap(triangle.points[i].x, triangle.points[i].y);
		}
	}
	std::sort(triangle.points, triangle.points + 3);
	LineStepper____ f(triangle.points[0], triangle.points[1]);
	LineStepper____ s(triangle.points[0], triangle.points[2]);
	for(int x = triangle.points[0].x; x <= triangle.points[2].x ; x++){
		if(!swapped)
			drawLine(Geom::Point2D<int32_t>(x, f.y), Geom::Point2D<int32_t>(x, s.y), color);
		else
			drawLine(Geom::Point2D<int32_t>(f.y, x), Geom::Point2D<int32_t>(s.y, x), color);
		if(x == triangle.points[1].x){
			f = LineStepper____(triangle.points[1], triangle.points[2]);
		}
		f.next();
		s.next();
	}
}

void Renderer::Drawer::drawMash(const char* filename, const Rgba& color){
	using namespace Geom;
	std::ifstream in;
    in.open (filename, std::ifstream::in);
    if(in.fail()){
    	DEB("%s\n", filename);
    	return;
    }
    std::string line;
    std::vector<Point3D<double>> verts;
    std::vector<std::vector<int>> faces;
    float mini[3] = {INF, INF, INF};
    while (!in.eof()) {
        std::getline(in, line);
        std::stringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Point3D<float> v;
            for (int i=0;i<3;i++) iss >> v[i], mini[i] = std::min(mini[i], v[i]);
            	verts.push_back(v);
        }  else if (!line.compare(0, 2, "f ")) {
        	 std::vector<int> f;
        	int itrash, idx;
        	iss >> trash;
        	while (iss >> idx >> trash >> itrash >> trash >> itrash) {
        	idx--; // in wavefront obj all indices start at 1, not zero
        	f.push_back(idx);
        	}
        	faces.push_back(f);
        }
    }
    float maxi[3] = {0, 0, 0};
    for(int i = 0; i < 3; i++){
    	mini[i]--;
    }
    for(auto &v : verts){
    	for(int i = 0; i < 3; i++){
    		v[i] += abs(mini[i]);
    		maxi[i] = std::max(maxi[i], v[i]);
    	}
    }
    for (size_t i=0; i < faces.size(); i++) {
		std::vector<int> face = faces[i];
		Point2D<int32_t> mas[3];
		for (int j=0; j<3; j++) {
			int x1 = (verts[face[j]].x/(maxi[0] + .1) ) * buf->height ;
			int y1 = (verts[face[j]].y/(maxi[1] + .1) ) * buf->width ;
			mas[j] = Point2D<int32_t>(x1, y1);
		}

		drawFilledTriangle(Triangle2D<int32_t>(mas), color);
    }
}

void Renderer::Drawer::fill2(const Rgba& color) {

}

void Renderer::Drawer::drawBegin() {
	current_draw++;
	zbuffer.resize(buf->size);
	check.resize(buf->size);
}

void Renderer::Drawer::drawEnd() {
}
