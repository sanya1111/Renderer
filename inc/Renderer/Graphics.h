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
		std::vector<int32_t> zbuffer;
		void at(uint8_t * ptr,const Rgba & color);
		bool inScreen(int32_t x, int32_t y);
		//views
	public:
		void fill(const Rgba &color);
		void fill2(const Rgba & color);
		void drawPixel(int32_t screen_x,int32_t screen_y, uint32_t h, Rgba color);
		void drawBegin(Buffer * buf);
		void drawEnd();
		//newwww
		template<class BaseStage, class VertexStage, class Rast, class PixelStage>
		void drawModel_new(BaseStage &bstage, VertexStage &vstage, Rast &rast, PixelStage &pstage);
		Texture saveSnapshot();
		Drawer() {	}
	};
}




#endif /* GRAPHICS_H_ */
