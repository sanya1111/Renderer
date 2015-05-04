#ifndef STAGES_H_
#define STAGES_H_

#include "Renderer/Geom.hpp"
#include <memory>


namespace Renderer{

/* STAGES AND RASTERIZATOR MUST BE LIKE THIS
template<class PT, class PARAMS>
class VertexStage {
public:
	Renderer::Geom::Triangle_<PT> process(const Renderer::Geom::Triangle_<PT> &, const PARAMS &, bool &) ;
};

template<class PT, class PARAMS>
class PixelStage{
public:
	PT process(const PT &, const PARAMS &, bool &) ;
};

template<class PT>
class Rasterizator{
	void draw(const Renderer::Geom::Triangle_<PT> &);
};

*/

template<class BaseStage, class VertexStage, class PixelStage, template<class> class Rasterizator>
class Pipe{
	BaseStage base_stage;
	VertexStage vertex_stage;
	PixelStage pixel_stage;
	Rasterizator<PixelStage> rasterizator;
public:
	Pipe(BaseStage base_stage, VertexStage vertex_stage, PixelStage pixel_stage, Rasterizator rasterizator):
		base_stage(base_stage), vertex_stage(vertex_stage), pixel_stage(pixel_stage), rasterizator(rasterizator){}
	void refresh(typename VertexStage::param p1, typename PixelStage::param p2){
		vertex_stage.refresh(p1);
		pixel_stage.refresh(p2);
	}
	void process(){
		while(base_stage.have()){
			bool ret = true;
			typename BaseStage::result res_base = base_stage.process(ret);
			if(ret)
				continue;
			typename VertexStage::result res_vertex = vertex_stage.process(res_base, ret);
			if(ret)
				continue;
			rasterizator.draw(res_vertex, pixel_stage);
		}
	}

};

}


#endif /* STAGES_H_ */
