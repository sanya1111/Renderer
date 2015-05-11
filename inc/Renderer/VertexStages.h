#ifndef VERTEXSTAGES_H_
#define VERTEXSTAGES_H_

#include "Renderer/Camera.h"
#include "Renderer/Geom.hpp"
#include "Renderer/Utils.hpp"
#include "Renderer/Light.h"

namespace Renderer{

class DefaultVertexStage{
	Geom::Matrix44f to_world,
					to_cam;
	Phong light;
	CameraView * main_view;
public:
	typedef std::tuple<Geom::TriangleF4, Geom::TriangleF> result;
	DefaultVertexStage(CameraView &main_view, const Geom::V3f &position, const Geom::V3f &scale, const Geom::V3f &rot, const Phong &light)
		: main_view(&main_view), light(light){
		to_world = Geom::MatrixFactory::transform(position, rot, scale);
		to_cam = main_view.projection_matrix();
	}
	DefaultVertexStage() {}
	result process(std::tuple<Geom::TriangleF4, Geom::TriangleF, Geom::V3f, Geom::V3f> &tu, bool &ret);
};

}


#endif /* VERTEXSTAGES_H_ */
