//
// Created by יובל היטר on 10/02/2023.
//

#include "Snake.h"

#include <utility>
#include "Movable.h"
#include "IglMeshLoader.h"
#include <igl/deform_skeleton.h>
#include <igl/dqs.h>
#include <igl/per_vertex_normals.h>


#include <igl/opengl/glfw/imgui/ImGuiMenu.h>

#include <imgui/imgui.h>

#include <iostream>
#include <vector>
#include <stb_image.h>
#include <triangle/triangulate.h>
#include <iostream>
#include <vector>
#include <tuple>
#include <map>

Snake::Snake(){
    std::cout<< "Snake :) " << " \n"<< std::endl;
}
Snake::Snake(const std::shared_ptr<cg3d::Material>& material, const std::shared_ptr<cg3d::Movable>& root, std::shared_ptr<cg3d::Camera> _camera){


    auto cylMesh{cg3d::IglLoader::MeshFromFiles("cyl_igl","data/zcylinder.obj")};
    bones.push_back(cg3d::Model::Create("bone 0",cylMesh, material));
    bones[0]->Scale(scaleFactor,cg3d::Movable::Axis::Z);
    bones[0]->SetCenter(Eigen::Vector3f(0,0,-0.8f*scaleFactor));
    root->AddChild(bones[0]);
    number_of_joints = 16;
    for(int i = 1;i < number_of_joints; i++)
    {
        bones.push_back(cg3d::Model::Create("bone " + std::to_string(i), cylMesh, material));
        bones[i]->Scale(scaleFactor,cg3d::Movable::Axis::Z);
        bones[i]->Translate(1.6f*scaleFactor,cg3d::Movable::Axis::Z);
        bones[i]->SetCenter(Eigen::Vector3f(0,0,-0.8f*scaleFactor));
        bones[i-1]->AddChild(bones[i]);
    }
    bones[0]->Translate({0,0,0.8f*scaleFactor});
    camera = std::move(_camera);
    bones[0]->AddChild(camera);
    auto snakeMesh{IglLoader::MeshFromFiles("snake_igl", "data/snake1.obj")};
    snake = Model::Create("snake",snakeMesh,material);
    root->AddChild(snake);
    snake->showWireframe = true;


    float lengthOfSnake = snake->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff()[2] - snake->GetMeshList()[0]->data[0].vertices.colwise().minCoeff()[2];
    std::cout<< lengthOfSnake << std::endl;
    joint_length = lengthOfSnake / number_of_joints;
    std::cout<< joint_length << std::endl;
    snake->Scale((number_of_joints),cg3d::Movable::Axis::Z);
    snake->Translate(0,cg3d::Movable::Axis::XYZ);
    viewer.data().set_mesh(snake->GetMeshList()[0]->data[0].vertices, snake->GetMeshList()[0]->data[0].faces);
    //initJoints();
    //getTextureCoords("../tutorial/textures/snake.png",snake->GetMeshList()[0]->data[0].vertices,snake->GetMeshList()[0]->data[0].faces);
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
void Snake::initJoints(){
    int bones_size = number_of_joints;
    vC.resize(bones_size + 1);
    vT.resize(bones_size + 1);
    vQ.resize(bones_size + 1,Eigen::Quaterniond::Identity());
    Eigen::Vector3d min = snake->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
    double min_z = min[2];
    //set parents
    BE.resize(16, 2);
    Cp.resize(17, 3);
    CT.resize(32, 3);
    BE << 0, 1,
            1, 2,
            2, 3,
            3, 4,
            4, 5,
            5, 6,
            6, 7,
            7, 8,
            8, 9,
            9,10,
            10, 11,
            11, 12,
            12, 13,
            13, 14,
            14, 15,
            15, 16;


    restartSnake();
    //viewer->data_list[0].add_points(points, Eigen::RowVector3d(0, 0, 1));
    V = snake->GetMeshList()[0]->data[0].vertices;
    calcWeight(V, min_z);
    snakeJointBoxes.resize(bones_size);
    //initBoundingBoxesForJoints();
    //jointLength = 0.1;
}
void Snake::restartSnake(){
    int bones_size = number_of_joints;
   // viewer->data(0).set_vertices(V);
    Eigen::Vector3d min = snake->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
    Eigen::Vector3d max = snake->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff();
    float min_z = min[2];
    float max_z = max[2];
    Eigen::Vector3d pos(0, 0, min_z);
    Eigen::MatrixXd points(17, 3);
    for (int i = 0; i < bones_size + 1; i++) {
        vC[i] = pos.cast<double>();
        vT[i] = pos;
        points.row(i) = pos;
        pos = pos + Eigen::Vector3d(0, 0, joint_length);
    }
//    for (int i = 0; i < bones_size; i++) {
//
//        bones[i]->Tout = Eigen::Affine3f::Identity();
//        bones[i]->Tin = Eigen::Affine3f::Identity();
//        bones[i]->Tout.pretranslate(Eigen::Vector3f(0, 0, min_z + (i - 1) * bones_size));
//        bones[i]->Tin.pretranslate(-Eigen::Vector3f(0, 0, min_z + (i - 1) * bones_size));
//        bones[i]->Tout.pretranslate(vC[i - 1].cast<float>());
//    }
    for (int i = 0; i < bones_size + 1; i++) {
        Cp.row(i) = vC[i];
    }
    for (int i = 0; i < bones_size; i++) {
        CT.row((i+1) * 2 - 1) = vC[i+1];
    }
}
void Snake::calcWeight(Eigen::MatrixXd& V, double min_z){
    int bones_size = number_of_joints;
    int n = snake->GetMeshList()[0]->data[0].vertices.rows();
    W=Eigen::MatrixXd::Zero( n,bones_size+1);
    for (int i = 0; i < n; i++) {
        double curr_z = V.row(i)[2];
        for (int j = 0; j < bones_size + 1; j++) {
            if (curr_z >= min_z + joint_length * float(j) && curr_z <= min_z + joint_length * float(j + 1)) {
                double res = abs(curr_z - (min_z + joint_length * float(j + 1))/16) * 10;
                W.row(i)[j] = res;
                W.row(i)[j + 1] = 1-res ;
                break;
            }
        }

    }

}

void Snake::skinning(Eigen::Vector3d t) {
    //moveSnake(std::move(t));
    const int dim = Cp.cols();
    Eigen::MatrixXd T(BE.rows() * (dim + 1), dim);
    for (int e = 0; e < BE.rows(); e++)
    {
        Eigen::Affine3d a = Eigen::Affine3d::Identity();
        a.translate(vT[e].cast<double>());
        a.rotate(vQ[e].cast<double>());
        T.block(e * (dim + 1), 0, dim + 1, dim) =
                a.matrix().transpose().block(0, 0, dim + 1, dim);
    }
    igl::dqs(snake->GetMeshList()[0]->data[0].vertices, W, vQ, vT, U);
    //move joints according to T, returns new position in CT and BET
    igl::deform_skeleton(Cp.cast<double>(), BE, T, CT, BET);
    //viewer.data(0).set_vertices(U);
    //viewer.data(0).set_edges(CT.cast<double>(), BET,Eigen::RowVector3d(70. / 255., 252. / 255., 167. / 255.));

    //igl::per_vertex_normals(U,snake->GetMeshList()[0]->data[0].faces,snake->GetMeshList()[0]->data[0].vertices);



    T = Eigen::MatrixXd::Zero(U.rows(),2);
    std::shared_ptr<cg3d::Mesh> new_mesh = std::make_shared<cg3d::Mesh>(snake->name,
                                                                        U,
                                                                        snake->GetMeshList()[0]->data[0].faces,
                                                                        snake->GetMeshList()[0]->data[0].vertexNormals,
                                                                        snake->GetMeshList()[0]->data[0].textureCoords);
//    snake->GetMeshList()[0]->data.pop_back();
//
//    snake->GetMeshList()[0]->data.push_back({U,viewer.data().F,viewer.data().compute_normals(),T});
    snake->SetMeshList({new_mesh});




    for (int i = 0; i < number_of_joints + 1; i++)
        vC[i] = vT[i].cast<double>();

    for (int i = 0; i < number_of_joints; i++) {
        SetTranslation(CT.row(2*(i+1) - 1), i);
    }

}
void Snake::SetTranslation(const Eigen::Vector3d& position, int id)
{
    Eigen::Vector3f oldPositionOfObject = (bones[id]->Tout * bones[id]->Tin).matrix().block(0, 3, 3, 1);
    bones[id]->Tout.pretranslate(position.cast<float>() - oldPositionOfObject);
}
Eigen::Matrix4d Snake::getHeadMakeTransd()
{

    Eigen::Affine3d a = Eigen::Affine3d::Identity();
    a.translate((Eigen::Vector3d)CT.row(31));
    a.rotate(quat);
    return a.matrix();
}


void Snake::moveSnake(Eigen::Vector3d t){
    for (int i = 0; i < number_of_joints; i++)
        vT[i] = vT[i] + (vT[i + 1] - vT[i]) / 10;
    vT[number_of_joints] = vT[number_of_joints] + t;
    //update the joints
    for (int i = 0; i < number_of_joints; i++) {
        quat = Eigen::Quaterniond::FromTwoVectors(vC[i].cast<double>() - vC[i - 1], vT[i].cast<double>() - vC[i - 1]);

        Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
        mat.col(3) << bones[i]->Tin.translation(), 1;



        Eigen::Quaterniond q = Eigen::Quaterniond(( (bones[i]->Tout.matrix() * mat).block<3, 3>(0, 0)).cast<double>());

        bones[i]->Tout.rotate(((q * quat.conjugate()) * q.conjugate()).cast<float>());
    }
}
