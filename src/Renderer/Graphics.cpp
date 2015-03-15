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
