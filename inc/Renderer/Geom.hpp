#ifndef GEOM_H_
#define GEOM_H_

#include <initializer_list>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>
namespace Renderer {

namespace Geom {
//help functions

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
	std::vector<std::vector<T> > ma;
public:
	Matrix() :ma(std::vector<std::vector<T>>(N,std::vector<T>(M))) {}
	Matrix(std::initializer_list<T > Li) : ma(std::vector<std::vector<T>>(N,std::vector<T>(M))){
		int posx = 0, posy = 0;
		for(auto it : Li){
			ma[posx][posy] = it;
			posy++;
			if(posy == M) {
				posx++;
				posy = 0;
			}
		}
	}
	void print(){
//		FOR(i, N){
//			FOR(j, M){
//				DEB("%lf ", ma[i][j]);
//			}
//			DEB("\n");
//		}
	}
	std::vector<T> &operator [](size_t i){
		return ma[i];
	}
	std::vector<T> colomn(size_t j){
		std::vector<T> res;
		for(int i = 0; i < N; i++){
			res.push_back(ma[i][j]);
		}
		return res;
	}
	template<int M2>
	Matrix<T, N, M2> operator*(const Matrix<T, M, M2> &other){
		Matrix<T, N, M2> ret;
		for(int i = 0; i < N; i++){
			for(int j = 0; j < M2; j++){
				ret[i][j] = 0;
				for(int k = 0; k < M; k++){
					ret[i][j] += ma[i][k] * other.ma[k][j];
				}
			}
		}
		return ret;
	}

	Matrix<T, N, M> operator*(const T value){
		Matrix ret;
		for(int i = 0; i < N;i++){
			for(int j = 0; j < M; j++){
				ret[i][j] = ma[i][j] * value;
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

namespace MatrixFactory{
	Matrix33f e3();
	Matrix33f rotX(float angle);
	Matrix33f rotY(float angle);
	Matrix33f rotZ(float angle);
	Matrix33f scale(float x);
	Matrix44f projection(float left, float right, float top, float bottom,
			             float near, float far);
};



template<class T >
class V3 {
public:
	T x, y, z;
	V3() = default;
	V3(T x, T y, T z) : x(x), y(y), z(z) {}
	V3(const std::vector<T> &val ){
		x = val[0];
		y = val[1];
		z = val[2];
	}
	V3& operator=(const std::vector<T> &val){
		x = val[0];
		y = val[1];
		z = val[2];
		return *this;
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
		vSc(v);
		return *this;
	}

	V3& rotate(V3 v){
		using namespace MatrixFactory;
		return *this = (rowMatrix() * rotX(v.x) * rotY(v.y) * rotZ(v.z))[0];
	}
	void print(){
		DEB("%f %f %f\n", x, y, z);
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
	T x, y, z, w;
	V4() = default;
	V4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w){}
	V4(V3<T> f, T w) : x(f.x), y(f.y), z(f.z), w(w) {}
	V4(const V3<T>& fr) : x(fr.x), y(fr.y), z(fr.z), w(1){
	}
	V4(const std::vector<T> &val ){
		x = val[0];
		y = val[1];
		z = val[2];
		w = val[3];
	}
	V4& operator=(const std::vector<T> &val){
		x = val[0];
		y = val[1];
		z = val[2];
		w = val[3];
		return *this;
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
};

using V4i = V4<int32_t>;
using V4d = V4<double>;
using V4f = V4<float>;



template<class T >
class Triangle_{
public:
	V3<T> vs[3];
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
};

using Triangle = Triangle_<int32_t>;
using TriangleF = Triangle_<float>;
using TriangleD = Triangle_<double>;



}
}

#endif /* GEOM_H_ */
