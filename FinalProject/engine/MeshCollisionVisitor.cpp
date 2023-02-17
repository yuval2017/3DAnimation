
#include "MeshCollisionVisitor.h"
#include "Visitor.h"
#include "Camera.h"
#include <utility>
#include <memory>
#include "iostream"
#include "Model.h"
#include "Scene.h"
#include "../tutorial/Final//ObjectsNames.h"
using namespace std;
void cg3d::MeshCollisionVisitor::Run(cg3d::Scene *scene, cg3d::Camera *camera) {
    basicScene = (BasicScene *)scene;
    Visitor::Run(basicScene, camera);
}


void cg3d::MeshCollisionVisitor::Visit(Model *model) {

    Visitor::Visit(model);
    if (model != nullptr && model->name != std::string("bone 1") && (model->name != std::string("bone 0")) && (model->name.substr(0,
                                                                                                                                  strlen(COLLISION_OBJECT)) == COLLISION_OBJECT || model->name.substr(0,
                                                                                                                                                                                                      strlen(BONE_NAME)) == BONE_NAME)) {
        std::shared_ptr<Model> snake = basicScene->snake->GetSnakeBones()[0];
        if (Calculates::getInstance()->isMeshCollision(snake, model, ((snake)->GetTreeWithOutCube()), model->GetTreeWithOutCube())) {
            if(model->name.substr(0, strlen(BONE_NAME)) == BONE_NAME){
                std::cout << "collision with " << model->name << " \n" << std::endl;
                basicScene->animate = false;
            }else if(model->name.substr(0, strlen(COLLISION_OBJECT)) == COLLISION_OBJECT){
                std::string name = model->name.substr(15,model->name.size()-1);
                //to make sure it was deleted
                std::cout << "collision with " << model->name << " \n" << std::endl;
                basicScene->GetRoot()->RemoveChild(model->bezier);
                model->bezier = nullptr;
                basicScene->GetRoot()->RemoveChild(model->shared_from_this());
                //TODO for each collide change data in scene.
            }
            else{
                std::cout << "should not collide with: " << model->name << " \n" << std::endl;
            }
        }
    }
    //for childs
}

