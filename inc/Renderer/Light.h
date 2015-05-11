#ifndef LIGHT_H_
#define LIGHT_H_

#include "Renderer/Geom.hpp"

namespace Renderer{

class AmbientLight{
	float force;
public:
	AmbientLight() {}
	AmbientLight(float force) : force(force) {}
	float getForce()const;
};

class DiffuseLight{
	Geom::V3f source;
	float force;
public:
	DiffuseLight() {}
	DiffuseLight(const DiffuseLight&)=default;
	DiffuseLight(const Geom::V3f &source, float force) : source(source), force(force) {}
	float getForce(const Geom::V3f &norm, const Geom::V3f &pt)const;
};


class SpecularLight{
	float force;
	float alpha;
	Geom::V3f source;
public:
	SpecularLight() {}
	SpecularLight(const Geom::V3f &source, float force, float alpha) : source(source), force(force), alpha(alpha){}
	float getForce(const Geom::V3f &normal, const Geom::V3f &to_eye, const Geom::V3f &pt) const;
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
	Phong() {}
	float getForce(const Geom::V3f &normal, const Geom::V3f &to_eye, const Geom::V3f &pt);
};

}

#endif /* LIGHT_H_ */
