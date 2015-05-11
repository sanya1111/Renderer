#include "Renderer/Camera.h"

using namespace Renderer;
using namespace Renderer::Geom;

Renderer::CameraView::CameraView(Geom::V3f cen_, Geom::V3f up1_, Geom::V3f f_, float angle_, int32_t width_, int32_t height_, float near_, float far_) :
		cen(cen_) , f(f_), aw(angle_), near(near_), far(far_){

	this->r = up1_.vMul(this->f);
	this->up = this->f.vMul(this->r);

	this->f = this->f.norm();
	this->up = this->up.norm();
	this->r = this->r.norm();

	ah = atan(tan(aw) * height_ / width_);

	float projection_plane_z = 1.0;
	float right = std::tan(aw) * projection_plane_z;
	float left = -right;
	float top = std::tan(ah) * projection_plane_z;
	float bottom = -top;
	move_ma = MatrixFactory::translation(-cen) * MatrixFactory::withRotation(r, up, f);
	projection_ma = move_ma * MatrixFactory::projection(left, right, top, bottom, near, far);
}

Geom::V3f Renderer::CameraView::moveToCam(const Geom::V3f &v) {
	return V4f((V4f(v, 1).rowMatrix() * move_ma)[0]).norm();
}

Geom::V3f Renderer::CameraView::projection(const Geom::V3f& v) {
	return projection2(v).norm();
}

Geom::Matrix44f Renderer::CameraView::projection_matrix()const {
	return projection_ma;
}

Geom::V4f Renderer::CameraView::projection2(const Geom::V3f& v) {
	return V4f((V4f(v, 1).rowMatrix() * projection_ma)[0]);
}

bool Renderer::CameraView::inProjection(const Geom::V4f& pt) {
	V3f good = pt.norm();
	return between(-1.0f, good.x, 1.0f) && between(-1.0f, good.y, 1.0f) && between(near, good.z, far);
}

Geom::V3f Renderer::CameraView::get_center() {
	return cen;
}
