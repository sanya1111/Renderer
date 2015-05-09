#include "Renderer/Light.h"
using namespace Renderer;
using namespace Geom;

float Renderer::DiffuseLight::getForce(Geom::V3f vec, float mat_kof) {
	return vec.norm().scMul(direction) * force * mat_kof;
}

float Renderer::AmbientLight::getForce() {
	return force;
}

Geom::V3f Renderer::SpecularLight::nextBeam() {

}

//\vec{R}\cdot\vec{V}=2(\vec{N}\cdot\vec{L})(\vec{N}\cdot\vec{V})-(\vec{L}\cdot\vec{V})

float Renderer::SpecularLight::getForce(Geom::V3f normal, Geom::V3f to_eye, float alpha, float mat_kof) {
	normal = normal.norm();
	to_eye = to_eye.norm();
	return force * mat_kof * pow((2 * normal.scMul(direction) * normal.scMul(to_eye) - direction.scMul(to_eye)), alpha);
}

float Renderer::Phong::getForce(Geom::V3f normal, Geom::V3f to_eye,
		float dif_kof, float alpha, float spec_kof) {
	return a_kof * a.getForce() + b_kof * b.getForce(normal, dif_kof) + c_kof * c.getForce(normal, to_eye, alpha, spec_kof);
}
