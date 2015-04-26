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
		2.0f / (right - left), 0, 0, 0,
		0, 2.0f / (top - bottom), 0, 0,
		(left + right) / (left - right), (bottom + top) / (bottom - top), (far + near) / (far - near), 1,
		0, 0, -2.0f * near * far / ( far - near ), 0

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

Triangle4 Renderer::Geom::makeTriangle4(const Triangle &a, const V3i& ot) {
	Triangle4 res;
	for(int i = 0; i < 3; i++){
		res.vs[i] = V4i(a.vs[i], ot[i]);
	}
	return res;
}

TriangleX<6> Renderer::Geom::makeTriangle6(const Triangle &a, const V3i &norm, const V3i &u, const V3i &v){
	TriangleX<6> res;
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			res.vs[i][j] = a.vs[i][j];
		}
		res.vs[i][3] = norm[i];
		res.vs[i][4] = u[i];
		res.vs[i][5] = v[i];
	}
	return res;
}

TriangleXF<7> Renderer::Geom::makeTriangle7(Triangle_<V4f> a, V3f norm, V3f u, V3f v ){
	TriangleXF<7> res;
	FOR(i ,3){
		FOR(j, 4){
			res.vs[i][j] = a.vs[i][j];
		}
		res.vs[i][4] = norm[i];
		res.vs[i][5] = u[i];
		res.vs[i][6] = v[i];
	}
	return res;
}

