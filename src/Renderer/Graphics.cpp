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
static const float SCALE_01FLOAT_TO_INT = 1e3;

static inline int scale_float(float a){
	return a * SCALE_01FLOAT_TO_INT;
}

static inline float scale_int(int a){
	return (float)a / SCALE_01FLOAT_TO_INT;
}

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

template<class V>
struct LineStepper{
	V begin, end, current, error2, derror2, delta;
	int dx;
	bool swapped;

	LineStepper(const V &begin_, const V & end_, int8_t force_asix = -1, Drawer * dr = NULL, int ignored = 0) : begin(begin_), end(end_){
		if(dr){
			dr->LineToScreenBounds(begin, end, ignored);
		}
		swapped = false;
		if (force_asix != 0 && (force_asix == 1 ||  std::abs(begin[0] - end[0]) < std::abs(begin[1] - end[1]))) {
			std::swap(begin[0], begin[1]);
			std::swap(end[0], end[1]);
			swapped = true;
		}
		if (begin[0] > end[0]) {
			std::swap(begin, end);
		}

		for(int i = 0; i < V::num; i++){
					delta[i] = derror2[i] = error2[i] = 0;
		}

		for(int i = 1; i < V::num; i++){
				delta[i] = sign(end[i] - begin[i]);
				derror2[i] = std::abs(end[i] - begin[i]) * 2;
		}

		current = begin;
		dx = end[0] - begin[0];
	}
	bool finish(){
		return current[0] >= end[0];
	}

	void next(){
		current[0]++;
		error2 = error2 + derror2;
		for(int i = 1; i < V::num; i++){
			if(error2[i] > dx){
				int tim = ((error2[i]  - dx)/ (dx * 2)) + (error2[i] % (dx * 2 ) > 0);
				current[i] += delta[i] * tim;
				error2[i] -= dx * 2 * tim;
			}
		}
	}
	V getP(){
		V ret = current;
		if(swapped) {
			swap(ret[0], ret[1]);
		}
		return ret;
	}
};


Geom::V3f Renderer::Drawer::toScreenTranslation(const Geom::V3f &pt){
	Matrix44f transf{
		buf->height / 2.0f, 0, 0, 0,
		0, buf->width / 2.0f, 0, 0,
		0, 0, 1, 0,
		buf->height / 2.0f, buf->width / 2.0f, 0, 1};
	return (V4f(pt, 1).rowMatrix() * transf)[0];
}



V3f Renderer::Drawer::translationPipeline(const V3f &cen, V3f pt, const V3f &scale, const V3f &rot, bool &success){
	pt.transform(cen, rot, scale);
	pt = mainView.projection(pt);
	if(!inBounds(pt.x, -1.0, 1.0) ||
	   !inBounds(pt.y, -1.0, 1.0) ||
	   !inBounds(pt.z, -1.0, 1.0)){
		success = false;
	}
	pt = toScreenTranslation(pt);

	return pt;
}

bool Renderer::Drawer::inScreen(int32_t x, int32_t y){
	return inBounds(x, 0, buf->height - 1) &&
		   inBounds(y, 0, buf->width - 1 );
}

template<class V>
void Renderer::Drawer::LineToScreenBounds(V & begin, V & end, int ignored ){

	vector<int> bounds_mi = { 0, 0, 0, 0, 0, 0};
	vector<int> bounds_ma = { (int)buf->height - 1, (int)buf->width - 1, (int)INF, (int)INF, (int) INF, (int) INF};
	V nbegin = begin, nend = end;
	for(int i = 0; i < V::num; i++){
		if(ignored &(1 <<i))
			continue;
		toBounds(nbegin[i], bounds_mi[i], bounds_ma[i]);
		toBounds(nend[i], bounds_mi[i], bounds_ma[i]);
		float a = segmSolver(begin[i], end[i], nbegin[i]);
		float b = segmSolver(begin[i], end[i], nend[i]);
		for(int j= 0; j < V::num; j++){
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
	LineStepper<V3i> step(begin, end);
	V3i pt;
	while(!step.finish()){
		step.next();
		pt = step.getP();
		drawPixel(pt.x, pt.y, pt.z, color);
	}
}

void Renderer::Drawer::drawLine2(Geom::V4i begin, Geom::V4i end,
		const Rgba& color) {
	LineToScreenBounds(begin, end);
	LineStepper<V4i> step(begin, end);
	V4i pt;
	while(!step.finish()){
		step.next();
		pt = step.getP();
		if(pt.w >= 0)
			drawPixel(pt.x, pt.y, pt.z, color * scale_int(pt.w));
		else{
			drawPixel(pt.x, pt.y, pt.z, Rgba(0, 0, 0, 0));
		}
	}
}

template<class Texture>
void Renderer::Drawer::drawLine3(Geom::VXi<6> begin, Geom::VXi<6> end,
		Texture &tex) {
	LineToScreenBounds(begin, end);
	LineStepper<VXi<6>> step(begin, end);
	VXi<6> pt;
	while(!step.finish()){
		step.next();
		pt = step.getP();
		if(pt[3] >= 0){
			int c1 = scale_int(pt[4]) * tex.width;
			int c2 =  tex.height - scale_int(pt[5]) * tex.height;
			Rgba take = tex.at(c2, c1);
			drawPixel(pt[0], pt[1], pt[2], take * scale_int(pt[3]));
		}
		else{
			drawPixel(pt[0], pt[1], pt[2], Rgba(0, 0, 0, 0));
		}
	}
}

void Renderer::Drawer::drawTriangle(Triangle triangle,
		const Rgba& color) {
	for(int8_t i = 0; i < 3; i++){
			drawLine(triangle.vs[i], triangle.vs[(i + 1) % 3], color);
	}
}

void Renderer::Drawer::drawTriangle2(Geom::Triangle4 triangle,
		const Rgba& color) {
	for(int8_t i = 0; i < 3; i++){
			drawLine2(triangle.vs[i], triangle.vs[(i + 1) % 3], color);
	}
}

template<class Texture>
inline void Renderer::Drawer::drawTriangle3(Geom::TriangleX<6> triangle,
		Texture& tex) {
	for(int8_t i = 0; i < 3; i++){
			drawLine3(triangle.vs[i], triangle.vs[(i + 1) % 3], tex);
	}
}

template<class Texture>
inline void Renderer::Drawer::drawFilledTriangle3(Geom::TriangleX<6> triangle, Texture &tex){
	std::sort(triangle.vs, triangle.vs + 3);
	drawTriangle3(triangle, tex);
	LineStepper<VXi<6>> a(triangle.vs[0], triangle.vs[2], 0, this, 14);
	LineStepper<VXi<6>> b(triangle.vs[0], triangle.vs[1], 0, this, 14);
	VXi<6> pa, pb;
	while(!a.finish()){
		pa = a.getP();
		if(pa[0] == triangle.vs[1][0]){
			b = LineStepper<VXi<6>>(triangle.vs[1], triangle.vs[2], 0, this,  14);
		}
		pb = b.getP();
		drawLine3(pa, pb, tex);
		a.next();
		b.next();
	}
}

void Renderer::Drawer::drawFilledTriangle2(	Geom::Triangle4 triangle,
		const Rgba& color) {
	std::sort(triangle.vs, triangle.vs + 3);
	drawTriangle2(triangle, color);
	LineStepper<V4i> a(triangle.vs[0], triangle.vs[2], 0, this, 14);
	LineStepper<V4i> b(triangle.vs[0], triangle.vs[1], 0, this, 14);
	V4i pa, pb;
	while(!a.finish()){
		pa = a.getP();
		if(pa.x == triangle.vs[1].x){
			b = LineStepper<V4i>(triangle.vs[1], triangle.vs[2], 0, this,  14);
		}
		pb = b.getP();
		drawLine2(pa, pb, color);
		a.next();
		b.next();
	}
}

void Renderer::Drawer::drawFilledTriangle(Triangle triangle,
		const Rgba& color) {
	drawTriangle(triangle, color);
	std::sort(triangle.vs, triangle.vs + 3);
	LineStepper<V3i> a(triangle.vs[0], triangle.vs[2], 0, this, 6);
	LineStepper<V3i> b(triangle.vs[0], triangle.vs[1], 0, this, 6);
	V3i pa, pb;
	while(!a.finish()){
		pa = a.getP();
		if(pa.x == triangle.vs[1].x){
			b = LineStepper<V3i>(triangle.vs[1], triangle.vs[2], 0, this,  6);
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



Renderer::CameraView::CameraView(Geom::V3f cen_, Geom::V3f up1_, Geom::V3f f_, float angle_, int32_t width_, int32_t height_, float near_, float far_) :
		cen(cen_) , f(f_), aw(angle_), near(near_), far(far_){

	this->r = up1_.vMul(this->f);
	this->up = this->f.vMul(this->r);

	this->f = this->f.norm();
	this->up = this->up.norm();
	this->r = this->r.norm();

	ah = atan(tan(aw) * height_ / width_);

	float projection_plane_z = 1.0;
	float right = std::tan(aw) * projection_plane_z;
	float left = -right;
	float top = std::tan(ah) * projection_plane_z;
	float bottom = -top;
	move_ma = MatrixFactory::translation(cen) * MatrixFactory::withRotation(r, up, f);
	projection_ma = move_ma * MatrixFactory::projection(left, right, top, bottom, near, far);
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
			mas[j] = V3i(res.x, res.y, scale_float(res.z) );
 		}
		Rgba color(255, 255, 255, 0);
		if(suc) {
			drawFilledTriangle(Triangle(mas), color);
		}
	}
}


void Renderer::Drawer::drawModel2(const MeshModel& model, Geom::V3f position,
		Geom::V3f scale, Geom::V3f rot, Geom::V3f light_dir) {
	light_dir = light_dir.norm();
	for (size_t i=0; i < model.faces.size(); i++) {
		V3i mas[3];
		V3i inten;
		bool suc = true;
		for (int j=0; j<3; j++) {
			V3f res = translationPipeline(position, model.verts[model.faces[i][j]], scale, rot, suc);
			mas[j] = V3i(res.x, res.y, scale_float(res.z));
			inten[j] =  scale_float((mainView.moveToCam((const V3f )model.normals[model.faces[i][j]].norm()).transform(position, rot, scale)).norm().scMul(light_dir));

		}
		Rgba color(255, 255, 255, 0);
		if(suc) {
			drawFilledTriangle2(makeTriangle4(Triangle(mas), inten), color);
		}
	}
}

void Renderer::Drawer::drawModel3(const MeshModel& model, Geom::V3f position,
		Geom::V3f scale, Geom::V3f rot, Geom::V3f light_dir) {
	light_dir = light_dir.norm();
	for (size_t i=0; i < model.faces.size(); i++) {
		V3i mas[3];
		V3i inten;
		V3i u;
		V3i v;
		bool suc = true;
		for (int j=0; j<3; j++) {
			V3f res = translationPipeline(position, model.verts[model.faces[i][j]], scale, rot, suc);
			mas[j] = V3i(res.x, res.y, scale_float(res.z));
			inten[j] =  scale_float((mainView.moveToCam((const V3f )model.normals[model.faces[i][j]].norm()).transform(position, rot, scale)).norm().scMul(light_dir));
			u[j] = scale_float(model.verts_tex[model.faces[i][j]][0]);
			v[j] = scale_float(model.verts_tex[model.faces[i][j]][1]);
		}
		Rgba color(255, 255, 255, 0);
		if(suc) {
			drawFilledTriangle3(makeTriangle6(Triangle(mas), inten, u, v), model.mats[model.mat_index].col[1][0]);
		}
	}
}



Geom::V3f Renderer::CameraView::moveToCam(const Geom::V3f &v) {
	return V4f((V4f(v, 1).rowMatrix() * move_ma)[0]).norm();
}
Rgba Renderer::Rgba::operator *(const float& intensity) const{
	return Rgba((float)r * intensity, (float)g * intensity, (float)b * intensity, a);
}

Geom::V3f Renderer::CameraView::projection(const Geom::V3f& v) {
	return V4f((V4f(v, 1).rowMatrix() * projection_ma)[0]).norm();

}

