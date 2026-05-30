#pragma once
#include "best_mesh.h"
class L_Mesh : public Mesh {
public:
    std::vector<LineVertex> lineVertices;

    L_Mesh(std::vector<LineVertex>& lineVertices, std::vector<unsigned int>& indices);
    ~L_Mesh();


    void Draw(Shader& shader);

    void Clean();
private:

};
