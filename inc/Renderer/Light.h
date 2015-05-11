#ifndef LIGHT_H_
#define LIGHT_H_

#include "Renderer/Geom.hpp"

namespace Renderer{

class AmbientLight{
	float force;
public:
	AmbientLight(float force) : force(force) {}
	float getForce()const;
};

class DiffuseLight{
	Geom::V3f direction;
	float force;
public:
	DiffuseLight(const DiffuseLight&)=default;
	DiffuseLight(Geom::V3f source, float force) : direction(source), force(force) {}
	float getForce(Geom::V3f vec)const;
};


class SpecularLight{
	float force;
	float alpha;
	Geom::V3f direction;
public:
	SpecularLight(Geom::V3f direction, float force, float alpha) : direction(direction.norm()), force(force), alpha(alpha){}
	Geom::V3f nextBeam();
	float getForce(Geom::V3f normal, Geom::V3f to_eye) const;
};

class Phong{
	AmbientLight a;
	DiffuseLight b;
	SpecularLight c;
	float a_kof, b_kof, c_kof;
public:
	Phong(const AmbientLight &a, const DiffuseLight &b, const SpecularLight &c, float a_kof, float b_kof, float c_kof) :
		a(a), b(b), c(c),
		a_kof(a_kof),
		b_kof(b_kof),
		c_kof(c_kof){}
	Phong(const Phong&) = default;
	float getForce(Geom::V3f normal, Geom::V3f to_eye);
};

}

#endif /* LIGHT_H_ */
