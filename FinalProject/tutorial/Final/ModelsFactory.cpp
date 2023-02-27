//
// Created by יובל היטר on 17/02/2023.
//

#include "ModelsFactory.h"
#include "IglMeshLoader.h"
#include "ObjLoader.h"
#include "Calculates.h"


ModelsFactory* ModelsFactory::instancePtr = NULL;
ModelsFactory::ModelsFactory(){
    //basic program and material
    basicProgram = std::make_shared<Program>("shaders/basicShader");
    materials[BASIC_MATERIAL] = std::make_shared<Material>("basic_material", basicProgram);


    //basic program and material
    phongProgram = std::make_shared<Program>("shaders/phongShader");
    materials[PHONG_MATERIAL] = std::make_shared<Material>("material", phongProgram);


    //bricks material
    materials[BRICKS_MATERIAL] = std::make_shared<Material>("basic_material", basicProgram);
    materials[BRICKS_MATERIAL]->AddTexture(0, "textures/bricks.jpg", 2);


    //cube map shader material
    materials[DAYLIGHT_MATERIAL] = {std::make_shared<Material>("daylight", "shaders/cubemapShader")};
    materials[DAYLIGHT_MATERIAL]->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);


    //cube map shader material LEVEL 2
    materials[LEVEL2_MAP] = {std::make_shared<Material>("level 2 map", "shaders/cubemapShader")};
    materials[LEVEL2_MAP]->AddTexture(0, "../tutorial/textures/cubemaps/Level2Map/Stairs/Stairs_", 3);


    //cube map shader material LEVEL 3
    materials[LEVEL3_MAP] = {std::make_shared<Material>("level 3 map", "shaders/cubemapShader")};
    materials[LEVEL3_MAP]->AddTexture(0, "../tutorial/textures/cubemaps/Level3Map/Teide/Teide_", 3);


    //frog material
    //materials[FROG_MATERIAL] = {std::make_shared<Material>("green_color", "shaders/phongShader")};
    //materials[FROG_MATERIAL]-> program->name = "green";


    //green material
    materials[GREEN_MATERIAL] = {std::make_shared<Material>("green_color", "shaders/phongShader")};
    materials[GREEN_MATERIAL]->program->name = "green";

    //grey material
    materials[GREY_MATERIAL] = {std::make_shared<Material>("grey_color", "shaders/phongShader")};
    materials[GREY_MATERIAL]->program->name = "grey";

    //tree material
    materials[TREE_MATERIAL] = std::make_shared<Material>("tree", "shaders/basicShader"); // default material
    materials[TREE_MATERIAL]->AddTexture(0, "data/masktree_obj/tree2.tga", 2);

    //gold material
    materials[GOLD_MATERIAL] = {std::make_shared<Material>("gold_color", "shaders/phongShader")};
    materials[GOLD_MATERIAL]->program->name = "gold";

    //meshes
    meshes[CUBE] = Mesh::Cube();


    meshes[CYL] = ObjLoader::MeshFromObj("Cyl", {"data/zcylinder.obj"});
    meshes[SPHERE] = ObjLoader::MeshFromObj("sphere_mesh", {"data/sphere.obj"});
    meshes[TRUCK] = ObjLoader::MeshFromObj("sphere_mesh", {"data/truck.obj"});
    meshes[COIN] = ObjLoader::MeshFromObj("coin_mesh", {"data/coin.obj"});
    meshes[MOUSE] = {IglLoader::MeshFromFiles("mouse_mesh", "data/mouse/Mouse.obj")};
    meshes[FROG] =  {IglLoader::MeshFromFiles("frog2_mesh", "data/frog/Frog.obj")};
    meshes[TREE] = {IglLoader::MeshFromFiles("frog2_mesh", "data/masktree_obj/masktree.obj")};

    //need to implement but not in lines
    meshes[LINES] = NULL;
    meshes[SNAKE1] = ObjLoader::MeshFromObj("snake_mesh", {"data/snake1.obj"});

    for (int i = 0; i < NUMBER_OF_MESHES; i++) {
        if(i != 3){
            create_bounding_box(i);
        }
    }
}
ModelsFactory *ModelsFactory::getInstance(){
    if (instancePtr == NULL){
        instancePtr = new ModelsFactory();
    }
    return instancePtr;
}
std::shared_ptr<Model> ModelsFactory::CreateModel(int material_id, int mesh_id, const std::string& name){
    return Model::Create(name, meshes[mesh_id], materials[material_id]);
}
std::shared_ptr<Model> ModelsFactory::CreateModel2(int material_id, const std::string& path, std::string name){
    return ObjLoader::ModelFromObj(name , path, materials[material_id]);
}

void ModelsFactory::create_bounding_box(int mesh_id){
    igl::AABB<Eigen::MatrixXd, 3> tree;
    tree.init(meshes[mesh_id]->data[0].vertices, meshes[mesh_id]->data[0].faces);
    trees[mesh_id] = tree;
    bounding_boxes[mesh_id] = Calculates::getInstance()->createBox(trees[mesh_id].m_box);
    bounding_boxes[mesh_id]->isHidden = true;
}
