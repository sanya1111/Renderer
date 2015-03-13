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
class Point {
public:
	T x, y;
	Point(T _x, T _y) :
			x(_x), y(_y) {
	}
	Point() {}
	Point operator+(const Point &with) {
		return Point(x + with.x, y + with.y);
	}
	Point operator-(const Point &with) {
		return Point(x - with.x, y - with.y);
	}
};

template<class T>
class Line {
public:
	Point<T> begin, end;
	Line(Point<T> _begin, Point<T> _end) :
			begin(_begin), end(_end) {
	}
	Line() {}
};

/*template<class T>
class Triangle{
public:
	Line<T> lines[3];
	Triangle(Line<T> from[]){
		for(int32_t i = 0; i < 3; i++){
			lines[i] = from[i];
		}
	}
	Triangle(std::initializer_list<Line<T>> from){
//		for(int32_t i = 0; i < 3; i++){
//			lines[i] = from[i];
//		}
	}
	Triangle(Point<T> from[]){
		for(int32_t i = 0; i < 3; i++){
			lines[i] = Line<T>(from[i], from[(i + 1) % 3]);
		}
	}
	Triangle(std::initializer_list<Point<T>> from){
		int i = 0;
		for(typename std::initializer_list<Point<T>>::const_iterator it = from.begin(); i < 3; ++it){
			typename std::initializer_list<Point<T>>::const_iterator nex = it;
			++nex;
			if(nex == from.end())
				nex = from.begin();
			lines[i] = Line<T>(*it, *nex);
			i++;
		}
	}
};
*/
}
}

#endif /* GEOM_H_ */
