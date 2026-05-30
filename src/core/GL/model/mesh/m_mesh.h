#pragma once

#include "best_mesh.h"


class Texture;
class M_Mesh : public Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<Texture*> textures;

    M_Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture*>& textures);
    ~M_Mesh();


	void Draw(Shader& shader);

    void Clean();
	void CleanTextures();

	Texture* getTex(){return textures[0];}
private:
};