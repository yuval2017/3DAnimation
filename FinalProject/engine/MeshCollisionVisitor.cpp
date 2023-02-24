
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
    int snake_head =basicScene->snake->bones.size()-1;
    Visitor::Visit(model);
    if (basicScene->animate && model != nullptr && !(model->isHidden)  && model->name != std::string(std::string(BONE_NAME) +" "+
    std::to_string(snake_head)) && (model->name != std::string(std::string(BONE_NAME) + " " +
    std::to_string(snake_head- 1))) )
    {
        std::shared_ptr<Model> snake = basicScene->snake->GetSnakeBones()[snake_head];
        if(!(basicScene->getStatistics()->objectCollisionStopper->is_countdown_running()) & model->name.substr(0,strlen(COLLISION_OBJECT)) == COLLISION_OBJECT &&
                Calculates::getInstance()->isMeshCollision(snake, model, ((snake)->GetTreeWithOutCube()), model->GetTreeWithOutCube())){
            handle_object_hit(model);
        }
        else if (!(basicScene->getStatistics()->selfCollisionStopper->is_countdown_running())& model->name.substr(0,strlen(BONE_NAME)) == BONE_NAME &&
                Calculates::getInstance()->isMeshCollision(snake, model, ((snake)->GetTreeWithOutCube()), model->GetTreeWithOutCube())){
            std::cout << "collision with " << model->name << " \n" << std::endl;
            handle_self_hit();
        }

    }
}


void MeshCollisionVisitor::handle_self_hit() {
    if(basicScene->getData()->get_life_bought() >0){
        basicScene->getData()->dec_life_bought();
        basicScene->getStatistics()->selfCollisionStopper->start(4);
    }
    else{
        basicScene->animate = false;
        basicScene->getStatistics()->objectCollisionStopper->reset();
        basicScene->getStatistics()->selfCollisionStopper->reset();
        basicScene->getSoundManager()->play_sound(std::to_string(FAIL_SOUND));
        basicScene->getStatistics()->menu_flags[GameOverMenu_OP] = true;
    }
}

void MeshCollisionVisitor::handle_object_hit(Model *model){

    if(model->name.find(COIN_NAME) != std::string::npos){
        std::cout<< "hitting coin" << std::endl;
        model->isHidden = true;
        model->stopper.reset();
        basicScene->getStatistics()->inc_Score(COIN_PRICE);


    }
    else if(model->name.find(FROG_NAME) != std::string::npos){
        std::cout<< "hitting frog" << std::endl;
        model->isHidden = true;
        model->stopper.reset();
        basicScene->getSoundManager()->play_sound(std::to_string(HIT_SOUND));
        basicScene->getStatistics()->inc_Score(FROG_PRICE);

    }
    else if(model->name.find(MOUSE_NAME) != std::string::npos){
        std::cout<< "hitting mouse" << std::endl;
        model->isHidden = true;
        model->stopper.reset();
        basicScene->getSoundManager()->play_sound(std::to_string(HIT_SOUND));
        basicScene->getStatistics()->inc_Score(MOUSE_PRICE);

    }
    else if(model->name.find(CUBE_NAME) != std::string::npos) {
        std::cout<< "hitting cube" << std::endl;
        if(basicScene->getData()->get_life_bought() >0){
            basicScene->getData()->dec_life_bought();
            basicScene->getStatistics()->selfCollisionStopper->start(4);
        }
        else{
            basicScene->animate = false;
            basicScene->getStatistics()->objectCollisionStopper->reset();
            basicScene->getStatistics()->selfCollisionStopper->reset();
            basicScene->getStatistics()->menu_flags[GameOverMenu_OP] = true;
            basicScene->getSoundManager()->play_sound(std::to_string(FAIL_SOUND));
        }
    }

}

