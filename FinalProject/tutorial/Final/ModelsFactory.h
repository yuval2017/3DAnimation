//
// Created by יובל היטר on 17/02/2023.
//
#pragma once
#include <iostream>
#include <vector>
#include "iostream"
#include "Model.h"
#include "Material.h"
#include "Program.h"
#include "Camera.h"
#include "igl/opengl/glfw/Viewer.h"
#include "igl/opengl/ViewerData.h"

#define BASIC_MATERIAL 0
#define PHONG_MATERIAL 1
#define BRICKS_MATERIAL 2
#define CUBE 0
#define CYL 1
#define SPHERE 2
#define LINES 3
#define TRUCK 4
#define SNAKE1 5
//increment every mesh or material
#define NUMBER_OF_MATERIALS 3
#define NUMBER_OF_MESHES 6

using namespace cg3d;
class ModelsFactory {
    private:
    static ModelsFactory* instancePtr;
    ModelsFactory();

public:
    //programs
    std::shared_ptr<Program> basicProgram;
    std::shared_ptr<Program> phongProgram;

    //materials
    std::shared_ptr<Material> materials[NUMBER_OF_MATERIALS];
    //1) basic
    //2) phong
    //3) bricks

    //meshes
    std::shared_ptr<Mesh> meshes[NUMBER_OF_MESHES];
    //1) cube
    //2) cyls
    //3) sphere
    //4 truck
    //5) snake

    std::shared_ptr<Model> CreateModel(int material_id, int mesh_id, std::string name);
    std::shared_ptr<Model> CreateBricksCubeModel();


    static ModelsFactory* getInstance();
    ModelsFactory(ModelsFactory const&) = delete;
    void operator=(ModelsFactory const&)  = delete;


};


