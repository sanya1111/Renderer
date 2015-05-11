#include "Renderer/Light.h"
using namespace Renderer;
using namespace Geom;

float Renderer::DiffuseLight::getForce(Geom::V3f vec)const {
	return vec.norm().scMul(direction) * force ;
}

float Renderer::AmbientLight::getForce() const {
	return force ;
}

Geom::V3f Renderer::SpecularLight::nextBeam() {

}

//\vec{R}\cdot\vec{V}=2(\vec{N}\cdot\vec{L})(\vec{N}\cdot\vec{V})-(\vec{L}\cdot\vec{V})


float Renderer::SpecularLight::getForce(Geom::V3f normal, Geom::V3f to_eye) const {
	normal = normal.norm();
	to_eye = to_eye.norm();
	return force * pow((2 * normal.scMul(direction) * normal.scMul(to_eye) - direction.scMul(to_eye)), alpha);
}

float Renderer::Phong::getForce(Geom::V3f normal, Geom::V3f to_eye) {
	return a_kof * a.getForce() + b_kof * b.getForce(normal) + c_kof * c.getForce(normal, to_eye);
}
