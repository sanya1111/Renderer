#ifndef RAST_H_
#define RAST_H_
#include "Renderer/Geom.hpp"

namespace Renderer{

class DefaultRast{
	float  width, height;
	Geom::Matrix44f screen_ma;
	Geom::TriangleF4 saved_tr;
	float bleft, bright;
	float aty, toy;
	Geom::V2<float> dleft, dright;
	Geom::V2<float> p1, p2, p3, p4;
	bool now;
	void init(Geom::TriangleF4 );
	void changeT();
	void moveP();
	void change();
public:
	DefaultRast(int height,int width);
	void process(std::tuple<Geom::TriangleF4, Geom::TriangleF> &inp, bool &ret);
	bool have();
	Geom::V2<int> next();
};

}


#endif /* RAST_H_ */
