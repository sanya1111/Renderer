#include "Renderer/Geom.hpp"
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
			0, cos(angle), sin(angle),
			0, -sin(angle), cos(angle)
	};
}

Renderer::Geom::Matrix33f Renderer::Geom::MatrixFactory::rotY(float angle) {
	return Matrix33f{
			cos(angle),  0, sin(angle),
			0,           1,      0,
			-sin(angle), 0, cos(angle)
	};
}

Renderer::Geom::Matrix33f Renderer::Geom::MatrixFactory::rotZ(float angle) {
	return Matrix33f{
			cos(angle),  sin(angle), 0,
			-sin(angle), cos(angle), 0,
			0 ,          0,          1
	};
}

Renderer::Geom::Matrix33f Renderer::Geom::MatrixFactory::scale(float x) {
	return e3() * x;
}


Renderer::Geom::Matrix44f Renderer::Geom::MatrixFactory::projection(float left, float right,
		float top, float bottom, float near, float far) {
	return Matrix44f{
		2.0 / (right - left), 0, 0, 0,
		0, 2.0 / (top - bottom), 0, 0,
		(left + right) / (left - right), (bottom + top) / (bottom - top), (far + near) / (far - near), 1,
		0, 0, -2*near*far/(far - near), 0

	};
}
