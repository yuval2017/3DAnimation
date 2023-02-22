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
#define DAYLIGHT_MATERIAL 3
#define GREEN_MATERIAL 4
#define CUBE 0
#define CYL 1
#define SPHERE 2
#define LINES 3
#define TRUCK 4
#define SNAKE1 5
#define COIN 6
#define TREE 7
#define FROG 8
#define MOUSE 9
//increment every mesh or material
#define NUMBER_OF_MATERIALS 5
#define NUMBER_OF_MESHES 10

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
    //4) cube map shader
    //5) frog material
    //meshes
    std::shared_ptr<Mesh> meshes[NUMBER_OF_MESHES];
    //0) cube
    //1) cyls
    //2) sphere
    //3) lines
    //4) truck
    //5) snake
    //6) coin
    //7) tree
    //8) frog
    //9) mouse

    std::shared_ptr<Model> CreateModel(int material_id, int mesh_id, std::string name);
    std::shared_ptr<Model> CreateBricksCubeModel();


    static ModelsFactory* getInstance();
    ModelsFactory(ModelsFactory const&) = delete;
    void operator=(ModelsFactory const&)  = delete;


};


