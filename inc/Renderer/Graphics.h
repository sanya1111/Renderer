#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "Renderer/Buffer.h"
#include "Renderer/Geom.hpp"
#include "Renderer/Model.h"
#include "Renderer/Camera.h"
#include "Renderer/BaseStages.h"
#include "Renderer/PixelStages.h"
#include "Renderer/VertexStages.h"

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


	class Drawer{
		Buffer * buf;
		Geom::Matrix44f world_to_screen;
		std::vector<int32_t> zbuffer;
		void at(uint8_t * ptr,const Rgba & color);
		bool inScreen(int32_t x, int32_t y);
		//views
		CameraView mainView;
		Geom::V3f toScreenTranslation(const Geom::V3f &pt);
		Geom::V4f toScreenTranslation2(const Geom::V4f &pt);
		void toScreenTranslation3(Geom::TriangleF4 &tr);
		Geom::V3f translationPipeline(const Geom::V3f &cen, Geom::V3f pt, const Geom::V3f &scale, const Geom::V3f &rot, bool &);
		Geom::V4f translationPipeline2(const Geom::V3f &cen, Geom::V3f pt, const Geom::V3f &scale, const Geom::V3f &rot, bool &success);
	public:
		template<class V>
		void LineToScreenBounds(V & begin, V & end, int ignored = 0);

		void fill(const Rgba &color);
		void fill2(const Rgba & color);

		void drawPixel(int32_t screen_x,int32_t screen_y, uint32_t h, Rgba color);
		void drawLine(Geom::V3i begin, Geom::V3i end, const Rgba & color);
		void drawLine2(Geom::V4i begin, Geom::V4i end, const Rgba & color);
		void drawLine3(Geom::VXi<6> begin, Geom::VXi<6> end, const Texture &tex);


		void drawTriangle(Geom::Triangle triangle, const Rgba & color);
		void drawTriangle2(Geom::Triangle4 triangle, const Rgba & color);
		void drawTriangle3(Geom::TriangleX<6> triangle, const Texture &tex);

		void drawFilledTriangle(Geom::Triangle triangle, const Rgba &color);
		void drawFilledTriangle2(Geom::Triangle4 triangle, const Rgba & color);
		void drawFilledTriangle3(Geom::TriangleX<6> triangle, const Texture &tex);
		void drawFilledTriangle4(Geom::TriangleX<6> triangle, const Texture &tex);
		void drawFilledTriangle5(Geom::TriangleXF<7> triangle, const Texture &tex);

		void drawBegin(Buffer * buf, const CameraView &mainView_);

		void drawTranslateTriangle(Geom::TriangleF triangle, const Rgba &color);

		void drawTranslateFilledTriangle(Geom::TriangleF triangle, const Rgba& color);

		void drawEnd();

		void drawModel(const MeshModel & model, Geom::V3f position, Geom::V3f scale, Geom::V3f rot);
		void drawModel2(const MeshModel & model, Geom::V3f position, Geom::V3f scale, Geom::V3f rot, Geom::V3f light_dir);
		void drawModel3(const MeshModel &model,  Geom::V3f position, Geom::V3f scale, Geom::V3f rot, Geom::V3f light_dir);
		void drawModel4(const MeshModel &model,  Geom::V3f position, Geom::V3f scale, Geom::V3f rot, Geom::V3f light_dir);

		//newwww
		template<class Inp, class BaseStage, class VertexStage, class PixelStage>
		void drawModel_new(Inp model, BaseStage bstage, VertexStage vstage, PixelStage pstage);
		template<class PixelStage>
		void draw(Geom::TriangleF4 &inp, PixelStage pstage);
		template<class PixelStage>
		void drawLine_new(Geom::V2<int> begin, Geom::V2<int> end, PixelStage pstage);
		Texture saveSnapshot();
		Drawer() {	}
	};
}




#endif /* GRAPHICS_H_ */
