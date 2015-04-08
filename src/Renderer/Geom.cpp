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

template<class T, int N, int M>
inline void Renderer::Geom::Matrix<T, N, M>::M4x4_quick(float* A, float* B,
		float* C) {
	__m128 row1 = _mm_load_ps(&B[0]),
		   row2 = _mm_load_ps(&B[4]),
	       row3 = _mm_load_ps(&B[8]),
	       row4 = _mm_load_ps(&B[12]);
	for(int i=0; i<4; i++) {
		__m128 brod1 = _mm_set1_ps(A[4*i + 0]),
		       brod2 = _mm_set1_ps(A[4*i + 1]),
		       brod3 = _mm_set1_ps(A[4*i + 2]),
		       brod4 = _mm_set1_ps(A[4*i + 3]);
		_mm_store_ps(&C[4*i], _mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(brod1, row1),
					_mm_mul_ps(brod2, row2)),
				_mm_add_ps(
					_mm_mul_ps(brod3, row3),
					_mm_mul_ps(brod4, row4))));
	}
}
