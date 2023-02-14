//
// Created by יובל היטר on 14/02/2023.
//
#pragma once
#include <iostream>
#include <vector>
#include "iostream"
#include "Model.h"
#include "Camera.h"
#include "igl/opengl/glfw/Viewer.h"
#include "igl/opengl/ViewerData.h"



struct Vertex {
    float x, y, z;
};

// Define a face structure
struct Face {
    int v1, v2, v3;
};

// Define a texture coordinate structure
struct TexCoord {
    float u, v;
};
using namespace cg3d;
class Calculates {

public:
    std::vector<TexCoord> calculateTextureCoordinates(std::vector<Vertex> vertices, std::vector<Face> faces, const std::string& filename);
    std::vector<TexCoord> calculateTextureCoordinates(Eigen::MatrixXd vertices, Eigen::MatrixXi faces, const std::string& filename);
    void write_obj_file(const std::vector<Vertex>& vertices,std::vector<Face> faces,std::vector<TexCoord> VT, const std::string& filename);


};


