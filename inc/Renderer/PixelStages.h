#ifndef PIXELSTAGES_H_
#define PIXELSTAGES_H_

#include "Renderer/Geom.hpp"
#include "Renderer/Model.h"
#include "Renderer/Light.h"
namespace Renderer{

class Rgba;

class DefaultPixelStage{
protected:
	Geom::TriangleF4 * tr;
	Geom::TriangleF * other;
	Geom::V3f bc_clip;
	Texture * tex;
public:
	DefaultPixelStage(Texture &with) : tex(&with) {
	};
	DefaultPixelStage() {}
	void save(std::tuple<Geom::TriangleF4, Geom::TriangleF> &result);
	bool apply(Geom::V2<int> &pt);
	float getZ();
	float getzz();
	Geom::V2<float> getxy();
	Geom::V2<float> getXY();
	Rgba getColor();
	float getW();
};

}

#endif /* PIXELSTAGES_H_ */
