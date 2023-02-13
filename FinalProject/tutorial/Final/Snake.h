//
// Created by יובל היטר on 10/02/2023.
//

#pragma once
#include "iostream"
#include "Model.h"
#include "Camera.h"
#include "igl/opengl/glfw/Viewer.h"
#include "igl/opengl/ViewerData.h"



using namespace cg3d;
class Snake {
private:

    std::vector<std::shared_ptr<cg3d::Model>> bones;
    std::shared_ptr<cg3d::Camera> camera;
    std::shared_ptr<cg3d::Model> snake;
    float speed = 1.0f;
    float scaleFactor = 1.0f;




    typedef
    std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond> >
            RotationList;
    Eigen::MatrixXd V;
    Eigen::MatrixXd VHead; // Vertices of the current mesh (#V x 3)
    Eigen::MatrixXi FHead; // Faces of the mesh (#F x 3)
    Eigen::MatrixXd CT;
    Eigen::MatrixXi BET;
    Eigen::MatrixXd W, Cp, U, M;
    Eigen::MatrixXi  BE;
    Eigen::VectorXi P;
    RotationList rest_pose;
    RotationList anim_pose;
    std::vector<RotationList > poses; // rotations of joints for animation
    RotationList vQ;
    std::vector<Eigen::Vector3d> vC;
    std::vector<Eigen::Vector3d> vT;
    Eigen::Quaterniond quat;
    double anim_t ;
    double anim_t_dir;
    int number_of_joints;
    float joint_length;
    igl::opengl::glfw::Viewer *viewer;
    void load_snake(std::string& path);



public:
    std::vector<Eigen::AlignedBox <double, 3>> snakeJointBoxes;
    Snake();
    Snake(const std::shared_ptr<cg3d::Material>& material, const std::shared_ptr<cg3d::Movable>& root, std::shared_ptr<cg3d::Camera> _camera, igl::opengl::glfw::Viewer *viewer);
    std::vector<std::shared_ptr<cg3d::Model>> GetSnakeBones();
    void SetSpeed(float new_speed);
    float GetSpeed(){return speed;};
    void MoveLeft();
    void MoveRight();
    void MoveUp();
    void MoveDone();
    void initJoints();
    void restartSnake();
    void calcWeight(Eigen::MatrixXd& V, double min_z);

    void skinning(Eigen::Vector3d t);

    Eigen::Matrix4d getHeadMakeTransd();

    void moveSnake(Eigen::Vector3d t);

    void SetTranslation(const Eigen::Vector3d& position, int id);
};


