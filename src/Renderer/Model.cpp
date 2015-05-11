#include "Renderer/Model.h"
#include "Renderer/Log.h"
#include "Renderer/Graphics.h"

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Wrapper/stb_image.h"
#include "Wrapper/stb_image_write.h"

using namespace std;
using namespace Renderer::Geom;
using namespace Renderer;

void Renderer::Model::clean() {
	vector<MeshModel>().swap(meshs);
	vector<size_t>().swap(mat_index);
	vector<Material>().swap(mats);
}

void Renderer::Model::loadObj2(const string &filename) {
	clean();
	Assimp::Importer imp;
	const aiScene* sc = imp.ReadFile(filename,
		    aiProcess_GenSmoothNormals |
		    aiProcess_Triangulate |
		    aiProcess_JoinIdenticalVertices |
		    aiProcess_OptimizeMeshes);
	meshs.resize(sc->mNumMeshes);
	mat_index.resize(sc->mNumMeshes);
	for(unsigned i = 0; i < sc->mNumMeshes; i++){
		aiMesh * mesh = sc->mMeshes[i];
		mat_index[i] = mesh->mMaterialIndex;
		for(unsigned j = 0 ;j < mesh->mNumVertices; j++){
			meshs[i].verts.push_back(V3f(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z));
			meshs[i].normals.push_back(V3f(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z));
			meshs[i].verts_tex.push_back(V3f(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y, mesh->mTextureCoords[0][j].z));
		}
//		DEB("OUT\n");

		for(unsigned j = 0; j < mesh->mNumFaces; j++){
			vector<int> res;
			for(unsigned k = 0; k < mesh->mFaces[j].mNumIndices; k++){
				res.push_back(mesh->mFaces[j].mIndices[k]);
			}
			meshs[i].faces.push_back(res);
		}
	}
	string glob_path = filename.substr(0, filename.find_last_of("/")) + "/";
	for(unsigned i = 0; i < sc->mNumMaterials; i++){
		aiMaterial * mat = sc->mMaterials[i];
		Material pmat;
		for(int t = aiTextureType_DIFFUSE; t <= aiTextureType_EMISSIVE; t++){
			for(unsigned j = 0; j < mat->GetTextureCount((aiTextureType)t); j++){
				aiString path;
				if(mat->GetTexture((aiTextureType)t, j, &path) == AI_SUCCESS) {
					cerr << glob_path + string(path.C_Str()) << endl;
					pmat.add(t, Texture(glob_path + string(path.C_Str())));
				}
			}
		}
		mats.push_back(pmat);
	}
}

Renderer::Texture::Texture(const std::string& filename) : data((uint8_t *) stbi_load(filename.c_str(), (int *)&width, (int *)&height, (int *)&comp, 0),
		[](uint8_t * ptr){free(ptr);}) {
}

Rgba Renderer::Texture::at(const uint32_t& x, const uint32_t& y) const{
	uint8_t * beg = data.get() + (x * width + y) * comp;
	return Rgba(*beg, *(beg + 1), *(beg + 2), 0);
}

void Renderer::Material::add(uint8_t type, const Texture& tex) {
	col[type].push_back(tex);
}

void Renderer::Texture::writePng(const string &filename) {
	stbi_write_png(filename.c_str(), width, height, comp, data.get(), comp * width);
}

void Renderer::Texture::writeBmp(const std::string& filename) {
	stbi_write_bmp(filename.c_str(), width, height, comp, data.get());
}

void Renderer::Texture::writeHdr(const std::string& filename) {
	//float * what the?????
//	stbi_write_hdr(filename.c_str(), width, height, comp, data.get());
}

void Renderer::Texture::writeTga(const std::string& filename) {
	stbi_write_tga(filename.c_str(), width, height, comp, data.get());
}

Renderer::Texture::Texture(shared_ptr<uint8_t> data,
		uint32_t width, uint32_t height, uint32_t comp) : comp(comp), width(width), height(height), data(data){
}
