#ifndef BASESTAGES_H_
#define BASESTAGES_H_

#include "Renderer/Model.h"
#include "Renderer/Geom.hpp"

namespace Renderer{

class ModelStage{
	const Model * model;
	size_t ptr_face;
	int current_mesh;
	std::vector<Geom::V3f> norm_v;
	void next_mesh();
public:
	//pt n u v
	typedef std::tuple<Geom::TriangleF4, Geom::TriangleF, Geom::V3f, Geom::V3f>  result;
	bool have();
	result process(bool &fin);
	ModelStage(const Model& model);
	ModelStage(){}
	void start();
	void operator=(ModelStage &&mstage);
};

}



#endif /* BASESTAGES_H_ */
