//
// Created by יובל היטר on 12/02/2023.
//

#include "ObjectsAnimationVisitor.h"
#include "../tutorial/Final/IglMeshLoader.h"

void ObjectsAnimationVisitor::Run(Scene *scene, Camera *camera) {
    basicScene = (BasicScene *)scene;
    if(!is_visitor_inited){
        //don't need to init it anymore
        is_visitor_inited = true;
        program = std::make_shared<Program>("../tutorial/shaders/phongShader");
        //material1
        material =  std::make_shared<Material>("material", program); // empty material
        material->AddTexture(0, "textures/box0.bmp", 2);
        GenerateSphereObject(material);
        GenerateSphereObject(material);
        GenerateCubeObject(material);
    }

    Visitor::Run(scene, camera);
}

void ObjectsAnimationVisitor::Visit(Model *model) {
    Visitor::Visit(model);
}

void ObjectsAnimationVisitor::GenerateCubeObject(const std::shared_ptr<Material>& _material) {
    auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","../tutorial/data/cube_old.obj")};
    auto cube = Model::Create( "cube", cubeMesh, _material);
    basicScene->GetRoot()->AddChild(cube);
    cube->showWireframe = true;
    cube->Translate({generate_random_number(minx,maxx),0,0});

}

void ObjectsAnimationVisitor::GenerateSphereObject(const std::shared_ptr<Material>& _material) {
    auto sphereMesh{IglLoader::MeshFromFiles("sphere_igl", "../tutorial/data/sphere.obj")};
    auto sphere1 = Model::Create( "sphere3",sphereMesh, _material);
    basicScene->GetRoot()->AddChild(sphere1);
    sphere1->showWireframe = true;
    sphere1->Translate({generate_random_number(minx,maxx),0,0});
}

int ObjectsAnimationVisitor::generate_random_number(int min, int max) {
    return rand()%(max-min + 1) + min;;
}




