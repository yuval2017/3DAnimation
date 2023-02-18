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
    double x, y, z;
};

// Define a face structure
struct Face {
    int v1, v2, v3;
};

// Define a texture coordinate structure
struct TexCoord {
    double u, v;
};
using namespace cg3d;
class Calculates {
private:
    static Calculates* instancePtr;
    Calculates(){}
public:
    Calculates(const Calculates& obj) = delete;
    static Calculates *getInstance(){
        if (instancePtr == NULL){
            instancePtr = new Calculates();
        }
        return instancePtr;
    }
    bool isMeshCollision(std::shared_ptr<cg3d::Model> mesh1, Model* mesh2, igl::AABB<Eigen::MatrixXd, 3>* treeA, igl::AABB<Eigen::MatrixXd, 3>* treeB);
    bool isBoxesIntersect(Eigen::AlignedBox<double, 3>& boxA, Eigen::AlignedBox<double, 3>& boxB, const std::shared_ptr<cg3d::Model>& mesh1,Model* mesh2);
    std::vector<TexCoord> calculateTextureCoordinates(std::vector<Vertex> vertices, std::vector<Face> faces, const std::string& filename);
    std::vector<TexCoord> calculateTextureCoordinates(Eigen::MatrixXd vertices, Eigen::MatrixXi faces, const std::string& filename);
    void write_obj_file(const std::vector<Vertex>& vertices,std::vector<Face> faces,std::vector<TexCoord> VT, const std::string& filename);
    std::vector<double> linspace(float start_in, float end_in, int num_in);
    void generateRandomBeizierCurve(Eigen::Vector3f vector, Eigen::Matrix <float, 4, 3 > &MG_Result);

    std::vector<TexCoord>
    getVertexTextureCoordinates(std::vector<Vertex> vertices, std::vector<Face> faces, std::string imagePath);
    std::vector<TexCoord> getVertexTextureCoordinates(Eigen::MatrixXd vertices, Eigen::MatrixXi faces, std::string imagePath);
};


