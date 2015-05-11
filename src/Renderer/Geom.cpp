#include "Renderer/Geom.hpp"

using namespace Renderer::Geom;

Renderer::Geom::Matrix33f Renderer::Geom::MatrixFactory::e3() {
	return Matrix33f{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};
}

Renderer::Geom::Matrix33f Renderer::Geom::MatrixFactory::rotX(float angle) {
	return Matrix33f{
			1, 0 , 0,
			0, cosf(angle), sinf(angle),
			0, -sinf(angle), cosf(angle)
	};
}

Renderer::Geom::Matrix33f Renderer::Geom::MatrixFactory::rotY(float angle) {
	return Matrix33f{
			cosf(angle),  0, sinf(angle),
			0,           1,      0,
			-sinf(angle), 0, cosf(angle)
	};
}

Renderer::Geom::Matrix33f Renderer::Geom::MatrixFactory::rotZ(float angle) {
	return Matrix33f{
			cosf(angle),  sinf(angle), 0,
			-sinf(angle), cosf(angle), 0,
			0 ,          0,          1
	};
}

Matrix44f Renderer::Geom::MatrixFactory::rotX4(float angle) {
	return Matrix44f{
		1, 0 ,           0,           0,
		0, cosf(angle),  sinf(angle), 0,
		0, -sinf(angle), cosf(angle), 0,
		0, 0,            0,           1
	};
}

Matrix44f Renderer::Geom::MatrixFactory::rotY4(float angle) {
	return Matrix44f{
			cosf(angle),  0,              sinf(angle), 0,
			0,            1,              0,           0,
			-sinf(angle), 0,              cosf(angle), 0,
			0,            0,              0,           1
	};
}

Matrix44f Renderer::Geom::MatrixFactory::rotZ4(float angle) {
	return Matrix44f{
			cosf(angle),  sinf(angle), 0, 0,
			-sinf(angle), cosf(angle), 0, 0,
			0 ,           0,           1, 0,
			0,            0,           0, 1
	};
}

Renderer::Geom::Matrix33f Renderer::Geom::MatrixFactory::scale(const V3<float> &with) {
	return Matrix33f{
		with.x, 0, 0,
		0, with.y, 0,
		0, 0, with.z
	};
}

Matrix44f Renderer::Geom::MatrixFactory::scale4(const V3<float>& with) {
	return Matrix44f{
		with.x, 0,      0,      0,
		0,      with.y, 0,      0,
		0,      0,      with.z, 0,
		0,      0,      0,      1
	};
}

Renderer::Geom::Matrix44f Renderer::Geom::MatrixFactory::projection(float left, float right,
		float top, float bottom, float near, float far) {
	return Matrix44f{
		2.0f / (right - left), 0, 0, 0,
		0, 2.0f / (top - bottom), 0, 0,
		(left + right) / (left - right), (bottom + top) / (bottom - top), (far + near) / (far - near), 1,
		0, 0, -2.0f * near * far / ( far - near ), 0

	};
}

Matrix44f MatrixFactory::translation(const V3<float> &with) {
	return Matrix44f{
			1.0f, 0.0f, 0.0f, 0,
			0.0f, 1.0f, 0.0f, 0,
			0.0f, 0.0f, 1.0f, 0,
			with.x, with.y, with.z, 1.0f
	};
}

Renderer::Geom::Matrix44f Renderer::Geom::MatrixFactory::withRotation(const V3<float> & r, const V3<float> & up, const V3<float> & f) {
	return Matrix44f {
			r.x, up.x, f.x, 0.0f,
			r.y, up.y, f.y, 0.0f,
			r.z, up.z, f.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
	};
}

Matrix44f Renderer::Geom::MatrixFactory::transform(const V3<float>& cen,
		const V3<float>& rot, const V3<float> scale) {
	return  (scale4(scale) * rotX4(rot.x) * rotY4(rot.y) * rotZ4(rot.z) ) * translation(cen) ;
}


