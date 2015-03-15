#include "Renderer/Graphics.h"
#include <vector>
#include <fstream>
#include <sstream>

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

void Renderer::Drawer::drawPixel(const uint32_t& screen_x,const uint32_t& screen_y, const Rgba & color) {
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
	int dx = end.x - begin.x;
	int dy = end.y - begin.y;
	int derror2 = std::abs(dy)*2;
	int error2 = 0;
	int y = begin.y;
	for (int x=begin.x; x<=end.x; x++) {
		if (swapped) {
			drawPixel(y, x, color);
		} else {
			drawPixel(x, y, color);
		}
		error2 += derror2;

		if (error2 > dx) {
			y += (end.y > begin.y ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void Renderer::Drawer::drawTriangle(Geom::Triangle2D<int32_t> triangle,
		const Rgba& color) {
	for(int8_t i = 0; i < 3; i++){
			drawLine(triangle.points[i], triangle.points[(i + 1) % 3], color);
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
    std::vector<Point3D<float>> verts;
    std::vector<std::vector<int>> faces;
    float mini[3] = {1e9, 1e9, 1e9};
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
		drawTriangle(Triangle2D<int32_t>(mas), color);
    }
}

