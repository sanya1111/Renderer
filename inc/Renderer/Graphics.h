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
		Rgba() = default;
	};

	class Drawer{
	protected:
		Buffer * buf;
		void at(uint8_t * ptr,const Rgba & color);
	public:
		void setBuffer(Buffer * buf	);
		void fill(const Rgba &color);
		void drawPixel(const uint32_t &screen_x,const uint32_t &screen_y, const Rgba & color);
		void drawLine(Geom::Point2D<int32_t> beg, Geom::Point2D<int32_t> en, const Rgba & color);
		void drawTriangle(Geom::Triangle2D<int32_t> triangle, const Rgba & color);
	};

}

#endif /* GRAPHICS_H_ */
