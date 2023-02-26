//
// Created by יובל היטר on 10/02/2023.
//

#include "Snake.h"

#include <utility>
#include "Movable.h"
#include "IglMeshLoader.h"
#include <igl/deform_skeleton.h>
#include <igl/dqs.h>
#include <igl/directed_edge_parents.h>
#include <igl/per_vertex_normals.h>
#include "Movable.h"
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <imgui/imgui.h>
#include <iostream>
#include <vector>
#include <forward_kinematics.h>
#include "ModelsFactory.h"
#include "Calculates.h"
#include "ObjLoader.h"

Snake::Snake(){
    std::cout<< "Snake :) " << " \n"<< std::endl;
}
Snake::Snake(const std::shared_ptr<cg3d::Material>& material, const std::shared_ptr<cg3d::Movable>& root, std::shared_ptr<cg3d::Camera> _camera){
    ModelsFactory *factory = ModelsFactory::getInstance();
    this->root = root;

    //for check
    auto program1 = std::make_shared<Program>("shaders/pickingShader");
    auto material1{ std::make_shared<Material>("material", program1)};
    Eigen::MatrixXd vertices(6,3);
    vertices << -1,0,0,1,0,0,0,-1,0,0,1,0,0,0,-1,0,0,1;
    Eigen::MatrixXi faces(3,2);
    faces << 0,1,2,3,4,5;
    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(6,3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(6,2);
    auto coordsys = std::make_shared<Mesh>("coordsys",vertices,faces,vertexNormals,textureCoords);
    auto axis = (Model::Create("axis",coordsys,material1));
    axis->mode = 1;
    axis->Scale(4,Model::Axis::XYZ);
    root->AddChild(axis);





    camera = std::move(_camera);

    std::cout<< joint_length << std::endl;

    initSnake();

}

std::vector<std::shared_ptr<cg3d::Model>> Snake::GetSnakeBones(){
    return bones;
}
void Snake::SetSpeed(float new_speed){
    speed = new_speed;
}
void Snake::MoveLeft(){
    direction = Eigen::Vector3d(speed, 0, 0.7f);
//    float angle_radians = -20.0f * M_PI / 180.0f;
//    bones[bones.size() - 1]->Rotate(Eigen::Quaternionf(cos(angle_radians / 2.0), 0.0, sin(angle_radians / 2.0), 0.0).toRotationMatrix());
//    if(!with_skinning) {
//        bones[0]->Rotate(0.1f, cg3d::Movable::Axis::Y);
//        bones[1]->Rotate(-0.1f, cg3d::Movable::Axis::Y);
//    }
}
void Snake::MoveRight(){
    direction = Eigen::Vector3d(-speed, 0, 0.7f);
//    float angle_radians = 20.0f * M_PI / 180.0f;
//    bones[bones.size() - 1]->Rotate(Eigen::Quaternionf(cos(angle_radians / 2.0), 0.0, sin(angle_radians / 2.0), 0.0).toRotationMatrix());
//    if(!with_skinning) {
//
//        bones[0]->Rotate(-0.1f, cg3d::Movable::Axis::Y);
//        bones[1]->Rotate(0.1f, cg3d::Movable::Axis::Y);
//    }
}
void Snake::MoveUp(){
    direction = Eigen::Vector3d(0, speed, 0.7f);
//    float angle_radians = 20.0f * M_PI / 180.0f;
//    bones[bones.size() - 1]->Rotate(Eigen::Quaternionf(cos(angle_radians / 2.0), sin(angle_radians / 2.0), 0.0, 0.0).toRotationMatrix());
    //direction = Eigen::Vector3d(0, 0.8f, 0);
//    if(!with_skinning) {
//        bones[0]->Rotate(0.1f, cg3d::Movable::Axis::X);
//        bones[1]->Rotate(-0.1f, cg3d::Movable::Axis::X);
//    }
}

void Snake::MoveDone(){
    direction = Eigen::Vector3d(0, -speed, 0.7f);
//    float angle_radians = -20.0f * M_PI / 180.0f;
//    bones[bones.size() - 1]->Rotate(Eigen::Quaternionf(cos(angle_radians / 2.0), sin(angle_radians / 2.0), 0.0, 0.0).toRotationMatrix());
//    if(!with_skinning) {
//        bones[0]->Rotate(-0.1f, cg3d::Movable::Axis::X);
//        bones[1]->Rotate(0.1f, cg3d::Movable::Axis::X);
//    }
}


void Snake::calcWeight(Eigen::MatrixXd& V, double min_z){
    int bones_size = number_of_joints;
    int n = snake->GetMeshList()[0]->data[0].vertices.rows();
    W=Eigen::MatrixXd::Zero( n,bones_size+1);
    for (int i = 0; i < n; i++) {
        double curr_z = V.row(i)[2];
        for (int j = 0; j < bones_size; j++) {
            if (curr_z >= min_z + joint_length * float(j) && curr_z <= min_z + joint_length * float(j + 1)) {
                //double res = abs(curr_z - (min_z + joint_length * float(j + 1)));
                double res = (joint_length - abs(curr_z - (min_z + joint_length * j)))/joint_length;
//                double res = abs(curr_z - (min_z*joint_length*(i+1)));
//                double res = abs(curr_z - (min_z*joint_length*(i+1)));
                W.row(i)[j] = res;
                W.row(i)[j + 1] = 1-res ;
                break;
            }
        }
    }
//    int n = snake->GetMeshList()[0]->data[0].vertices.rows();
//    W=Eigen::MatrixXd::Zero( n,number_of_joints+1);
//    for (int i = 0; i < n; i++) {
//        double curr_z = V.row(i)[2];
//        for (int j = 0; j < number_of_joints+1; j++) {
//            if (curr_z >= min_z + joint_length * j && curr_z <= min_z + joint_length * (j + 1)) {
//                double res = abs(curr_z - (min_z + joint_length * (j + 1))) * 10;
//                W.row(i)[j] = res;
//                W.row(i)[j + 1] = 1-res ;
//                break;
//            }
//        }
//
//    }


//    W = Eigen::MatrixXd::Zero(V.rows(), Cp.rows() - 1);
//    for (size_t i = 0; i < V.rows(); i++)
//    {
//        Eigen::Vector3f v = V.row(i).cast<float>().eval();
//        auto res = getDistanceFromColsestJoints(v, Cp);
//        auto weights = CalculateWeightByDistances(res[0], res[2], res[1], res[3]);
//        W.row(i)[(int)res[0]] = weights[0];
//        W.row(i)[(int)res[1]] = weights[1];
        }




void Snake::skinning(Eigen::Vector3d t) {
    if (with_skinning) {
        //moveSnake2(std::move(t));
        //IKFabric();
    }
    //moving the cyls here...
    for (int i = 0; i < number_of_joints; i++) {
        vT[i] = ikGetPosition(i, -joint_length / 2).cast<double>() - (Eigen::Vector3d) Cp.row(i);
    }
    vT[number_of_joints] = ikGetPosition(number_of_joints - 1, joint_length / 2).cast<double>() -
                           (Eigen::Vector3d) Cp.row(number_of_joints);



//    for (int i = 0; i < number_of_joints; i++)
//        vT[i] = vT[i] + (vT[i + 1] - vT[i])/10;
//    Eigen::Vector3d new_pos = bones[number_of_joints-1]->GetRotation().cast<double>()*t;
    //vT[number_of_joints] = vT[number_of_joints] + new_pos;

    std::vector<Eigen::Vector3f> p;

    std::vector<double> ris_Array;
    std::vector<double> lambdaI_Arra;
    Calc_Next_Position(p,ris_Array,lambdaI_Arra,t);



    std::cout<< bones[number_of_joints-1]->GetRotation().cast<double>() << "\n"<< std::endl;


    const int dim = Cp.cols();
    Eigen::MatrixXd T(BE.rows() * (dim + 1), dim);
    for (int e = 0; e < BE.rows(); e++) {
        Eigen::Affine3d a = Eigen::Affine3d::Identity();
        a.translate(vT[e]);
        a.rotate(vQ[e]);
        T.block(e * (dim + 1), 0, dim + 1, dim) =
                a.matrix().transpose().block(0, 0, dim + 1, dim);
    }
    // the position minus the first position.

    igl::dqs(V_new, W, vQ, vT, U);
    igl::deform_skeleton(Cp, BE, T, CT, BET);


    for (int i = 1; i < number_of_joints; i++) {
        ikRotateHelper(i-1,p[i]);
    }
    ikRotateHelper(number_of_joints-1, p[number_of_joints]);

//    Eigen::MatrixXd VN;
//    igl::per_vertex_normals(U, snake->GetMesh()->data[0].faces, VN);
//    snake->setMeshData(snake->name,
//                    U,
//                    snake->GetMeshList()[0]->data[0].faces,
//                    VN,
//                    snake->GetMeshList()[0]->data[0].textureCoords);
//
//


    for (int i = 0; i < number_of_joints + 1; i++) {
        vC[i] = vT[i].cast<double>();
    }

    for (int i = 1; i < number_of_joints+1; i++) {
        //Eigen::Vector3d newPositionOfObject = CT.row(2*i-1);
        Eigen::Vector3f newPositionOfObject = p[i];
        Eigen::Matrix3f system = bones[i-1]->GetRotation().transpose();
        bones[i-1]->Translate( newPositionOfObject.cast<float>() - ikGetPosition(i-1,joint_length/2));
    }
    direction = {0.0f,0.0f,speed};
}





void Snake::moveSnake(Eigen::Vector3d t){
//    for (int i = 0; i < number_of_joints; i++)
//        vT[i] = vT[i] + (vT[i + 1] - vT[i]);
//    vT[number_of_joint] = vT[number_of_joint] + t;
//    //update the joints

//    for (int i = 0; i < number_of_joints; i++) {
//        quat = Eigen::Quaterniond::FromTwoVectors(vC[i].cast<double>() - vC[i - 1], vT[i].cast<double>() - vC[i - 1]);
//        bones[i]->Rotate(quat.cast<float>());
//    }
}
void Snake::reset_bones(){



    bones.resize(0);

    bones.push_back(ModelsFactory::getInstance()->CreateModel(BRICKS_MATERIAL, CYL, "bone 0"));
    //calculate joint length after scale
    bones[0]->Scale(scaleFactor,cg3d::Movable::Axis::Z);
    //calculate joint length
    joint_length = bones[0]->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff()[2] - bones[0]->GetMeshList()[0]->data[0].vertices.colwise().minCoeff()[2];
    //length with scale
    joint_length = joint_length * bones[0]->scale_factor[2];

    root->AddChild(bones[0]);
    bones[0]->SetCenter(Eigen::Vector3f(0,0,-(joint_length/2.0f)));
    bones[0]->Translate((joint_length/2.0f),cg3d::Movable::Axis::Z);
    bones[0]->Translate(-(number_of_joints)*joint_length,cg3d::Movable::Axis::Z);
    for(int i = 1;i < number_of_joints; i++)
    {
        bones.push_back(ModelsFactory::getInstance()->CreateModel(PHONG_MATERIAL, CYL, "bone " + std::to_string(i)));
        root->AddChild(bones[i]);
        bones[i]->Scale(scaleFactor,cg3d::Movable::Axis::Z);
        bones[i]->Translate(-joint_length*( number_of_joints - i- 1),cg3d::Movable::Axis::Z);
        bones[i]->Translate(-(joint_length)/2.0f,cg3d::Movable::Axis::Z);

        bones[i]->SetCenter(Eigen::Vector3f(0,0,-(joint_length)));
        //bones[i-1]->AddChild(bones[i]);
        bones[i]->GetTree();
    }
}
void Snake::reset_sake() {
    //remove from scene
    for(int i = 0;i < number_of_joints; i++)
    {
        root->RemoveChild(bones[i]);
    }
    reset_bones();
    skinning({0.0f,0.0f,0.0f});
}
void Snake::initSnake(){
    sphere1 = ModelsFactory::getInstance()->CreateModel(PHONG_MATERIAL,SPHERE,"junk_sphere");
    sphere1->showWireframe = true;
    with_skinning = false;
    number_of_joints = 16;

    reset_bones();

    //create snake
    snake = ModelsFactory::getInstance()->CreateModel(PHONG_MATERIAL,SNAKE1,"snake");
    root->AddChild(snake);
    snake->showWireframe = true;
    vC.resize(number_of_joints+1);
    BE.resize(number_of_joints, 2);
    Cp.resize(number_of_joints+1, 3);
    CT.resize(32, 3);
    // vQ - rotations of joints
    // vT - translation of joints
    vT.resize(number_of_joints + 1);
    vQ.resize(number_of_joints + 1, Eigen::Quaterniond::Identity());

    for (int i = 0; i < BE.rows(); i++) {
        BE.row(i) << i, i+1;
    }

    Eigen::MatrixXd vertices = snake->GetMeshList()[0]->data[0].vertices;
    Eigen::Vector3d translate = Eigen::Vector3d(0.0f,0.0f,-number_of_joints*(joint_length/2));
    Eigen::Vector3d scale = Eigen::Vector3d(1.0f,1.0f,number_of_joints);
    Eigen::Affine3d Tout{Eigen::Affine3d::Identity()}, Tin{Eigen::Affine3d::Identity()};
    Tout.pretranslate(translate);
    Tin.scale(scale);
    Eigen::MatrixX4d transform = Tout.matrix()*Tin.matrix();

    // Init new V
    V_new = Eigen::MatrixXd::Zero(vertices.rows(), vertices.cols());
    for (int i = 0; i < vertices.rows(); i++) {
        Eigen::Vector4d Vi = Eigen::Vector4d(vertices.row(i).x(), vertices.row(i).y(), vertices.row(i).z(), 1);
        Eigen::Vector4d V_new_i = transform * Vi;
        V_new.row(i) = Eigen::Vector3d(V_new_i[0], V_new_i[1], V_new_i[2]);
    }
    U = V_new;
    Eigen::MatrixXd VN;
    igl::per_vertex_normals(V_new, snake->GetMesh()->data[0].faces, VN);
    snake->setMeshData(snake->name,
                       V_new,
                       snake->GetMeshList()[0]->data[0].faces,
                       VN,
                       snake->GetMeshList()[0]->data[0].textureCoords);

    Eigen::Vector3d min = snake->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
    Eigen::Vector3d maxi = snake->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff();
    double min_z = min[2];
    Eigen::Vector3d pos(0, 0, min_z);

    // Init C
    Cp.resize(number_of_joints + 1, 3);
    for (int i = 0; i < number_of_joints+1; i++)
    {
        //float posi = ikGetPosition(i,-joint_length/2)[2];
        Cp.row(i) = pos;
        vT[i] = pos;
        vC[i] = pos;
        pos = pos + Eigen::Vector3d(0, 0, joint_length);
    }
    igl::directed_edge_parents(BE, P);

    Eigen::Vector3d min_vec = V_new.colwise().minCoeff();
    double minz = min_vec[2];
    calcWeight(V_new,minz);


}
Eigen::Vector3f Snake::ikGetPosition(int id, float length){
    Eigen::Vector3f cen = bones[id]->GetAggregatedTransform().col(3).head(3);
    Eigen::Vector3f mov = Eigen::Vector3f(0,0,length);
    Eigen::Vector3f res = (cen+(bones[id]->GetRotation()*mov));
    return res;
}


Eigen::Vector3f Snake::get_snake_head(){
    return ikGetPosition(number_of_joints-1, joint_length/2);
}




void Snake::ikRotateHelper(int id, const Eigen::Vector3f& t){
    Eigen::Vector3f r = ikGetPosition(id, -joint_length/2);
    Eigen::Vector3f e = ikGetPosition(id, joint_length/2);
    Eigen::Vector3f rd = t - r;
    Eigen::Vector3f re = e - r;
    Eigen::Vector3f normal = re.normalized().cross(rd.normalized());
    float dot = rd.normalized().dot(re.normalized());//get dot
//    std::cout << "the r is: " << r << std::endl;
//    std::cout << "the e is: " << e << std::endl;


    if (dot > 1)
        dot = 1;
    if (dot < -1)
        dot = 1;
    float angle = (float)acos(dot) ;
    Eigen::Vector3f rotationVec = (bones[id]->GetAggregatedTransform()).block<3, 3>(0, 0).inverse() * normal;
  //  std::cout << "the angel is: " << angle << std::endl;
    Eigen::Matrix3f rot = (Eigen::AngleAxisf(angle,rotationVec.normalized())).toRotationMatrix();
    Eigen::Vector3d oi = rot.cast<double>().eulerAngles(1,0,1);

//    bones[id]->Rotate(oi[0],Movable::Axis::Y);
//    bones[id]->Rotate(oi[1], Movable::Axis::X);
//    bones[id]->Rotate(oi[2],Movable::Axis::Y);
    bones[id]->Rotate(angle, rotationVec);
}




void Snake::Calc_Next_Position(std::vector<Eigen::Vector3f> &p, std::vector<double> &ris_Array, std::vector<double> &lambdaI_Array, Eigen::Vector3d t){
    int num_of_cyls = number_of_joints;
    int lastLinkIndex = bones.size() - 1;
    int firstLinkIndex = 0;
    float link_len = joint_length;
    std::vector<Eigen::Vector3f> _p;
    _p.resize(num_of_cyls + 1);

    int curr = lastLinkIndex;
    while (curr != -1) {
        _p[curr] = ikGetPosition(curr, -link_len / 2);
        curr = curr - 1;
    }
    _p[lastLinkIndex + 1] = ikGetPosition(lastLinkIndex, link_len / 2);

    std::vector<double> _ris_Array;
    std::vector<double> _lambdaI_Array;

    _ris_Array.resize(num_of_cyls + 1);
    _lambdaI_Array.resize(num_of_cyls + 1);
    _p[lastLinkIndex + 1] = ikGetPosition(lastLinkIndex,joint_length/2) + bones[lastLinkIndex - 1]->GetRotation()*t.cast<float>();
    int parent = lastLinkIndex;
    int child = lastLinkIndex + 1;
    while (parent != -1) {

        // Find the distance ri between the new joint position pi+1 and the joint pi
        //steps 1.24-1.25
        float disti = (_p[child] - _p[parent]).norm();
        _ris_Array[parent] = disti;
        float all_dist = (link_len / _ris_Array[parent]);
        _lambdaI_Array[parent] = all_dist;

        //step 1.27
        //calculate the new position.
        Eigen::Vector3f new_pos = (1 - _lambdaI_Array[parent]) * _p[child] + _lambdaI_Array[parent] * _p[parent];
        _p[parent] = new_pos;
        child = parent;
        parent = parent - 1;
    }
    lambdaI_Array = _lambdaI_Array;
    p = _p;
    ris_Array = _ris_Array;
}

