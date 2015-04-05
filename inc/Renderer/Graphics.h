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

	class MeshModel{
			std::vector<Geom::V3f> verts;
			std::vector<std::vector<int> > faces;
		public:
			void loadObj(const char *filename);
		friend class Drawer;
	};

	class CameraView{
		Geom::V3f cen,
			up,
			r,
			f;
		float aw, ah;
		float near, far;
	public:
		CameraView & operator=(const CameraView & other) = default;
		CameraView() {}
		CameraView(Geom::V3f cen, Geom::V3f up1, Geom::V3f f, float angle, int32_t width, int32_t height, float near, float far);
		Geom::V3f translate(Geom::V3f v);
	};

	class Drawer{
	protected:
		//screen : buf, zbuf
		Buffer * buf;
		std::vector<int32_t> zbuffer,
							 check;
		int32_t current_draw;
		bool zbufferAt(const int32_t &x, const int32_t &y, const int32_t &h);
		void at(uint8_t * ptr,const Rgba & color);
		bool inScreen(int32_t x, int32_t y);
		//views
		CameraView mainView;
		Geom::V3f toScreen(Geom::V3f pt);
		Geom::V3f translate(Geom::V3f cen, Geom::V3f pt, Geom::V3f scale, Geom::V3f rot, bool &);
	public:
		void translateLineToScreenBounds(Geom::V3i & begin, Geom::V3i & end, bool swapped);
		void fill(const Rgba &color);
		void fill2(const Rgba & color);
		void drawPixel(const int32_t &screen_x,const int32_t &screen_y, const uint32_t &h, const Rgba & color);
		void drawLine(Geom::V3i begin, Geom::V3i end, const Rgba & color, bool swapped = false);
		void drawTriangle(Geom::Triangle triangle, const Rgba & color);
		void drawFilledTriangle(Geom::Triangle triangle, const Rgba &color);
		void drawBegin(Buffer * buf, const CameraView &mainView_);
		void drawTranslateTriangle(Geom::TriangleF triangle, const Rgba &color);
		void drawTranslateFilledTriangle(Geom::TriangleF triangle, const Rgba& color);
		void drawEnd();
		void drawModel(const MeshModel & model, Geom::V3f position, Geom::V3f scale, Geom::V3f rot);
		Drawer() : current_draw(0) {}
	};

}

#endif /* GRAPHICS_H_ */
