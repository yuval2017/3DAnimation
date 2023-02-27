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
#include <igl/AABB.h>


#define BASIC_MATERIAL  0
#define PHONG_MATERIAL  1
#define BRICKS_MATERIAL 2
#define DAYLIGHT_MATERIAL   3
#define GREEN_MATERIAL  4
#define GREY_MATERIAL   5
#define GOLD_MATERIAL   6
#define FROG_MATERIAL   7
#define TREE_MATERIAL   8
#define LEVEL2_MAP      9
#define LEVEL3_MAP      10
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
#define NUMBER_OF_MATERIALS 11
#define NUMBER_OF_MESHES 10
#define TREE_PATH "data/masktree_obj/masktree.obj"


using namespace cg3d;
class ModelsFactory {
    private:
    MeshData createDataFromBox(Eigen::AlignedBox<double, 3>& box);
    std::shared_ptr<Model> createBox(Eigen::AlignedBox<double, 3>& box);
    static ModelsFactory* instancePtr;
    ModelsFactory();
    void create_bounding_box(int mesh_id);

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
    //5) green material
    //6) grey material
    //7) gold material
    //8) frog material
    //9) tree material
    //10)level2 map
    //11) level3 map

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

    std::shared_ptr<Model> bounding_boxes[NUMBER_OF_MESHES];
    igl::AABB<Eigen::MatrixXd, 3> trees[NUMBER_OF_MESHES];

    std::shared_ptr<Model> CreateModel(int material_id, int mesh_id, const std::string& name);
    std::shared_ptr<Model> CreateModel2(int material_id, const std::string& path, std::string name);

    static ModelsFactory* getInstance();
    ModelsFactory(ModelsFactory const&) = delete;
    void operator=(ModelsFactory const&)  = delete;


};


