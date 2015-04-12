#include "Renderer/Graphics.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Renderer/Log.h"

using namespace Renderer;
using namespace Geom;
using namespace std;
//Helpers
static const int INF = 1e9;
static const float eps = 1e-9;



static inline void * mem(void *s, int32_t c, size_t count){
	 asm volatile("rep\n\t"
			 	 "stosl"
			 :
			 : "a" (c), "D" (s), "c" (count)
			 : "memory");
	 return s;
}

static inline bool inBounds(float w, float b, float e){
	return w >= b  && w <= e;
}

static inline bool inBounds(int w, int b, int e){
	return w  >= b && w <= e;
}

static inline float segmSolver(float x_s, float x_e, float at){
	if(x_s == x_e)
		return INF;
	return ( (at - x_s) / (x_e - x_s));
}

static inline float segmPointer(float x_s, float x_e, float al){
	return x_s + al * (x_e - x_s);
}

void toBounds(int &a, int l, int r){
	a = max(a, l);
	a = min(a, r);
}

struct LineStepper{
	V3i begin, end, current, error2, derror2, delta;
	int dx;
	bool swapped;

	LineStepper(const V3i &begin_, const V3i & end_, int8_t force_asix = -1, Drawer * dr = NULL, int ignored = 0) : begin(begin_), end(end_){
		if(dr){
			dr->LineToScreenBounds(begin, end, ignored);
		}
		swapped = false;
		if (force_asix != 0 && (force_asix == 1 ||  std::abs(begin.x - end.x) < std::abs(begin.y - end.y))) {
			std::swap(begin.x, begin.y);
			std::swap(end.x, end.y);
			swapped = true;
		}
		if (begin.x > end.x) {
			std::swap(begin, end);
		}

		for(int i = 1; i < 3; i++){
				delta[i] = sign(end[i] - begin[i]);
				derror2[i] = std::abs(end[i] - begin[i]) * 2;
		}
		current = begin;
		dx = end.x - begin.x;
	}
	bool finish(){
		return current.x >= end.x;
	}

	void next(){
		current.x++;
		error2 = error2 + derror2;
		for(int i = 1; i < 3; i++){
			if(error2[i] > dx){
				int tim = ((error2[i]  - dx)/ (dx * 2)) + (error2[i] % (dx * 2 ) > 0);
				current[i] += delta[i] * tim;
				error2[i] -= dx * 2 * tim;
			}
		}
	}
	V3i getP(){
		V3i ret = current;
		if(swapped)
			swap(ret.x, ret.y);
		return ret;
	}
};

Geom::V3f Renderer::Drawer::toScreenTranslation(Geom::V3f pt){
	Matrix44f transf{
		buf->height / 2.0f, 0, 0, 0,
		0, buf->width / 2.0f, 0, 0,
		0, 0, 1, 0,
		buf->height / 2.0f, buf->width / 2.0f, 0, 1};
	return (V4f(pt, 1).rowMatrix() * transf)[0];
}



V3f Renderer::Drawer::translationPipeline(V3f cen, V3f pt, V3f scale, V3f rot, bool &success){
	V3f res = cen + (pt).rotate(rot).scale(scale);
	res = mainView.translate(res);
	if(!inBounds(res.x, -1.0, 1.0) ||
	   !inBounds(res.y, -1.0, 1.0) ||
	   !inBounds(res.z, -1.0, 1.0)){
		success = false;
	}
	res = toScreenTranslation(res);

	return res;
}

bool Renderer::Drawer::inScreen(int32_t x, int32_t y){
	return inBounds(x, 0, buf->height - 1) &&
		   inBounds(y, 0, buf->width - 1 );
}

void Renderer::Drawer::LineToScreenBounds(Geom::V3i & begin, Geom::V3i & end, int ignored ){
	V3i bounds_mi = { 0, 0, 0};
	V3i bounds_ma = { (int)buf->height - 1, (int)buf->width - 1, (int)INF};
	V3i nbegin = begin, nend = end;
	for(int i = 0; i < 3; i++){
		if(ignored &(1 <<i))
			continue;
		toBounds(nbegin[i], bounds_mi[i], bounds_ma[i]);
		toBounds(nend[i], bounds_mi[i], bounds_ma[i]);
		float a = segmSolver(begin[i], end[i], nbegin[i]);
		float b = segmSolver(begin[i], end[i], nend[i]);
		for(int j= 0; j < 3; j++){
			if(a != INF)
				nbegin[j] = segmPointer(begin[j], end[j], a);
			if(b != INF)
				nend[j] = segmPointer(begin[j], end[j], b);
		}
	}
	begin = nbegin;
	end = nend;
}

Renderer::Rgba::Rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {
}


inline void Renderer::Drawer::at(uint8_t* ptr, const Rgba& color) {
	if(buf->format_desc[buf->format_desc_entry].bpp == 32){
		*((uint32_t *) ptr) = ((color.a) << 24) | ((color.r) << 16) |
							   ((color.g) << 8) | (color.b);
	}
}


void Renderer::Drawer::drawPixel(const int32_t& screen_x,const int32_t& screen_y, const uint32_t &h, const Rgba & color) {
	if(zbufferAt(screen_x, screen_y, h)){
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
	LineToScreenBounds(begin, end);
	LineStepper step(begin, end);
	V3i pt;
	while(!step.finish()){
		step.next();
		pt = step.getP();
		drawPixel(pt.x, pt.y, pt.z, color);
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
	std::sort(triangle.vs, triangle.vs + 3);
	LineStepper a(triangle.vs[0], triangle.vs[2], 0, this, 6);
	LineStepper b(triangle.vs[0], triangle.vs[1], 0, this, 6);
	V3i pa, pb;
	while(!a.finish()){
		pa = a.getP();
		if(pa.x == triangle.vs[1].x){
			b = LineStepper(triangle.vs[1], triangle.vs[2], 0, this,  6);
		}
		pb = b.getP();
		drawLine(pa, pb, color);
		a.next();
		b.next();
	}
}


void Renderer::Drawer::fill2(const Rgba& color) {
	if(buf->format_desc[buf->format_desc_entry].bpp == 32){
		int32_t col = ((color.a) << 24) | ((color.r) << 16) |
				   ((color.g) << 8) | (color.b);
		mem(buf->map, col, buf->size / sizeof(int32_t));
	}
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

void Renderer::Drawer::drawTranslateTriangle(Geom::TriangleF triangle,
		const Rgba& color) {
	bool suc = true;
	for(int8_t i = 0; i < 3; i++){
		triangle.vs[i] = translationPipeline(V3f(0, 0, 0), triangle.vs[i],
				V3f(1, 1, 1), V3f(0, 0, 0), suc);
	}
	drawTriangle(triangle, color);
}

void Renderer::Drawer::drawTranslateFilledTriangle(Geom::TriangleF triangle,
		const Rgba& color) {
	bool suc = true;
	for(int8_t i = 0; i < 3; i++){
		triangle.vs[i] = translationPipeline(V3f(0, 0, 0), triangle.vs[i],
				V3f(1, 1, 1), V3f(0, 0, 0), suc);
	}
	drawFilledTriangle(triangle, color);
}

void Renderer::Drawer::drawModel(const MeshModel& model, Geom::V3f position,
		Geom::V3f scale, Geom::V3f rot) {
	for (size_t i=0; i < model.faces.size(); i++) {
		std::vector<int> face = model.faces[i];
		V3i mas[3];
		bool suc = true;
		for (int j=0; j<3; j++) {
			V3f res = translationPipeline(position, model.verts[face[j]], scale, rot, suc);
			mas[j] = V3i(res.x, res.y, res.z * 10000);
 		}
		Rgba color(0, 0, 0, 0);
		if(suc) {
			drawTriangle(Triangle(mas), color);
		}
	}
}


Geom::V3f Renderer::CameraView::translate(Geom::V3f v) {
	float projection_plane_z = 1.0;
	float right = std::tan(aw) * projection_plane_z;
	float left = -right;
	float top = std::tan(ah) * projection_plane_z;
	float bottom = -top;
	Matrix44f summary { MatrixFactory::translation(cen) * MatrixFactory::withRotation(r, up, f)* MatrixFactory::projection(left, right, top, bottom, near, far)};
	return V4f((V4f(v, 1).rowMatrix() * summary)[0]).norm();
}
