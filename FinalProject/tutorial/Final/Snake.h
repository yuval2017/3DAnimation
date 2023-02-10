//
// Created by יובל היטר on 10/02/2023.
//

#pragma once
#include "iostream"
#include "Model.h"
#include "Camera.h"
using namespace cg3d;
class Snake {
private:
    std::vector<std::shared_ptr<cg3d::Model>> bones;
    std::shared_ptr<cg3d::Camera> camera;
    float speed = 1.0f;
    float scaleFactor = 1.0f;

public:
    Snake();
    Snake(const std::shared_ptr<cg3d::Material>& material, const std::shared_ptr<cg3d::Movable>& root, std::shared_ptr<cg3d::Camera> _camera);
    std::vector<std::shared_ptr<cg3d::Model>> GetSnakeBones();
    void SetSpeed(float new_speed);
    float GetSpeed(){return speed;};
    void MoveLeft();
    void MoveRight();
    void MoveUp();
    void MoveDone();
};


