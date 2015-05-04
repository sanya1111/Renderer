#ifndef BASESTAGES_H_
#define BASESTAGES_H_

#include "Renderer/Model.h"
#include "Renderer/Geom.hpp"

namespace Renderer{

class ModelStage{
	const MeshModel * model;
	size_t ptr;
public:
	//pt n u v
	typedef std::tuple<Geom::TriangleF4, Geom::TriangleF, Geom::V3f, Geom::V3f>  result;
	bool have();
	result process(bool &fin);
	ModelStage(const MeshModel& model);
};

}



#endif /* BASESTAGES_H_ */
