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

    std::shared_ptr<cg3d::Camera> camera;
    std::shared_ptr<cg3d::Model> snake;
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
    Eigen::MatrixXd V_new;
    void Calc_Next_Position(std::vector<Eigen::Vector3f> &p, std::vector<double> &ris_Array, std::vector<double> &lambdaI_Array, Eigen::Vector3d t);


public:
    float speed = 0.8;
    std::vector<std::shared_ptr<cg3d::Model>> bones;
    std::vector<Eigen::AlignedBox <double, 3>> snakeJointBoxes;
    Snake();
    Snake(const std::shared_ptr<cg3d::Material>& material, const std::shared_ptr<cg3d::Movable>& root, std::shared_ptr<cg3d::Camera> _camera);
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
    igl::opengl::glfw::Viewer viewer;
    void skinning(Eigen::Vector3d t);
    void initSnake();
    Eigen::Vector3f ikGetPosition(int id, float length);
    Eigen::Matrix4d getHeadMakeTransd();
    Eigen::Vector3d direction;
    bool with_skinning = false;
    float move = 0.5f;

    std::shared_ptr<Movable> root;

    void ikRotateHelper(int id, const Eigen::Vector3f& t);
    void IKFabric();
    std::shared_ptr<Model> sphere1;
    void moveSnake(Eigen::Vector3d t);
    void moveSnake2(Eigen::Vector3d t);
    void propTranslate(Eigen::Vector3d t ,int id);
    void reset_bones();
    void reset_sake();
    Eigen::Vector3f  get_snake_head();
    void skinningV2();

    Eigen::Vector4f getDistanceFromColsestJoints(Eigen::Vector3f posV, Eigen::MatrixXd C);
    Eigen::Vector2f CalculateWeightByDistances(int joint1Index, float distance1, int joint2Index, float distance2);
    float WeightFunction(float distance);
    void SetTranslation(const Eigen::Vector3d& position, int id);

};


