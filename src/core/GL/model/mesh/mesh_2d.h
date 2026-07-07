#pragma once

#include "best_mesh.h"


class Texture;
class Mesh_2D : public Mesh {
public:

    std::vector<Texture*> textures;

    Mesh_2D(std::vector<Vertex2D>& vertices, std::vector<unsigned int>& indices, std::vector<Texture*>& textures);
    ~Mesh_2D();


    void Draw(Shader& shader);
	void unTexDraw(Shader& shader);

    void Clean();

	void setTextures(std::vector<Texture*>& textures) {
		this->textures = textures;
	}

    Texture* getTex() { return textures[0]; }
private:
};