#ifndef PIXELSTAGES_H_
#define PIXELSTAGES_H_

#include "Renderer/Geom.hpp"
#include "Renderer/Model.h"

namespace Renderer{

class Rgba;

class DefaultPixelStage{
	Geom::TriangleF4 * tr;
	Geom::TriangleF * other;
	Geom::V2<int> *last;
	Geom::V3f bc_clip;
	Texture * tex;
public:
	DefaultPixelStage(Texture &with) : tex(&with) {};
	void process(std::tuple<Geom::TriangleF4, Geom::TriangleF> &result);
	bool apply(Geom::V2<int> &pt);
	int getZ();
	Rgba getColor();
};

}

#endif /* PIXELSTAGES_H_ */
