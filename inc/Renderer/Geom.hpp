#ifndef GEOM_HPP_
#define GEOM_HPP_

#include <initializer_list>
#include <vector>
#include <cmath>
#include <xmmintrin.h>
#include <bits/stdc++.h>
#include "Renderer/Log.h"
#include "Renderer/Utils.hpp"

namespace Renderer {

namespace Geom {

template<class T>
int8_t sign(T a) {
	if (a < 0)
		return -1;
	if (a > 0)
		return 1;
	return 0;
}



template<class T, int N, int M>
class Matrix{
	std::array<T, N * M> ma;
public:
	struct quick_multipler{
		typedef Matrix<float ,4, 4> with;
		void mult(with &A, const with &B, with &C){
			__m128 row1 = _mm_load_ps(B[0]),
				   row2 = _mm_load_ps(B[1]),
				   row3 = _mm_load_ps(B[2]),
				   row4 = _mm_load_ps(B[3]);
			for(int i=0; i<4; i++) {
				__m128 brod1 = _mm_set1_ps(A[i][0]),
					   brod2 = _mm_set1_ps(A[i][1]),
					   brod3 = _mm_set1_ps(A[i][2]),
					   brod4 = _mm_set1_ps(A[i][3]);
				_mm_store_ps(C[i], _mm_add_ps(
						_mm_add_ps(
							_mm_mul_ps(brod1, row1),
							_mm_mul_ps(brod2, row2)),
						_mm_add_ps(
							_mm_mul_ps(brod3, row3),
							_mm_mul_ps(brod4, row4))));
			}
		}
	};
	struct standart_multipler{
		template<int M2>
		void mult(Matrix<T, N, M> &A, const Matrix<T, M, M2> &B, Matrix<T, N, M2> &C){
			for(int i = 0; i < N; i++){
				for(int j = 0; j < M2; j++){
					C[i][j] = 0;
					for(int k = 0; k < M; k++){
						C[i][j] += A[i][k] * B[k][j];
					}
				}
			}
		}
	};
	typedef typename Selector_<Matrix<T, N, M>, Matrix<float, 4, 4>, quick_multipler, standart_multipler>::result multipler_t;

	Matrix() {}
	Matrix(std::initializer_list<T > Li) {
		int pos = 0;
		for(auto it : Li){
			ma[pos++] = it;
		}
	}
	void print(){
		for(int i = 0; i < N;i++){
			for(int j = 0; j < M; j++){
				DEB("%lf ", ma[i * M + j]);
			}
			DEB("\n");
		}
	}
	T* operator [](size_t i){
		return &ma[i * M];
	}
	const T * operator[](size_t i)const{
		return &ma[i * M];
	}

	std::array<T, N> colomn(size_t j){
		std::array<T, N> res;
		for(int i = 0; i < N; i++){
			res[i] = ma[i * M + j];
		}
		return res;
	}

	template<int M2>
	Matrix<T, N, M2> operator*(const Matrix<T, M, M2> &other){
		Matrix<T, N, M2> ret;
		(typename int_Selector_<M, M2, multipler_t, standart_multipler>::result ()).mult(*this, other, ret);
		return ret;
	}

	Matrix<T, N, M> operator*(const T value){
		Matrix ret;
		for(int i = 0; i < N;i++){
			for(int j = 0; j < M; j++){
				ret[i][j] = ma[i * M + j] * value;
			}
		}
		return ret;
	}
};

template<class T>
using Matrix33 = Matrix<T, 3, 3>;
template<class T>
using Matrix44 = Matrix<T, 4, 4>;

using Matrix44f = Matrix44<float>;
using Matrix33f = Matrix33<float>;

template<class T>
class V3;
namespace MatrixFactory{
	Matrix33f e3();
	Matrix33f rotX(float angle);
	Matrix33f rotY(float angle);
	Matrix33f rotZ(float angle);
	Matrix33f scale(const V3<float> &with);
	Matrix44f projection(float left, float right, float top, float bottom,
			             float near, float far);
	Matrix44f translation(const V3<float> &with);
	Matrix44f withRotation(const V3<float> & r, const V3<float> &up, const V3<float> &f);
};



template<class T >
class V3 {
public:
	static const int num = 3;
	T x, y, z;
	V3() : x(0), y(0), z(0) {}
	V3(T x, T y, T z) : x(x), y(y), z(z) {}
	V3(const std::array<T,3> &val ){
		x = val[0];
		y = val[1];
		z = val[2];
	}
	V3(const T* val){
		x = val[0];
		y = val[1];
		z = val[2];
	}

	T & operator[](size_t id){
		switch(id){
		case 0 :
			return x;
		case 1:
			return y;
		default:
			return z;
		}
	}

	V3 operator+(const V3 &with) {
		return V3(x + with.x, y + with.y, z + with.z);
	}
	V3 operator-(const V3 &with) {
		return V3(x - with.x, y - with.y, y + with.y);
	}
	bool operator<(const V3 &other){
		return (x < other.x) || (x == other.x && y < other.y)
		||	(x == other.x && y == other.y && z < other.z);
	}
	Matrix33<T> rowMatrix(){
		return Matrix33<T>{
			x, y, z,
			0, 0, 0,
			0, 0, 0
		};
	}
	Matrix33<T> colomnMatrix(){
		return Matrix33<T>{
			x, 0, 0,
			y, 0, 0,
			z, 0, 0
		};
	}
	Matrix33<T> skewSimMatrix(){
		return Matrix33<T>{
			0, -z, y,
			z, 0, -x,
			-y, x, 0
		};
	}
	V3<T> vMul(V3<T> &other){
		return V3<T>((skewSimMatrix() * other.colomnMatrix()).colomn(0));
	}

	V3 vSc(V3 & other){
		return V3(x * other.x, y * other.y, z * other.z);
	}

	float scMul(V3 &other){
		return x * other.x + y * other.y + z * other.z;
	}

	V3& scale(V3 v){
		return *this = (rowMatrix() * MatrixFactory::scale(v))[0];
	}

	operator V3<int> (){
		return V3<int>(x, y, z);
	}
	V3& rotate(V3 v){
		using namespace MatrixFactory;
		return *this = (rowMatrix() * rotX(v.x) * rotY(v.y) * rotZ(v.z))[0];
	}
	void print(){
//		DEB("%f %f %f\n", x, y, z);
		DEB("%d %d %d\n", x, y, z);
	}
	T getLen(){
		return sqrt(x * x + y * y + z * z);
	}
	V3& norm(){
		T len = getLen();
		x /= len;
		y /= len;
		z /= len;
		return *this;
	}
};

using V3i = V3<int32_t>;
using V3d = V3<double>;
using V3f = V3<float>;


template<class T >
class V4 {
public:
	static const int num = 4;
	T x, y, z, w;
	V4() : x(0), y(0), z(0), w(0) {}
	V4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w){}
	V4(V3<T> f, T w) : x(f.x), y(f.y), z(f.z), w(w) {}
	V4(T * val){
		x = val[0];
		y = val[1];
		z = val[2];
		w = val[3];
	}
	T & operator[](size_t id){
		switch(id){
		case 0 :
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			return w;
		}
	}
	Matrix44<T> rowMatrix(){
		return Matrix44<T>{
			x, y, z, w,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		};
	}
	Matrix44<T> colomnMatrix(){
		return Matrix44<T>{
			x, 0, 0, 0,
			y, 0, 0, 0,
			z, 0, 0, 0,
			w, 0, 0, 0
		};
	}
	V3<T> norm(){
		return V3<T>(x / w, y / w, z / w);
	}
	V4<T> operator+(const V4<T> &other){
		return V4<T>(x + other.x, y + other.y, z + other.z, w + other.w);
	}
};

using V4i = V4<int32_t>;
using V4d = V4<double>;
using V4f = V4<float>;



template<class T >
class Triangle_{
public:
	V3<T> vs[3];
	Triangle_() {}
	Triangle_(V3<T> from[]){
		for(int8_t i = 0; i < 3; i++){
			vs[i] = from[i];
		}
	}
	Triangle_(const V3<T> &A, const V3<T> &B, const V3<T> &C){
		vs[0] = A;
		vs[1] = B;
		vs[2] = C;
	}
	operator Triangle_<int>(){
		Triangle_<int> outp;
		for(int8_t i = 0; i < 3; i++){
			outp.vs[i] = vs[i];
		}
		return outp;
	}
};

using Triangle = Triangle_<int32_t>;
using TriangleF = Triangle_<float>;
using TriangleD = Triangle_<double>;



}
}


#endif
