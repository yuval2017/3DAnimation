
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
    if(basicScene->animate &&
    (model != nullptr) &&
    (model->name != std::string(std::string(BONE_NAME) +" "+ std::to_string(snake_head)))&&
    (model->name != std::string(std::string(BONE_NAME) + " " + std::to_string(snake_head- 1)))
    ){
        std::shared_ptr<Movable> shrd = model->shared_from_this();
        std::shared_ptr<Model> snake = basicScene->snake->GetSnakeBones()[snake_head];
      if(model->name.find(BONE_NAME) != std::string::npos &&
        !(basicScene->getStatistics()->selfCollisionStopper.is_countdown_running()) &&
         (Calculates::getInstance()-> isMeshCollision(snake, model->shared_from_this(), ((snake)->GetTree()),
                                        model->GetTree()))){
          std::cout << "collision with " << model->name << " \n" << std::endl;
          handle_self_hit();
      }
      else if(!model->isHidden &&
                model->name.find(COLLISION_OBJECT) != std::string::npos &&
                !(basicScene->getStatistics()->objectCollisionStopper.is_countdown_running())){
          if( (Calculates::getInstance()->
                  isMeshCollision(snake, model->shared_from_this(), ((snake)->GetTree()),
                                  model->GetTree()))) {
              std::cout << "collision with " << model->name << " \n" << std::endl;
              handle_object_hit(model);
          }
      }

      else if( !model->isHidden &&
                model->name.find(EATING_OBJECT) != std::string::npos ){
         // std::cout << " check collision with " << model->name << " \n" << std::endl;
          if (Calculates::getInstance()->
                  isMeshCollision(snake, model->shared_from_this(), ((snake)->GetTree()),
                                  model->GetTree())) {
              std::cout << " mouse!! " << model->name << " \n" << std::endl;
              handle_eating(model);
          }
      }
    }
}


void MeshCollisionVisitor::handle_self_hit() {

    basicScene->animate =false;
    basicScene->getSoundManager()->play_sound(std::to_string(FAIL_SOUND));
    basicScene->getStatistics()->menu_flags[GameOverMenu_OP] = true;
}

void MeshCollisionVisitor::handle_object_hit(Model *model){

    basicScene->animate =false;
    basicScene->getSoundManager()->play_sound(std::to_string(FAIL_SOUND));
    basicScene->getStatistics()->menu_flags[GameOverMenu_OP] = true;

}

void MeshCollisionVisitor::handle_eating(Model *model) {

    if(model->name.find(COIN_NAME) != std::string::npos){
       // std::cout<< "eating coin" << std::endl;
        model->isHidden = true;
        model->stopper.reset();
        basicScene->getSoundManager()->play_sound(std::to_string(HIT_SOUND));
        basicScene->getStatistics()->inc_Score(0,0,1);


    }
    else if(model->name.find(FROG_NAME) != std::string::npos){
      //  std::cout<< "eating frog" << std::endl;
        model->isHidden = true;
        model->stopper.reset();
        basicScene->getSoundManager()->play_sound(std::to_string(HIT_SOUND));
        basicScene->getStatistics()->inc_Score(0,1,0);

    }
    else if(model->name.find(MOUSE_NAME) != std::string::npos){
       // std::cout<< "eating mouse" << std::endl;
        model->isHidden = true;
        model->stopper.reset();
        basicScene->getSoundManager()->play_sound(std::to_string(HIT_SOUND));
        basicScene->getStatistics()->inc_Score(1,0,0);

    }
}

