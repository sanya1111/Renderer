#ifndef MODEL_H_
#define MODEL_H_
#include <string>
#include <vector>
#include <memory.h>
#include "Renderer/Geom.hpp"

namespace Renderer{

class Rgba;

class Texture{
	std::shared_ptr<uint8_t> data;
	uint32_t comp;
public:
	uint32_t width, height;
	Texture(const std::string &filename);
	Texture(std::shared_ptr<uint8_t> data_, uint32_t width, uint32_t height,uint32_t comp );
	Rgba at(const uint32_t &x, const uint32_t &y)const;
	void writePng(const std::string &filename);
	void writeBmp(const std::string &filename);
	void writeHdr(const std::string &filename);
	void writeTga(const std::string &filename);
	friend class Drawer;
};

class Material{
	//0 - DIFFUSE, 1 - SPECULAR, 2 - AMBIENT , 3 - EMMISIVE
	std::vector<Texture> col[4];
public:
	void add(uint8_t type, const Texture & tex);
	friend class Drawer;
};

class MeshModel{
	std::vector<Geom::V3f> verts;
	std::vector<Geom::V3f> verts_tex;
	std::vector<Geom::V3f> normals;
	std::vector<std::vector<int> > faces;
	std::vector<Material> mats;
	size_t mat_index;
public:
	void loadObj(const std::string &);
	void loadObj2(const std::string &);
	friend class ModelStage;
	friend class Drawer;
};

class Model{
	std::vector<MeshModel> meshs;
public:

};



}



#endif /* MODEL_H_ */
