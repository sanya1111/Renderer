#ifndef BASESTAGES_H_
#define BASESTAGES_H_

#include "Renderer/Model.h"
#include "Renderer/Geom.hpp"

namespace Renderer{

class ModelStage{
	const MeshModel * model;
	size_t ptr;
	std::vector<Geom::V3f> norm_v;
public:
	//pt n u v
	typedef std::tuple<Geom::TriangleF4, Geom::TriangleF, Geom::V3f, Geom::V3f>  result;
	bool have();
	result process(bool &fin);
	ModelStage(const MeshModel& model);
	ModelStage(){}
	void operator=(ModelStage &&mstage);
	void start();
};

}



#endif /* BASESTAGES_H_ */
