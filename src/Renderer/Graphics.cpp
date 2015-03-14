#include "Renderer/Graphics.h"

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
/*void Renderer::Graphics::fill(Buffer::DrawBuffer* buf,
		const Buffer::DrawBuffer::Rgb& color) {
	for (int32_t i = 0; i < buf->height; i++) {
		for (int32_t j = 0; j < buf->width; j++) {
			buf->at(i, j).set(color);
		}
	}
}

void Renderer::Graphics::draw_line(Buffer::DrawBuffer* buf,
		const Geom::Line<int32_t>& arg, const Buffer::DrawBuffer::Rgb& color) {
	Geom::Line<int32_t>  what = arg;
	bool swapped = false;
	if(std::abs(what.begin.x - what.end.x) < std::abs(what.begin.y - what.end.y)){
		swapped = true;
		std::swap(what.begin.x, what.begin.y);
		std::swap(what.end.x, what.end.y);
	}
	int32_t dx = -what.begin.x +  what.end.x;
	int32_t dy = -what.begin.y + what.end.y;
	int32_t delta_x = Geom::sign(dx);
	int32_t delta_y = Geom::sign(dy);
	int32_t every = 0;
	if(delta_y)
		every = std::max(1, abs(dx/ dy));
	int32_t cury = what.begin.y;
	for(int32_t i = what.begin.x, j = 0; i != what.end.x; i += delta_x, j++){
		if(every && j % every == 0){
			cury += delta_y;
		}
		if(!swapped){
			buf->at(i, cury).set(color);
		} else {
			buf->at(cury, i).set(color);
		}
	}
}

void Renderer::Graphics::draw_triangle(Buffer::DrawBuffer* buf,
		const Geom::Triangle<int32_t>& what,
		const Buffer::DrawBuffer::Rgb& color) {
	for(int8_t i = 0; i < 3; i++){
		draw_line(buf, what.lines[i], color);
	}
}
*/
