#include "Renderer/Graphics.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Renderer/Log.h"

using namespace Renderer;
using namespace Geom;
//Helpers
static const int INF = 1e9;
struct LineStepper____{
	int dx, error2, derror2, y, delta, z;
	LineStepper____(const V3i &begin, const V3i & end){
		z = 0;
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

Geom::V3f Renderer::Drawer::toScreen(Geom::V3f pt){
	Matrix44f transf{
		buf->height / 2.0, 0, 0, 0,
		0, buf->width / 2.0, 0, 0,
		0, 0, 1, 0,
		buf->height / 2.0, buf->width / 2.0, 0, 1};
	return (V4f(pt).rowMatrix() * transf)[0];
}

V3f Renderer::Drawer::translate(V3f cen, V3f pt, V3f scale, V3f rot){
	V3f res = cen + (pt).rotate(rot).scale(scale);
	res = mainView.translate(res);
	res = toScreen(res);
	return res;
}

bool Renderer::Drawer::inScreen(int32_t x, int32_t y){
	return x >= 0 && x < buf->height && y >= 0 && y < buf->width;
}
//
Renderer::Rgba::Rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {
}


inline void Renderer::Drawer::at(uint8_t* ptr, const Rgba& color) {
	if(buf->format_desc[buf->format_desc_entry].bpp == 32){
		*((uint32_t *) ptr) = ((color.a) << 24) | ((color.r) << 16) |
							   ((color.g) << 8) | (color.b);
	}
}


void Renderer::Drawer::drawPixel(const int32_t& screen_x,const int32_t& screen_y, const uint32_t &h, const Rgba & color) {
	if(inScreen(screen_x, screen_y) && zbufferAt(screen_x, screen_y, h)){
		uint8_t * ptr = buf->map + screen_x * buf->stride + screen_y * buf->bpp / 8;
		at(ptr, color);
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



void Renderer::Drawer::drawLine(V3i begin, V3i end, const Rgba & color) {
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
			drawPixel(step.y, x, step.z, color);
		} else {
			drawPixel(x, step.y, step.z, color);
		}
		step.next();
	}
}

void Renderer::Drawer::drawTriangle(Triangle triangle,
		const Rgba& color) {
	for(int8_t i = 0; i < 3; i++){
			drawLine(triangle.vs[i], triangle.vs[(i + 1) % 3], color);
	}
}

void Renderer::Drawer::drawFilledTriangle(Triangle triangle,
		const Rgba& color) {
	drawTriangle(triangle, color);
	int mi_x = INF, ma_x=-INF, mi_y = INF, ma_y = -INF;
	for(size_t i = 0; i < 3; i++) {
		mi_x = std::min(mi_x, triangle.vs[i].x);
		ma_x = std::max(ma_x, triangle.vs[i].x);
		mi_y = std::min(mi_y, triangle.vs[i].y);
		ma_y = std::max(ma_y, triangle.vs[i].y);
	}
	bool swapped = false;
	if(ma_y - mi_y > ma_x - mi_x){
		swapped = true;
		for(size_t i = 0; i < 3; i++) {
			std::swap(triangle.vs[i].x, triangle.vs[i].y);
		}
	}
	std::sort(triangle.vs, triangle.vs + 3);
	LineStepper____ f(triangle.vs[0], triangle.vs[1]);
	LineStepper____ s(triangle.vs[0], triangle.vs[2]);
	for(int x = triangle.vs[0].x; x <= triangle.vs[2].x ; x++){
		if(!swapped)
			drawLine(V3i(x, f.y, f.z), V3i(x, s.y, s.z), color);
		else
			drawLine(V3i(f.y, x, f.z), V3i(s.y, x, s.z), color);
		if(x == triangle.vs[1].x){
			f = LineStepper____(triangle.vs[1], triangle.vs[2]);
		}
		f.next();
		s.next();
	}
}


void Renderer::Drawer::fill2(const Rgba& color) {

}

void Renderer::Drawer::drawBegin(Buffer * buf, const CameraView &mainView_) {
	this->buf = buf;
	current_draw++;
	zbuffer.resize(buf->size);
	check.resize(buf->size);
	mainView = mainView_;
}

bool Renderer::Drawer::zbufferAt(const int32_t& x, const int32_t& y,const int32_t& h) {
	int32_t pos = x * buf->width + y;
	if(check[pos] != current_draw){
		check[pos] = current_draw;
		zbuffer[pos] = INF;
	}
	if(zbuffer[pos] > h){
		zbuffer[pos] = h;
		return true;
	}
	return false;
}



void Renderer::Drawer::drawEnd() {
}

void Renderer::MeshModel::loadObj(const char* filename) {
	std::ifstream in;
	in.open (filename, std::ifstream::in);
	if(in.fail()){
		DEB("%s\n", filename);
		return;
	}
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::stringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			V3f v;
			for (int i=0;i<3;i++) iss >> v[i];
				verts.push_back(v);
		}  else if (!line.compare(0, 2, "f ")) {
			 std::vector<int> f;
			int itrash, idx;
			iss >> trash;
			while (iss >> idx >> trash >> itrash >> trash >> itrash) {
			idx--;
			f.push_back(idx);
			}
			faces.push_back(f);
		}
	}
}

Renderer::CameraView::CameraView(Geom::V3f cen, Geom::V3f up1, Geom::V3f f, float angle, int32_t width, int32_t height, float near, float far) :
		cen(cen) , f(f), aw(angle), near(near), far(far){

	this->r = up1.vMul(this->f);
	this->up = this->f.vMul(this->r);

	this->f.norm();
	this->up.norm();
	this->r.norm();

	ah = atan(tan(aw) * height / width);
}



void Renderer::Drawer::drawModel(const MeshModel& model, Geom::V3f position,
		Geom::V3f scale, Geom::V3f rot) {
	for (size_t i=0; i < model.faces.size(); i++) {
		std::vector<int> face = model.faces[i];
		V3i mas[3];
		for (int j=0; j<3; j++) {
			V3f res = translate(position, model.verts[face[j]], scale, rot);
			mas[j] = V3i(res.x, res.y, res.z * 10000);
 		}
		Rgba color(0, 0, 0, 0);
		drawTriangle(Triangle(mas), color);
	}
}


Geom::V3f Renderer::CameraView::translate(Geom::V3f v) {
	v = v - cen;
	v.x = v.scMul(r);
	v.y = v.scMul(up);
	v.z = v.scMul(f);
	V4f v4(v, 1);
	float projection_plane_z = 1.0;
	float right = std::tan(aw) * projection_plane_z;
	float left = -right;
	float top = std::tan(ah) * projection_plane_z;
	float bottom = -top;
	v4 = (v4.rowMatrix() * MatrixFactory::projection(left, right, top, bottom, near, far))[0];
	V3f t = v4.norm();
	DEB("%f  %f %f %f\n", t.x, t.y, t.z);
	return t;
}
