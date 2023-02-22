//
// Created by יובל היטר on 17/02/2023.
//

#include "ModelsFactory.h"
#include "IglMeshLoader.h"
#include "ObjLoader.h"


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


    //green material
    materials[GREEN_MATERIAL] = {std::make_shared<Material>("green_color", "shaders/phongShader")};
    materials[GREEN_MATERIAL]->program->name = "green";
    //meshes
    meshes[CUBE] = Mesh::Cube();
    meshes[CYL] = ObjLoader::MeshFromObj("Cyl", {"data/zcylinder.obj"});
    meshes[SPHERE] = ObjLoader::MeshFromObj("sphere_mesh", {"data/sphere.obj"});
    meshes[TRUCK] = ObjLoader::MeshFromObj("sphere_mesh", {"data/truck.obj"});
    meshes[COIN] = ObjLoader::MeshFromObj("coin_mesh", {"data/coin.obj"});
    //meshes[TREE] = ObjLoader::MeshFromObj("tree_mesh",{"data/tree2.obj"} );
    meshes[FROG] = {IglLoader::MeshFromFiles("frog_mesh", "data/frog/Frog.obj")};
    //need to implement but not in lines
    meshes[LINES] = NULL;
    meshes[SNAKE1] = ObjLoader::MeshFromObj("snake_mesh", {"data/snake1.obj"});

}
ModelsFactory *ModelsFactory::getInstance(){
    if (instancePtr == NULL){
        instancePtr = new ModelsFactory();
    }
    return instancePtr;
}
std::shared_ptr<Model> ModelsFactory::CreateModel(int material_id, int mesh_id, std::string name){
    return Model::Create(name, meshes[mesh_id], materials[material_id]);
}
std::shared_ptr<Model> ModelsFactory::CreateBricksCubeModel(){
    return nullptr;
}

