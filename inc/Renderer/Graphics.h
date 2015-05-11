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
		std::vector<float> zbuffer;
		void at(uint8_t * ptr,const Rgba & color);
	public:
		void fill(const Rgba &color);
		void fill2(const Rgba & color);
		void drawPixel(int32_t screen_x,int32_t screen_y, float h, const Rgba &color);
		void drawBegin(Buffer * buf);
		void drawEnd();
		void cleanZ();
		Texture saveSnapshot();
		Drawer() {	}
	};

	template<class BaseStage, class VertexStage, class Rast, class PixelStage, class Painter>
	void drawModel(BaseStage &bstage, VertexStage &vstage, Rast &rast, PixelStage &pstage, Painter &painter) {
		bstage.start();
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
				Geom::V2<int> pt = rast.next();
				if(pstage.apply(pt)){
					painter.drawPixel(pt.x, pt.y, pstage.getZ(), pstage.getColor());
				}
			}
		}
	};
}




#endif /* GRAPHICS_H_ */
