#ifndef CAMERA_H_
#define CAMERA_H_
#include "Renderer/Geom.hpp"

namespace Renderer{
class CameraView{
	Geom::V3f cen,
		up,
		r,
		f;
	float aw, ah;
	float near, far;
	Geom::Matrix44f projection_ma;
	Geom::Matrix44f move_ma;
public:
	CameraView & operator=(const CameraView & other) = default;
	CameraView() {}
	CameraView(Geom::V3f cen, Geom::V3f up1, Geom::V3f f, float angle, int32_t width, int32_t height, float near, float far);
	Geom::V3f moveToCam(const Geom::V3f &v);
	Geom::V3f projection(const Geom::V3f &v);
	Geom::Matrix44f projection_matrix()const;
	Geom::V4f projection2(const Geom::V3f &v);
};

}


#endif /* CAMERA_H_ */
