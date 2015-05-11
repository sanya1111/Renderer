#ifndef RAST_H_
#define RAST_H_
#include "Renderer/Geom.hpp"


//Get Triangle from world, translale to screen, iterate and output points

namespace Renderer{

class DefaultRast{
	float  width, height;
	int32_t start_x,
		finish_x,
		x,
		it_y,
		bound_y;
	float y1, y2,
		add1,
		add2,
		add3;
	bool half;
	Geom::V2<float> mid;
	Geom::V3f delta1, delta2, delta3;
	Geom::Matrix44f screen_ma;
	void init(Geom::TriangleF4, bool &ret );
	void convert(Geom::TriangleF4 &, bool &ret);
	void fix();
	void fix_y();
public:
	DefaultRast(int height,int width);
	void process(std::tuple<Geom::TriangleF4, Geom::TriangleF> &inp, bool &ret);
	bool have();
	Geom::V2<int32_t> next();
};

}


#endif /* RAST_H_ */
