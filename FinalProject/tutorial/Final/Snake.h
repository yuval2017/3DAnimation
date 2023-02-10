//
// Created by יובל היטר on 10/02/2023.
//

#ifndef ENGINEREWORK_SNAKE_H
#define ENGINEREWORK_SNAKE_H
#include "iostream"
#include "Model.h"
using namespace cg3d;
class Snake {
private:
    std::vector<std::shared_ptr<cg3d::Model>> bones;
    std::shared_ptr<cg3d::Camera> camera;
    float speed = 1.0f;
    float scaleFactor = 1.0f;

public:
    Snake(std::shared_ptr<cg3d::Material> material, std::shared_ptr<cg3d::Movable> root, auto _camera);
    std::vector<std::shared_ptr<cg3d::Model>> GetSnakeBones();
    void SetSpeed(float new_speed);
    float GetSpeed(){return speed;};
    void MoveLeft();
    void MoveRight();
    void MoveUp();
    void MoveDone();
};


#endif //ENGINEREWORK_SNAKE_H
