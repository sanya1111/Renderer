#include "Renderer/Model.h"
#include "Renderer/Log.h"
#include "Renderer/Graphics.h"

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define STB_IMAGE_IMPLEMENTATION
#include "Wrapper/stb_image.h"

using namespace std;
using namespace Renderer::Geom;
using namespace Renderer;

void Renderer::MeshModel::loadObj(const string &filename) {

	std::ifstream in;
	in.open (filename, std::ifstream::in);
	if(in.fail()){
		DEB("%s\n", filename.c_str());
		return;
	}
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::stringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			V3f v;
			for (int i=0;i<3;i++) iss >> v[i];
				verts.push_back(v);
		}  else if (!line.compare(0, 2, "f ")) {
			 std::vector<int> f;
			int itrash, idx;
			iss >> trash;
			while (iss >> idx >> trash >> itrash >> trash >> itrash) {
			idx--;
			f.push_back(idx);
			}
			faces.push_back(f);
		}
	}
}



void Renderer::MeshModel::loadObj2(const string &filename) {
	Assimp::Importer imp;
	const aiScene* sc = imp.ReadFile(filename,
		    aiProcess_GenSmoothNormals |
		    aiProcess_Triangulate |
		    aiProcess_JoinIdenticalVertices |
		    aiProcess_OptimizeMeshes);
	for(unsigned i = 0; i < sc->mNumMeshes; i++){
		aiMesh * mesh = sc->mMeshes[i];
		mat_index = mesh->mMaterialIndex;
		for(unsigned j = 0 ;j < mesh->mNumVertices; j++){
			verts.push_back(V3f(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z));
			normals.push_back(V3f(mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z));
			verts_tex.push_back(V3f(mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y, mesh->mTextureCoords[0][j].z));
		}

		for(unsigned j = 0; j < mesh->mNumFaces; j++){
			vector<int> res;
			for(unsigned k = 0; k < mesh->mFaces[j].mNumIndices; k++){
				res.push_back(mesh->mFaces[j].mIndices[k]);
			}
			faces.push_back(res);
		}
	}

	/*string glob_path = filename.substr(0, filename.find_last_of("/")) + "/";
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
	*/
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
