#ifndef RASTERIZATORS_H_
#define RASTERIZATORS_H_

#include "Renderer/Geom.hpp"
#include "Renderer/Graphics.h"

namespace Renderer{

template<class PixelStage>
class DefaultRasterizator{
public:
	void draw(const Geom::TriangleXF<7> &tr, PixelStage pstage){

	}
};

}


#endif /* RASTERIZATORS_H_ */
