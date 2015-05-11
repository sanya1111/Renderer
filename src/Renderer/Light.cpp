#include "Renderer/Light.h"
using namespace Renderer;
using namespace Geom;

float Renderer::DiffuseLight::getForce(const Geom::V3f &normal, const Geom::V3f &pt)const {
	V3f direction = (source - pt).norm();
	return normal.norm().scMul(direction) * force ;
}

float Renderer::AmbientLight::getForce() const {
	return force ;
}

float Renderer::SpecularLight::getForce(const Geom::V3f &normal_i,const Geom::V3f &to_eye_i, const Geom::V3f &pt) const {
	V3f direction = (source - pt).norm();
	V3f normal = normal_i.norm();
	V3f to_eye = to_eye_i.norm();
	return force * pow((2 * normal.scMul(direction) * normal.scMul(to_eye) - direction.scMul(to_eye)), alpha);
}

float Renderer::Phong::getForce(const Geom::V3f &normal, const Geom::V3f &to_eye, const Geom::V3f &pt) {
//	DEB("%f \n", b.getForce(normal, pt));
	return a_kof * a.getForce() + b_kof * b.getForce(normal, pt) + c_kof * c.getForce(normal, to_eye, pt);
}
