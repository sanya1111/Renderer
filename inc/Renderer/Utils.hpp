#ifndef UTILS_HPP_
#define UTILS_HPP_

#define FOR(i, n) for(int i = 0; i < (n); i++)
#define FORN(i, j, n) for(int i = (j); i < (n); i++)

template<class now, class comp, class T, class U>
struct Selector_{
	typedef U result;
};

template<class comp, class T, class U>
struct Selector_<comp, comp, T, U>{
	typedef T result;
};

template<int now, int comp, class T, class U>
struct int_Selector_{
	typedef U result;
};

template<int now, class T, class U>
struct int_Selector_<now, now, T, U>{
	typedef T result;
};

template<class T>
bool between(T b, T pt, T e){
	return pt >= b && pt <= e;
}

const int INF = 1e9;

float segmSolver(float x_s, float x_e, float at);

float segmPointer(float x_s, float x_e, float al);



#endif /* UTILS_HPP_ */
