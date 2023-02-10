//
// Created by יובל היטר on 10/02/2023.
//

#include "Snake.h"
#include "Movable.h"
#include "IglMeshLoader.h"

Snake::Snake(){
    std::cout<< "Snake :) " << " \n"<< std::endl;
}
Snake::Snake(const std::shared_ptr<cg3d::Material>& material, const std::shared_ptr<cg3d::Movable>& root, std::shared_ptr<cg3d::Camera> _camera){
    auto cylMesh{cg3d::IglLoader::MeshFromFiles("cyl_igl","data/zcylinder.obj")};
    bones.push_back(cg3d::Model::Create("bone 0",cylMesh, material));
    bones[0]->Scale(scaleFactor,cg3d::Movable::Axis::Z);
    bones[0]->SetCenter(Eigen::Vector3f(0,0,-0.8f*scaleFactor));
    root->AddChild(bones[0]);

    for(int i = 1;i < 3; i++)
    {
        bones.push_back(cg3d::Model::Create("bone " + std::to_string(i), cylMesh, material));
        bones[i]->Scale(scaleFactor,cg3d::Movable::Axis::Z);
        bones[i]->Translate(1.6f*scaleFactor,cg3d::Movable::Axis::Z);
        bones[i]->SetCenter(Eigen::Vector3f(0,0,-0.8f*scaleFactor));
        bones[i-1]->AddChild(bones[i]);
    }
    bones[0]->Translate({0,0,0.8f*scaleFactor});
    camera = _camera;
    bones[0]->AddChild(camera);
}
std::vector<std::shared_ptr<cg3d::Model>> Snake::GetSnakeBones(){
    return bones;
}
void Snake::SetSpeed(float new_speed){
    speed = new_speed;
}
void Snake::MoveLeft(){
    bones[0]->Rotate(0.1f, cg3d::Movable::Axis::Y);
    bones[1]->Rotate(-0.1f, cg3d::Movable::Axis::Y);
}
void Snake::MoveRight(){
    bones[0]->Rotate(-0.1f, cg3d::Movable::Axis::Y);
    bones[1]->Rotate(0.1f, cg3d::Movable::Axis::Y);
}
void Snake::MoveUp(){
    bones[0]->Rotate(0.1f, cg3d::Movable::Axis::X);
    bones[1]->Rotate(-0.1f, cg3d::Movable::Axis::X);
}
void Snake::MoveDone(){
    bones[0]->Rotate(-0.1f, cg3d::Movable::Axis::X);
    bones[1]->Rotate(0.1f, cg3d::Movable::Axis::X);
}