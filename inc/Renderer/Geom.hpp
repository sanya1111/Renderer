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
class Matrix;

template<size_t DIM,typename T> struct dt {
    static T det(const Matrix<T, DIM,DIM>& src) {
        T ret=0;
        for (size_t i=DIM; i--; ret += src[0][i]*src.cofactor(0,i));
        return ret;
    }
};

template<typename T> struct dt<1,T> {
    static T det(const Matrix<T, 1,1>& src) {
        return src[0][0];
    }
};

template<class T>
class V3;

template<class T, int N, int M>
class Matrix{
protected:
	std::array<T, N * M> ma;
public:
	struct standart_multipler{
		template<int M2>
		static void mult(Matrix<T, N, M> &A, const Matrix<T, M, M2> &B, Matrix<T, N, M2> &C){
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
	typedef standart_multipler multipler_t;

	Matrix() {
		std::fill(ma.begin(), ma.end(), 0);
	}
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
		standart_multipler::mult(*this, other, ret);
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
	 T det() const {
		return dt<M,T>::det(*this);
	}

	Matrix<T, N-1, M-1> get_minor(size_t row, size_t col) const {
		Matrix<T, N-1,M-1> ret;
		for (size_t i=N-1; i--; )
			for (size_t j=M-1;j--; ret[i][j]=(*this)[i<row?i:i+1][j<col?j:j+1]);
		return ret;
	}

	T cofactor(size_t row, size_t col) const {
		return get_minor(row,col).det()*((row+col)%2 ? -1 : 1);
	}

	Matrix<T, N,M> adjugate() const {
		Matrix<T, N,M> ret;
		for (size_t i=N; i--; )
			for (size_t j=M; j--; ret[i][j]=cofactor(i,j));
		return ret;
	}

	Matrix<T, N, M> operator/(const T other){
		Matrix<T, N, M> res;
		FOR(i, N) FOR(j, M){
			res[i][j] = (*this)[i][j] /other;
		}
		return res;
	}

	Matrix<T, N,M> invert_transpose() {
		Matrix<T, N,M> ret = adjugate();
		T tmp = V3<T>(ret[0]).scMul(V3<T>((*this)[0]));
		return ret/tmp;
	}

	Matrix<T,M,N> invert() {
		return invert_transpose().transpose();
	}

	Matrix<T, M,N> transpose() {
			Matrix<T, M, N> ret;
			for (size_t i=M; i--; ){
				std::array<T, N> c= colomn(i);
				FOR(j, N){
					ret[i][j] = c[j];
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
	Matrix44f rotX4(float angle);
	Matrix44f rotY4(float angle);
	Matrix44f rotZ4(float angle);
	Matrix33f scale(const V3<float> &with);
	Matrix44f scale4(const V3<float> &with);
	Matrix44f projection(float left, float right, float top, float bottom,
			             float near, float far);
	Matrix44f translation(const V3<float> &with);
	Matrix44f withRotation(const V3<float> & r, const V3<float> &up, const V3<float> &f);
	Matrix44f transform(const V3<float> &cen, const V3<float> &rot, const V3<float> scale);
};



template<class T>
class V2{
public:
	static const int32_t num = 2;
	T x, y;
	V2(const V2 &other) = default;
	V2() : x(0), y(0){}
	V2(T x, T y) : x(x), y(y) {}
	V2(T * val){
		x = val[0];
		y = val[1];
	}
	T & operator[](size_t id){
		if(!id)
			return x;
		return y;
	}
	V2<T> operator+(const V2<T> &other){
		return V2<T>(x + other.x, y + other.y);
	}
	bool operator<(const V2<T> &other) const{
			return (x < other.x) || (x == other.x && y < other.y);
	}
	void print(){
//		DEB("%f %f %f %f\n", x, y, z, w);
	}
};

template<class T >
class V3 {
public:
	static const int32_t num = 3;
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

	std::array<T, 3> arr(){
		return {x, y, z};
	};
	std::array<T, 4> arr4(){
		return {x, y, z, 1};
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

	V3 operator*(const T &with) const{
		return V3(x * with, y * with, z * with);
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
	V3<T> vMul(V3<T> &other) const{
		return V3<T>((skewSimMatrix() * other.colomnMatrix()).colomn(0));
	}

	V3 vSc(const V3 & other)const{
		return V3(x * other.x, y * other.y, z * other.z);
	}

	float scMul(const V3 &other)const{
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

	V3 operator-(){
		return V3(-x, -y, -z);
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

	void print()const{
		DEB("%f %f %f\n", x, y, z);
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
	static const int32_t num = 4;
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
	V3<T> norm() const{
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
	void print(){
		DEB("%f %f %f %f\n", x, y, z, w);
	}
};

using V4i = V4<int32_t>;
using V4d = V4<double>;
using V4f = V4<float>;

template<class T, int32_t X>
class VX : public Matrix<T, X, 1>{
public:
	static const int32_t num = X;
	using Matrix<T, X, 1>::ma;
	VX(){
		std::fill(ma.begin(), ma.end(), 0);
//		for(int32_t i = 0; i < X; i++){s
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
		for(int32_t i = 0; i < X; i++){
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

template<int32_t X>
using VXf = VX<float, X>;
template<int32_t X>
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
	T& operator[](size_t id){
		return vs[id];
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
using TriangleF4 = Triangle_<V4f>;

template<int32_t X>
using TriangleX = Triangle_<VXi<X>>;
template<int32_t X>
using TriangleXF = Triangle_<VXf<X>>;







}
}



#endif
