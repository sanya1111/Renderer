#include "Renderer/Utils.hpp"
using namespace std;
#include <algorithm>

float segmSolver(float x_s, float x_e, float at) {
	if(x_s == x_e)
		return numeric_limits<float>::max();
	return ( (at - x_s) / (x_e - x_s));
}

float segmPointer(float x_s, float x_e, float al) {
	return x_s + al * (x_e - x_s);
}
