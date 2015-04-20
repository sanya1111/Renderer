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
protected:
	std::array<T, N * M> ma;
public:
	struct quick_multipler{
		typedef Matrix<float ,4, 4> with;
		void mult(with &A, const with &B, with &C){
			__m128 row1 = _mm_loadu_ps(B[0]),
				   row2 = _mm_loadu_ps(B[1]),
				   row3 = _mm_loadu_ps(B[2]),
				   row4 = _mm_loadu_ps(B[3]);
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

	T  operator[](size_t id) const{
		switch(id){
		case 0 :
			return x;
		case 1:
			return y;
		default:
			return z;
		}
	}
	V3 operator+(const V3 &with)const {
		return V3(x + with.x, y + with.y, z + with.z);
	}
	V3 operator-(const V3 &with)const{
		return V3(x - with.x, y - with.y, y + with.y);
	}
	bool operator<(const V3 &other)const{
		return (x < other.x) || (x == other.x && y < other.y)
		||	(x == other.x && y == other.y && z < other.z);
	}
	Matrix33<T> rowMatrix()const{
		return Matrix33<T>{
			x, y, z,
			0, 0, 0,
			0, 0, 0
		};
	}
	Matrix33<T> colomnMatrix()const{
		return Matrix33<T>{
			x, 0, 0,
			y, 0, 0,
			z, 0, 0
		};
	}
	Matrix33<T> skewSimMatrix()const{
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

	operator V3<int> (){
		return V3<int>(x, y, z);
	}

	V3& scale(const V3 &v){
		return *this = (rowMatrix() * MatrixFactory::scale(v))[0];
	}

	V3& rotate(const V3 &v){
		using namespace MatrixFactory;
		return *this = (rowMatrix() * rotX(v.x) * rotY(v.y) * rotZ(v.z))[0];
	}

	V3& transform(const V3<T> &cen, const V3<T> &rot, const V3<T> &scale){
		return *this = cen + this->rotate(rot).scale(scale) ;
	}

	V3 scale(const V3 &v)const{
		return rowMatrix() * MatrixFactory::scale(v)[0];
	}

	V3 rotate(const V3 &v)const{
		using namespace MatrixFactory;
		return rowMatrix() * rotX(v.x) * rotY(v.y) * rotZ(v.z)[0];
	}

	V3 transform(const V3<T> &cen, const V3<T> &rot, const V3<T> &scale)const{
		return cen + this->rotate(rot).scale(scale) ;
	}

	void print(){
//		DEB("%f %f %f\n", x, y, z);
//		DEB("%d %d %d\n", x, y, z);
	}
	T getLen()const{
		return sqrt(x * x + y * y + z * z);
	}
	V3 norm()const{
		T len = getLen();
		return V3<T>(x / len, y / len, z / len);
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
	V4(const V3<T> &f, T w) : x(f.x), y(f.y), z(f.z), w(w) {}
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
	Matrix44<T> rowMatrix()const{
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
	bool operator<(const V4<T> &other) const{
			return (x < other.x) || (x == other.x && y < other.y)
			||	(x == other.x && y == other.y && z < other.z)
			||  (x == other.x && y == other.y && z == other.z && w < other.w);
	}
};

using V4i = V4<int32_t>;
using V4d = V4<double>;
using V4f = V4<float>;

template<class T, int X>
class VX : public Matrix<T, X, 1>{
public:
	static const int num = X;
	using Matrix<T, X, 1>::ma;
	VX(){
		for(int i = 0; i < X; i++){
			ma[i] = 0;
		}
	}
	VX(const VX<T, X>&) = default;

	VX<T, X>& operator=(const VX<T, X> &other) {
		if(this != &other){
			ma = other.ma;
		}
		return *this;
	}


	VX<T, X> operator+(const VX<T, X> &other){
		VX<T, X> res;
		for(int i = 0; i < X; i++){
			res[i] = (*this)[i] + other[i];
		}
		return res;
	}
	bool operator<(const VX<T, X> &other){
		return ma < other.ma;
	}
	T& operator [](size_t i){
		return ma[i];
	}
	const T& operator[](size_t i)const{
		return ma[i];
	}
};

template<int X>
using VXf = VX<float, X>;
template<int X>
using VXi = VX<int, X>;

template<class T >
class Triangle_{
public:
	T vs[3];
	Triangle_() {}
	Triangle_(T from[]){
		for(int8_t i = 0; i < 3; i++){
			vs[i] = from[i];
		}
	}
	Triangle_(const T &A, const T &B, const T &C){
		vs[0] = A;
		vs[1] = B;
		vs[2] = C;
	}
	operator Triangle_<V3i>(){
		Triangle_<V3i> outp;
		for(int8_t i = 0; i < 3; i++){
			outp.vs[i] = vs[i];
		}
		return outp;
	}
};

using Triangle = Triangle_<V3i>;
using TriangleF = Triangle_<V3f>;
using Triangle4 = Triangle_<V4i>;

template<int X>
using TriangleX = Triangle_<VXi<X>>;

Triangle4 makeTriangle4(const Triangle &a, const V3i &ot);

TriangleX<6> makeTriangle6(const Triangle &a, const V3i &norm, const V3i &u, const V3i &v);

}
}


#endif
