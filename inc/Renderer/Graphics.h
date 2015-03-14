#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "Renderer/Buffer.h"
#include "Renderer/Geom.hpp"

namespace Renderer {

//functions
	class Rgba{
	public:
		uint8_t a, r, g, b;
		Rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	};

	class Drawer{
	protected:
		Buffer * buf;
		void at(uint8_t * ptr, Rgba & color);
	public:
		void setBuffer(Buffer * buf	);
		void fill(Rgba &color);
		void drawPixel(const uint32_t &screen_x,const uint32_t &screen_y, Rgba & color);
//		void draw_line(Buffer::DrawBuffer * buf, const Geom::Line<int32_t> &what,
//		const Buffer::DrawBuffer::Rgb& color);
//		void draw_triangle(Buffer::DrawBuffer * buf, const Geom::Triangle<int32_t> &what,
//		const Buffer::DrawBuffer::Rgb & color);
	};

}

#endif /* GRAPHICS_H_ */
