#ifndef LIGHT_H_
#define LIGHT_H_

#include "Renderer/Geom.hpp"

namespace Renderer{

class AmbientLight{
	float force;
public:
	AmbientLight(float mat_kof, float force) : force(mat_kof * force) {}
	float getForce();
};
class DiffuseLight{
	Geom::V3f direction;
	float force;
public:
	DiffuseLight(Geom::V3f direction, float force) : direction(direction.norm()), force(force) {}
	float getForce(Geom::V3f vec, float mat_kof);
};


class SpecularLight{
	float force;
	Geom::V3f direction;
public:
	SpecularLight(Geom::V3f direction, float force) : direction(direction.norm()), force(force) {}
	Geom::V3f nextBeam();
	float getForce(Geom::V3f normal, Geom::V3f to_eye, float alpha, float mat_kof);
};

class Phong{
	AmbientLight &a;
	DiffuseLight &b;
	SpecularLight &c;
	float a_kof, b_kof, c_kof;
public:
	Phong(AmbientLight &a, DiffuseLight &b, SpecularLight &c, float a_kof, float b_kof, float c_kof) :
		a(a), b(b), c(c),
		a_kof(a_kof),
		b_kof(b_kof),
		c_kof(c_kof){}
	float getForce(Geom::V3f normal, Geom::V3f to_eye, float dif_kof, float alpha, float spec_kof);
};

}

#endif /* LIGHT_H_ */
