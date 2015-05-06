#include "Renderer/Graphics.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Renderer/Log.h"
#include "Renderer/Model.h"
#include "Renderer/Utils.hpp"

using namespace Renderer;
using namespace Geom;
using namespace std;
//Helpers
static const float SCALE_01FLOAT_TO_INT = 1e5;

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


void toBounds(int &a, int l, int r){
	a = max(a, l);
	a = min(a, r);
}



template<class V>
struct LineStepper{
	V begin, end, current, error2, derror2, delta, step;
	int dx;

	LineStepper(const V &begin_, const V & end_, int8_t force_asix = -1, Drawer * dr = NULL, int ignored = 0) : begin(begin_), end(end_){
		if(dr){
			dr->LineToScreenBounds(begin, end, ignored);
		}
		if (force_asix == 0 || (force_asix != 1 &&  std::abs(begin[0] - end[0]) > std::abs(begin[1] - end[1]))) {
			step[0] = 1;
			if(end[0] < begin[0]){
				swap(begin, end);
			}
			dx = end[0] - begin[0];
		} else {
			step[1] = 1;
			if(end[1] < begin[1]){
				swap(begin, end);
			}
			dx = end[1] - begin[1];
		}

		FOR(i, V::num){
			if(step[i])
				continue;
			delta[i] = sign(end[i] - begin[i]);
			derror2[i] = std::abs(end[i] - begin[i]) * 2;
		}

		current = begin;
	}
	bool finish(){
		return (step[0] && current[0] >= end[0]) || (step[1] && current[1] >= end[1]);
	}

	void nextForce() {
		current = current + step;
		error2 = error2 + derror2;
		for(int i = 0; i < V::num; i++){
			if(error2[i] > dx && dx){
				int tim = ((error2[i]  - dx)/ (dx * 2)) + (error2[i] % (dx * 2) > 0);
				current[i] += delta[i] * tim;
				error2[i] -= dx * 2 * tim;
			}
		}
	}

	void next() {
		current = current + step;
		error2 = error2 + derror2;
		FOR(i, V::num){
			if(error2[i] > dx){
				current[i] += delta[i] ;
				error2[i] -= dx * 2 ;
			}
		}
	}
	V getP(){
		return current;
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

Geom::V4f Renderer::Drawer::toScreenTranslation2(const Geom::V4f &pt){
	Matrix44f transf{
			buf->height / 2.0f, 0, 0, 0,
			0, buf->width / 2.0f, 0, 0,
			0, 0, 1, 0,
			buf->height / 2.0f, buf->width / 2.0f, 0, 1};
	return (pt.rowMatrix() * transf)[0];
}

void Renderer::Drawer::toScreenTranslation3(Geom::TriangleF4 &tr){
	Geom::Matrix44f pt = Geom::Matrix44f{
		 tr[0].x, tr[0].y, tr[0].z, tr[0].w,
		 tr[1].x, tr[1].y, tr[1].z, tr[1].w,
		 tr[2].x, tr[2].y, tr[2].z, tr[2].w,
		 0,       0,       0,       0
	 } * world_to_screen;
	 FOR(i, 3) FOR(j, 4){
		 tr[i][j] = pt[i][j];
	 }
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

V4f Renderer::Drawer::translationPipeline2(const V3f &cen, V3f pt, const V3f &scale, const V3f &rot, bool &success){
//	pt.transform(cen, rot, scale);
	V4f ret = (V4f(pt, 1).rowMatrix() * Geom::MatrixFactory::transform(cen, rot, scale) * mainView.projection_matrix())[0];
//	ret.print();
//	V4f ret = mainView.projection2(pt);
	float w = fabs(ret.w);
	if(!inBounds(ret.x, -w, w) ||
	   !inBounds(ret.y, -w, w) ||
	   !inBounds(ret.z, -w, w)){
		success = false;
	}
	return toScreenTranslation2(ret);
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


void Renderer::Drawer::drawPixel(int32_t screen_x, int32_t screen_y, uint32_t h, Rgba color) {
	int32_t pos = screen_x * buf->width + screen_y;
	if(zbuffer[pos] > h){
		zbuffer[pos] = h;
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
		step.nextForce();
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
		step.nextForce();
		pt = step.getP();
		if(pt.w >= 0)
			drawPixel(pt.x, pt.y, pt.z, color * scale_int(pt.w));
		else{
			drawPixel(pt.x, pt.y, pt.z, Rgba(0, 0, 0, 0));
		}
	}
}

void Renderer::Drawer::drawLine3(Geom::VXi<6> begin, Geom::VXi<6> end,
		const Texture &tex) {
	LineToScreenBounds(begin, end);
	LineStepper<VXi<6>> step(begin, end);
	VXi<6> pt;
	while(!step.finish()){
		step.nextForce();
		pt = step.getP();
//		float alp = begin[0] != end[0] ? segmSolver(begin[0], end[0], pt[0]) : segmSolver(begin[1], end[1], pt[1]);
//		pt[4] = (segmPointer(begin[4], end[4], alp));
//		pt[5] = (segmPointer(begin[5], end[5], alp));
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

inline void Renderer::Drawer::drawTriangle3(Geom::TriangleX<6> triangle,
		const Texture& tex) {
	for(int8_t i = 0; i < 3; i++){
			drawLine3(triangle.vs[i], triangle.vs[(i + 1) % 3], tex);
	}
}

inline void Renderer::Drawer::drawFilledTriangle3(Geom::TriangleX<6> triangle, const Texture &tex){
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
		a.nextForce();
		b.nextForce();
	}
}

//void Renderer::Drawer::drawFilledTriangle4(Geom::Triangle triangle, const Rgba &color){
//	V3i bboxmin(buf->height - 1, buf->width - 1, 0);
//	V3i bboxmax(0, 0, 0);
//	V3i clamp = bboxmin;
//	for (int i = 0; i < 3; i++) {
//		for (int j = 0; j < 2; j++) {
//			bboxmin[j] = std::max(0, std::min(bboxmin[j], triangle.vs[i][j]));
//			bboxmax[j] = std::min(clamp[j],
//					std::max(bboxmax[j], triangle.vs[i][j]));
//		}
//	}
//	V3i P;
//	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
//		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
//			V3f bc_screen = barycentric(triangle, P);
//			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
//				continue;
//			drawPixel(P.x, P.y, 10, color);
//		}
//	}
//}


void Renderer::Drawer::drawFilledTriangle5(Geom::TriangleXF<7> triangle, const Texture &tex){
	V3i bboxmin(buf->height - 1, buf->width - 1, 0);
	V3i bboxmax(0, 0, 0);
	V3i clamp = bboxmin;
	V3f last;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0, std::min(bboxmin[j], (int)(triangle.vs[i][j] / triangle.vs[i][3])));
			bboxmax[j] = std::min(clamp[j],
					std::max(bboxmax[j], (int)(triangle.vs[i][j] / triangle.vs[i][3])));
		}
	}
	FOR(i, 3){
		FOR(j, 3){
			triangle.vs[i][j] /= triangle.vs[i][3];
		}
//		triangle.vs[i][3] = 1;
	}
	V3i P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			V3f bc_screen = barycentric(triangle, P);
			if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0)
				continue;
			V3f bc_clip = V3f(bc_screen.x/triangle.vs[0][3],
							  bc_screen.y/triangle.vs[1][3],
							  bc_screen.z/triangle.vs[2][3]);
			bc_clip = bc_clip * (1.0 / (bc_clip.x+bc_clip.y+bc_clip.z));
			VXf<6> pt;
			pt[0] = P.x;
			pt[1] = P.y;
//			drawPixel(pt[0], pt[1], 10, Rgba(255, 255, 255, 0));
//			continue;
			pt[2] = V3f(triangle.vs[0][2], triangle.vs[1][2], triangle.vs[2][2]).scMul(bc_clip);
			for(int i = 4; i < 7; i++){
				pt[i - 1] = V3f(triangle.vs[0][i], triangle.vs[1][i], triangle.vs[2][i]).scMul(bc_clip);
			}
			if(pt[3] >= 0){
				int c1 = pt[4] * tex.width;
				int c2 =  tex.height - pt[5] * tex.height;
				Rgba take = tex.at(c2, c1);
				drawPixel(pt[0], pt[1], scale_float(pt[2]), take * pt[3]);
			}
			else{
				drawPixel(pt[0], pt[1], pt[2], Rgba(0, 0, 0, 0));
			}
		}
	}
}
void Renderer::Drawer::drawFilledTriangle4(Geom::TriangleX<6> triangle, const Texture &tex){
//	V3i bboxmin(buf->height - 1, buf->width - 1, 0);
//	V3i bboxmax(0, 0, 0);
//	V3i clamp = bboxmin;
//	for (int i = 0; i < 3; i++) {
//		for (int j = 0; j < 2; j++) {
//			bboxmin[j] = std::max(0, std::min(bboxmin[j], triangle.vs[i][j]));
//			bboxmax[j] = std::min(clamp[j],
//					std::max(bboxmax[j], triangle.vs[i][j]));
//		}
//	}
//	V3i P;
//	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
//		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
////			V3f bc_screen = barycentric(triangle, P);
//			bc_screen = bc_screen * (1.0 / (bc_screen.x + bc_screen.y + bc_screen.y));
//			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
//				continue;
//			VXi<6> pt;
//			pt[0] = P.x;
//			pt[1] = P.y;
//			FORN(i, 2, 6){
//				pt[i] = bc_screen.scMul(V3f(triangle.vs[0][i], triangle.vs[1][i], triangle.vs[2][i]));
// 			}
//			if(pt[3] >= 0){
//				int c1 = scale_int(pt[4]) * tex.width;
//				int c2 =  tex.height - scale_int(pt[5]) * tex.height;
//				Rgba take = tex.at(c2, c1);
//				drawPixel(pt[0], pt[1], pt[2], take * scale_int(pt[3]));
//			}
//			else{
//				drawPixel(pt[0], pt[1], pt[2], Rgba(0, 0, 0, 0));
//			}
//		}
//	}
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
		a.nextForce();
		b.nextForce();
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
		a.nextForce();
		b.nextForce();
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
	zbuffer.assign(buf->size, INF);
	mainView = mainView_;
	world_to_screen = {
				buf->height / 2.0f, 0, 0, 0,
				0, buf->width / 2.0f, 0, 0,
				0, 0, 1, 0,
				buf->height / 2.0f, buf->width / 2.0f, 0, 1
	};
}






void Renderer::Drawer::drawEnd() {
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
//			V3f res = translationPipeline2(position, model.verts[model.faces[i][j]], scale, rot, suc).norm();
			V3f res = translationPipeline(position, model.verts[model.faces[i][j]], scale, rot, suc);
			mas[j] = V3i(res.x, res.y, scale_float(res.z));
			inten[j] =  scale_float((mainView.moveToCam((const V3f )model.normals[model.faces[i][j]].norm()).transform(position, rot, scale)).norm().scMul(light_dir));
			u[j] = scale_float(model.verts_tex[model.faces[i][j]][0]);
			v[j] = scale_float(model.verts_tex[model.faces[i][j]][1]);
		}
		FOR(j, 3){
			inten[j] = max(inten[j], 0);
		}
		Rgba color(255, 255, 255, 0);
		if(suc) {
			drawFilledTriangle3(makeTriangle6(Triangle(mas), inten, u, v), model.mats[model.mat_index].col[1][0]);
		}
	}
}

void Renderer::Drawer::drawModel4(const MeshModel& model, Geom::V3f position,
		Geom::V3f scale, Geom::V3f rot, Geom::V3f light_dir) {
	light_dir = light_dir.norm();
	for (size_t i=0; i < model.faces.size(); i++) {
		V4f mas[3];
		V3f inten;
		V3f u;
		V3f v;
		bool suc = true;
		for (int j=0; j<3; j++) {
			mas[j] = translationPipeline2(position, model.verts[model.faces[i][j]], scale, rot, suc);
			inten[j] =  mainView.moveToCam((const V3f )model.normals[model.faces[i][j]].norm()).transform(position, rot, scale).norm().scMul(light_dir.norm());
			u[j] = model.verts_tex[model.faces[i][j]][0];
			v[j] = model.verts_tex[model.faces[i][j]][1];
		}
		FOR(j, 3){
			inten[j] = max(inten[j], 0.0f);
		}
		Rgba color(255, 255, 255, 0);
		if(suc) {
			drawFilledTriangle5(makeTriangle7(Triangle_<V4f>(mas), inten, u, v), model.mats[model.mat_index].col[1][0]);
		}
	}
}





Rgba Renderer::Rgba::operator *(const float& intensity) const{
	return Rgba((float)r * intensity, (float)g * intensity, (float)b * intensity, a);
}



Texture Renderer::Drawer::saveSnapshot() {
	if(buf->format_desc[buf->format_desc_entry].bpp == 32){
		shared_ptr<uint8_t> new_buf(new uint8_t[buf->size / 4 * 3]);
		FOR(i, buf->height) FOR(j, buf->width){
			uint8_t * ptr = buf->map + i * buf->stride + j * 4;
			FOR(k, 3){
				new_buf.get()[i * buf->width * 3 + j * 3 + k] = ptr[2 - k];
			}
		}
		return Texture(new_buf, buf->width, buf->height, 3);
	}
}



template<class BaseStage, class VertexStage, class PixelStage>
void Renderer::Drawer::drawModel_new(BaseStage &bstage, VertexStage &vstage, PixelStage &pstage) {
	while(bstage.have()){
		bool ret = true;
		typename BaseStage::result res_base = bstage.process(ret);
		if(!ret)
			continue;
		typename VertexStage::result res_vertex = vstage.process(res_base, ret);
		if(!ret)
			continue;
		toScreenTranslation3(get<0>(res_vertex));
		pstage.process(res_vertex);
		draw(get<0>(res_vertex), pstage);
	}
}

template<class PixelStage>
inline void Renderer::Drawer::draw(Geom::TriangleF4 &tr, PixelStage &pstage) {
//	DEB("BEG\n");
	V2<int> cnt;
	FOR(i, 3){
		V3f tpt = tr[i].norm();
		if(between(0.0f, tpt.x, (float)buf->height))
			cnt.x++;
		if(between(0.0f, tpt.y, (float)buf->width))
			cnt.y++;
	}
	if(!cnt.x|| !cnt.y)
		return;
//	DEB("EN\n");
	V2<int> triangle[3];
	FOR(i, 3){
		int a = tr[i][0];
		int b = tr[i][1];
		triangle[i] = V2<int>(a, b );
	}
	sort(triangle, triangle + 3);
	LineStepper<V2<int>> a(triangle[0], triangle[2], 0, this, 14);
	LineStepper<V2<int>> b(triangle[0], triangle[1], 0, this, 14);
	V2<int> pa, pb;
	while(!a.finish()){
		pa = a.getP();
		if(pa[0] == triangle[1][0]){
			b = LineStepper<V2<int>>(triangle[1], triangle[2], 0, this,  14);
		}
		pb = b.getP();
		drawLine_new(pa, pb, pstage);
		a.nextForce();
		b.nextForce();
	}
}

template<class PixelStage>
inline void Renderer::Drawer::drawLine_new(Geom::V2<int> begin, Geom::V2<int> end, PixelStage &pstage){
//	V2<int> begin(begin_t.x, begin_t.y);
//	V2<int> end(end_t.x, end_t.y);
	LineToScreenBounds(begin, end);
	LineStepper<V2<int>> step(begin, end);
	V2<int> pt;
	while(!step.finish()){
		step.next();
		pt = step.getP();
		if(pstage.apply(pt)){
			drawPixel(pt.x, pt.y, pstage.getZ(), pstage.getColor());
		}
	}
}

template void Renderer::Drawer::drawModel_new<Renderer::ModelStage, Renderer::DefaultVertexStage, Renderer::DefaultPixelStage>(Renderer::ModelStage&, Renderer::DefaultVertexStage&, Renderer::DefaultPixelStage&);
