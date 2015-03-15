	#ifndef GEOM_H_
#define GEOM_H_

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

//classes
template<class T>
class Point2D {
public:
	T x, y;
	Point2D(T _x, T _y) :
			x(_x), y(_y) {
	}
	Point2D() {}
	Point2D operator+(const Point2D &with) {
		return Point(x + with.x, y + with.y);
	}
	Point2D operator-(const Point2D &with) {
		return Point(x - with.x, y - with.y);
	}
};


template<class T>
class Triangle2D{
public:
	Point2D<T> points[3];
	Triangle2D(Point2D<T> from[]){
		for(int8_t i = 0; i < 3; i++){
			points[i] = from[i];
		}
	}
	Triangle2D(const Point2D<T> &A, const Point2D<T> &B, const Point2D<T> &C){
		points[0] = A;
		points[1] = B;
		points[2] = C;
	}
};

}
}

#endif /* GEOM_H_ */
