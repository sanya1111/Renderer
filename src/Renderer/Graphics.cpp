#include "Renderer/Graphics.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Renderer/Log.h"
#include "Renderer/Model.h"
#include "Renderer/Utils.hpp"
#include "Renderer/Rast.h"

using namespace Renderer;
using namespace Geom;
using namespace std;
//Helpers
static const float SCALE_01FLOAT_TO_INT = 1e5;

static inline void * mem(void *s, int32_t c, size_t count){
	 asm volatile("rep\n\t"
			 	 "stosl"
			 :
			 : "a" (c), "D" (s), "c" (count)
			 : "memory");
	 return s;
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

void Renderer::Drawer::fill2(const Rgba& color) {
	if(buf->format_desc[buf->format_desc_entry].bpp == 32){
		int32_t col = ((color.a) << 24) | ((color.r) << 16) |
				   ((color.g) << 8) | (color.b);
		mem(buf->map, col, buf->size / sizeof(int32_t));
	}
}

void Renderer::Drawer::drawBegin(Buffer * buf) {
	this->buf = buf;
	zbuffer.assign(buf->size, INF);
}


void Renderer::Drawer::drawEnd() {
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



template<class BaseStage, class VertexStage, class Rast, class PixelStage>
void Renderer::Drawer::drawModel_new(BaseStage &bstage, VertexStage &vstage, Rast &rast, PixelStage &pstage) {
	while(bstage.have()){
		bool ret = true;
		typename BaseStage::result res_base = bstage.process(ret);
		if(!ret)
			continue;
		typename VertexStage::result res_vertex = vstage.process(res_base, ret);
		if(!ret)
			continue;
		rast.process(res_vertex, ret);
		if(!ret)
			continue;
		pstage.save(res_vertex);
		while(rast.have()){
			bool need = true;
			V2<int> pt = rast.next();
			if(pstage.apply(pt)){
				drawPixel(pt.x, pt.y, pstage.getZ(), pstage.getColor());
			}
		}
	}
}


template void Renderer::Drawer::drawModel_new<Renderer::ModelStage, Renderer::DefaultVertexStage, Renderer::DefaultRast, Renderer::DefaultPixelStage>(Renderer::ModelStage&, Renderer::DefaultVertexStage&, Renderer::DefaultRast&, Renderer::DefaultPixelStage&);
