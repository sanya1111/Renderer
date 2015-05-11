#include "Renderer/Light.h"
using namespace Renderer;
using namespace Geom;

float Renderer::DiffuseLight::getForce(const Geom::V3f &vec)const {
	return vec.norm().scMul(direction) * force ;
}

float Renderer::AmbientLight::getForce() const {
	return force ;
}

float Renderer::SpecularLight::getForce(const Geom::V3f &normal_i,const Geom::V3f &to_eye_i) const {
	V3f normal = normal_i.norm();
	V3f to_eye = to_eye_i.norm();
	return force * pow((2 * normal.scMul(direction) * normal.scMul(to_eye) - direction.scMul(to_eye)), alpha);
}

float Renderer::Phong::getForce(const Geom::V3f &normal, const Geom::V3f &to_eye) {
	return a_kof * a.getForce() + b_kof * b.getForce(normal) + c_kof * c.getForce(normal, to_eye);
}
