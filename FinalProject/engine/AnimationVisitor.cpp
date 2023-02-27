//
// Created by Ofir Gilad on 06/02/2023.
//

#include "AnimationVisitor.h"
#include "../tutorial/Final/BasicScene.h"
//#include <GL.h>
#include "Utility.h"
#include "../tutorial/Final/ObjectsNames.h"


namespace cg3d
{
    void AnimationVisitor::Run(Scene* _scene, Camera* camera)
    {
        basicScene =  (BasicScene *)_scene;
        number_of_bones = basicScene->snake->bones.size() - 1;
        Visitor::Run(basicScene, camera);
        Eigen::Vector3f snake_head = basicScene->snake->get_snake_head();
        Eigen::Vector3d max = basicScene->currLevelMap->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff();
        Eigen::Vector3d min = basicScene->currLevelMap->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
        if((snake_head[0] < max[0] && snake_head[0] > min[0] && snake_head[1] < max[1] && snake_head[1] > min[1] && snake_head[2] < max[2] && snake_head[2] > min[2])){
            //std::cout << "the snake is out alarm" << std::endl;
        }
    }
    void AnimationVisitor::Visit(Model* model)
    {
        if(basicScene->animate && model->name == std::string("snake")){
            basicScene->snake->skinning(basicScene->snake->direction);
        }
        Eigen::Matrix3f system = model->GetRotation().transpose();

        Eigen::Vector3f vector1x = Eigen::Vector3f(1, 0, 0);
        Eigen::Vector3f vector2x;

        Eigen::Vector3f vector1y = Eigen::Vector3f(0, 1, 0);
        Eigen::Vector3f vector2y;

    }
}

void AnimationVisitor::MoveAccordingToBezierCurve(Model *model) {

}


