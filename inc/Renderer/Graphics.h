#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "Renderer/Buffer.h"
#include "Renderer/Geom.hpp"

#include <vector>
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
		std::vector<int32_t> zbuffer,
							 check;
		int32_t current_draw;
		void at(uint8_t * ptr,const Rgba & color);
	public:
		void setBuffer(Buffer * buf	);
		void fill(const Rgba &color);
		void fill2(const Rgba & color);
		void drawPixel(const uint32_t &screen_x,const uint32_t &screen_y, const uint32_t &h, const Rgba & color);

		void drawLine(Geom::Point2D<int32_t> , Geom::Point2D<int32_t> , const Rgba & color);
		void drawTriangle(Geom::Triangle2D<int32_t> triangle, const Rgba & color);
		void drawFilledTriangle(Geom::Triangle2D<int32_t> triangle, const Rgba &color);
		void drawMash(const char *filename, const Rgba & color);
		void drawBegin();
		void drawEnd();
		Drawer() : current_draw() {}
	};

	/*
	class Model{
		std::vector<Geom::Point3D<float>> verts;
		std::vector<std::vector<int>> triange_pts;
	public:
		void load(const std::string &s);

	};
	*/
}

#endif /* GRAPHICS_H_ */
