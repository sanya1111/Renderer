#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "Renderer/Buffer.h"
#include "Renderer/Geom.hpp"
#include "Renderer/Model.h"

#include <string>
#include <vector>
namespace Renderer {

	class Rgba{
	public:
		uint8_t a, r, g, b;
		Rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		Rgba operator*(const float & intensity)const;
		Rgba() = default;
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
		Buffer * buf;
		std::vector<int32_t> zbuffer,
							 check;
		int32_t current_draw;
		bool zbufferAt(const int32_t &x, const int32_t &y, const int32_t &h);
		void at(uint8_t * ptr,const Rgba & color);
		bool inScreen(int32_t x, int32_t y);
		//views
		CameraView mainView;
		Geom::V3f toScreenTranslation(Geom::V3f pt);
		Geom::V3f translationPipeline(Geom::V3f cen, Geom::V3f pt, Geom::V3f scale, Geom::V3f rot, bool &);
	public:
		template<class V>
		void LineToScreenBounds(V & begin, V & end, int ignored = 0);
		void fill(const Rgba &color);
		void fill2(const Rgba & color);
		void drawPixel(const int32_t &screen_x,const int32_t &screen_y, const uint32_t &h, const Rgba & color);
		void drawLine(Geom::V3i begin, Geom::V3i end, const Rgba & color);
		void drawLine2(Geom::V4i begin, Geom::V4i end, const Rgba & color);
		void drawTriangle(Geom::Triangle triangle, const Rgba & color);
		void drawTriangle2(Geom::Triangle4 triangle, const Rgba & color);
		void drawFilledTriangle(Geom::Triangle triangle, const Rgba &color);
		void drawFilledTriangle2(Geom::Triangle4 triangle, const Rgba & color);
		void drawBegin(Buffer * buf, const CameraView &mainView_);
		void drawTranslateTriangle(Geom::TriangleF triangle, const Rgba &color);
		void drawTranslateFilledTriangle(Geom::TriangleF triangle, const Rgba& color);
		void drawEnd();
		void drawModel(const MeshModel & model, Geom::V3f position, Geom::V3f scale, Geom::V3f rot);
		void drawModel2(const MeshModel & model, Geom::V3f position, Geom::V3f scale, Geom::V3f rot, Geom::V3f light_dir);
		Drawer() : current_draw(0) {}
	};

}

#endif /* GRAPHICS_H_ */
