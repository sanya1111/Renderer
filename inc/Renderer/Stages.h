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

template<typename InpType, typename VertexStage, typename PixelStage, typename Rasterizator>
class Pipe{
	VertexStage vertex_stage;
	PixelStage pixel_stage;
	Rasterizator rasterizator;
public:
	Pipe(VertexStage vertex_stage, PixelStage pixel_stage, Rasterizator rasterizator):
		vertex_stage(vertex_stage), pixel_stage(pixel_stage), rasterizator(rasterizator){}
	void refresh(typename VertexStage::param p1, typename PixelStage::param p2){
		vertex_stage.refresh(p1);
		pixel_stage.refresh(p2);
	}
	void process(InpType inp){
		bool ret = true;
		InpType res =vertex_stage.process(inp, ret);
		if(ret){
			rasterizator.draw(res, pixel_stage);
		}
	}
	void ref_proc(InpType inp,typename VertexStage::param p1, typename PixelStage::param p2){
		refresh(p1, p2);
		process(inp);
	}

};

}


#endif /* STAGES_H_ */
