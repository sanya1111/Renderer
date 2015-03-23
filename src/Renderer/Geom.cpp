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

Renderer::Geom::Matrix33f Renderer::Geom::MatrixFactory::scale(const V3<float> &with) {
	return Matrix33f{
		with.x, 0, 0,
		0, with.y, 0,
		0, 0, with.z
	};
}


Renderer::Geom::Matrix44f Renderer::Geom::MatrixFactory::projection(float left, float right,
		float top, float bottom, float near, float far) {
	return Matrix44f{
		2.0 / (right - left), 0, 0, 0,
		0, 2.0 / (top - bottom), 0, 0,
		(left + right) / (left - right), (bottom + top) / (bottom - top), (far + near) / (far - near), 1,
		0, 0, -2.0 * near * far / ( far - near ), 0

	};
}

Renderer::Geom::Matrix44f Renderer::Geom::MatrixFactory::translation(const V3<float> &with) {
	return Matrix44f{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-with.x, -with.y, -with.z, 1.0f
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
