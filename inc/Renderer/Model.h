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
	std::vector<Texture> col[5];
public:
	enum{
		DIFFUSE_TID = 1,
		SPECULAR_TID = 2,
		AMBIENT_TID = 3,
		EMMISIVE_TID = 4,
	};
	void add(uint8_t type, const Texture & tex);
	std::vector<Texture>& getTextureVec(size_t id);
};

class MeshModel{
public:
	std::vector<Geom::V3f> verts;
	std::vector<Geom::V3f> verts_tex;
	std::vector<Geom::V3f> normals;
	std::vector<std::vector<int32_t> > faces;
};

class Model{
	void clean();
public:
	std::vector<MeshModel> meshs;
	std::vector<size_t> mat_index;
	std::vector<Material> mats;
	void loadFile(const std::string &);
};



}



#endif /* MODEL_H_ */
