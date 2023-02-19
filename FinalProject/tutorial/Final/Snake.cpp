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
#include "ModelsFactory.h"
#include "Calculates.h"

Snake::Snake(){
    std::cout<< "Snake :) " << " \n"<< std::endl;
}
Snake::Snake(const std::shared_ptr<cg3d::Material>& material, const std::shared_ptr<cg3d::Movable>& root, std::shared_ptr<cg3d::Camera> _camera){
    ModelsFactory *factory = ModelsFactory::getInstance();

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



    sphere1 = ModelsFactory::getInstance()->CreateModel(PHONG_MATERIAL,SPHERE,"junk_sphere");
    sphere1->showWireframe = true;
    with_skinning = false;

    direction = {0, 0, 1.0f};
    bones.push_back(factory->CreateModel(BRICKS_MATERIAL, CYL, "bone 0"));
    bones[0]->Scale(scaleFactor,cg3d::Movable::Axis::Z);
    bones[0]->SetCenter(Eigen::Vector3f(0,0,-0.8f*scaleFactor));
    root->AddChild(bones[0]);
    number_of_joints = 16;
    for(int i = 1;i < number_of_joints; i++)
    {
        bones.push_back(factory->CreateModel(PHONG_MATERIAL, CYL, "bone " + std::to_string(i)));
        bones[i]->Scale(scaleFactor,cg3d::Movable::Axis::Z);
        bones[i]->Translate(1.6f*scaleFactor,cg3d::Movable::Axis::Z);
        bones[i]->SetCenter(Eigen::Vector3f(0,0,-0.8f*scaleFactor));
        bones[i-1]->AddChild(bones[i]);
        bones[i]->GetTreeWithOutCube();
        //root->AddChild(bones[i]);
    }
    camera = std::move(_camera);
    //bones[0]->AddChild(camera);
    snake = factory->CreateModel(PHONG_MATERIAL,SNAKE1,"snake");
    root->AddChild(snake);
    snake->showWireframe = true;


    joint_length = bones[0]->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff()[2] - bones[0]->GetMeshList()[0]->data[0].vertices.colwise().minCoeff()[2];
    float snake_length = joint_length * number_of_joints;
    std::cout<< joint_length << std::endl;
    //initJoints();
    //Calculates calculate = Calculates();
    auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","data/cube_old.obj")};
    auto cube = Model::Create( "helpcube", cubeMesh, material);
    //std::vector<TexCoord> Vts = Calculates::getInstance()->getVertexTextureCoordinates(cube->GetMeshList()[0]->data[0].vertices,cube->GetMeshList()[0]->data[0].faces,"../tutorial/textures/bricks.jpg");
//    bones[0]->Rotate(M_PI,Model::Axis::X);
    bones[0]->Translate({0,0,0.8f*scaleFactor});
    sphere1->Translate(-1.6f*scaleFactor*(number_of_joints - 0.5*number_of_joints),cg3d::Movable::Axis::Z);
    root->AddChild(sphere1);
    initSnake();

}

std::vector<std::shared_ptr<cg3d::Model>> Snake::GetSnakeBones(){
    return bones;
}
void Snake::SetSpeed(float new_speed){
    speed = new_speed;
}
void Snake::MoveLeft(){
    direction = Eigen::Vector3d(-1.0, 0, 0);
    if(!with_skinning) {
        bones[0]->Rotate(0.1f, cg3d::Movable::Axis::Y);
        bones[1]->Rotate(-0.1f, cg3d::Movable::Axis::Y);
    }
}
void Snake::MoveRight(){
    direction = Eigen::Vector3d(1.0, 0, 0);
    if(!with_skinning) {

        bones[0]->Rotate(-0.1f, cg3d::Movable::Axis::Y);
        bones[1]->Rotate(0.1f, cg3d::Movable::Axis::Y);
    }
}
void Snake::MoveUp(){
    direction = Eigen::Vector3d(0, 1.0, 0);
    if(!with_skinning) {
        bones[0]->Rotate(0.1f, cg3d::Movable::Axis::X);
        bones[1]->Rotate(-0.1f, cg3d::Movable::Axis::X);
    }
}
void Snake::MoveDone(){
    direction = Eigen::Vector3d(0, 1.0, 0);
    if(!with_skinning) {
        bones[0]->Rotate(-0.1f, cg3d::Movable::Axis::X);
        bones[1]->Rotate(0.1f, cg3d::Movable::Axis::X);
    }
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
    snake->setMeshData(snake->name,
                       V_new,
                       snake->GetMeshList()[0]->data[0].faces,
                       snake->GetMeshList()[0]->data[0].vertexNormals,
                       snake->GetMeshList()[0]->data[0].textureCoords);


    Eigen::Vector3d min = snake->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
    float min_z = min[2];
    Eigen::Vector3d pos(0, 0, min_z);
    Eigen::MatrixXd points(17, 3);
    for (int i = 0; i < bones_size + 1; i++) {
        vC[i] = pos;
        vT[i] = pos;
        points.row(i) = pos;
        pos = pos + Eigen::Vector3d(0, 0, joint_length);
    }

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
                //double res = abs(curr_z - (min_z + joint_length * float(j + 1)));
                double res = (joint_length - abs(curr_z - (min_z + joint_length * j)))/joint_length;
//                double res = abs(curr_z - (min_z*joint_length*(i+1)));
                W.row(i)[j] = res;
                W.row(i)[j + 1] = 1-res ;
                break;
            }
        }
    }
//    int n = V.rows();
//    W = Eigen::MatrixXd::Zero(n, number_of_joints + 1);
//    for (int i = 0; i < n; i++) {
//        double curr_z = V.row(i)[2];
//        for (int j = 0; j < number_of_joints + 1; j++) {
//            if (curr_z >= min_z + joint_length * j && curr_z <= min_z + joint_length * (j + 1)) {
//                // my way
//                double dist = abs(curr_z - (min_z + joint_length * j));
//                W.row(i)[j] = (joint_length - dist) / joint_length;
//                W.row(i)[j + 1] = 1 - W.row(i)[j];
////                std::cout << "curr_z: " << curr_z << " i: " << i << " jointLength: " << jointLength << " j: " << j << " dist: " << dist << " W.row(i)[j]: " << W.row(i)[j] << " W.row(i)[j + 1]: " << W.row(i)[j + 1] << std::endl;
//                break;
//            }
//        }
//    }
    std::cout<< W <<std::endl;
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
    const int dim = Cp.cols();
    Eigen::MatrixXd T(BE.rows() * (dim + 1), dim);


    for (int e = 0; e < BE.rows(); e++) {
        Eigen::Affine3d a = Eigen::Affine3d::Identity();
        a.translate(vT[e].cast<double>());
        a.rotate(vQ[e].cast<double>());
        T.block(e * (dim + 1), 0, dim + 1, dim) =
                a.matrix().transpose().block(0, 0, dim + 1, dim);
    }
    // the position minus the first position.

    igl::dqs(V_new, W, vQ, vT, U);
    //igl::deform_skeleton(Cp, BE, T, CT, BET);


    snake->setMeshData(snake->name,
                    U,
                    snake->GetMeshList()[0]->data[0].faces,
                    snake->GetMeshList()[0]->data[0].vertexNormals,
                    snake->GetMeshList()[0]->data[0].textureCoords);



    for (int i = 0; i < number_of_joints + 1; i++)
        vC[i] = vT[i].cast<double>();
//    if (with_skinning) {
//        for (int i = 0; i < number_of_joints; i++) {
//            Eigen::Vector3d newPositionOfObject = CT.row(2 * (i + 1) - 1);
//            bones[i]->Translate(newPositionOfObject.cast<float>());
//        }
//    }
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
//    for (int i = 0; i < number_of_joints; i++)
//        vT[i] = vT[i] + (vT[i + 1] - vT[i]);
//    vT[number_of_joint] = vT[number_of_joint] + t;
//    //update the joints

//    for (int i = 0; i < number_of_joints; i++) {
//        quat = Eigen::Quaterniond::FromTwoVectors(vC[i].cast<double>() - vC[i - 1], vT[i].cast<double>() - vC[i - 1]);
//        bones[i]->Rotate(quat.cast<float>());
//    }
}
void Snake::initSnake(){
    vC.resize(number_of_joints+1);
    BE.resize(number_of_joints, 2);
    Cp.resize(number_of_joints+1, 3);
    CT.resize(32, 3);

    rotationPropogation.resize(number_of_joints);
    translatePropogation.resize(number_of_joints);
    for (int i = 0; i < number_of_joints; i++) {
        rotationPropogation[i] = Eigen::Vector3d::Identity();
        translatePropogation[i] = Eigen::Vector3d(0, 0, -0.1f);
    }
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



    Eigen::MatrixXd vertices = snake->GetMeshList()[0]->data[0].vertices;
    Eigen::Vector3d translate = Eigen::Vector3d(0.0f,0.0f,number_of_joints*(joint_length/2));
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

    Eigen::Vector3d min = snake->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
    double min_z = min[2];
    Eigen::Vector3d pos(0, 0, min_z);
    // Init C
    Cp.resize(number_of_joints + 1, 3);
    for (int i = 0; i < number_of_joints+1; i++)
    {
        Cp.row(i) = pos;
        pos = pos + Eigen::Vector3d(0, 0, joint_length);
    }
    snake->setMeshData(snake->name,
                       V_new,
                       snake->GetMeshList()[0]->data[0].faces,
                       snake->GetMeshList()[0]->data[0].vertexNormals,
                       snake->GetMeshList()[0]->data[0].textureCoords);
    Eigen::Vector3d min_vec = V_new.colwise().minCoeff();
    double minz = min_vec[2];
    calcWeight(V_new,minz);

    // vQ - rotations of joints
    // vT - translation of joints
    vT.resize(number_of_joints + 1);
    vQ.resize(number_of_joints + 1, Eigen::Quaterniond::Identity());

}
Eigen::Vector3f Snake::ikGetPosition(int id, float length){
    Eigen::Vector3f cen = bones[id]->GetAggregatedTransform().col(3).head(3);
    Eigen::Vector3f mov = Eigen::Vector3f(0,0,length);
    Eigen::Vector3f res = (cen+(bones[id]->GetRotation()*mov));
    return res;
}




void Snake::IKFabric() {
    sphere1->Translate(direction.cast<float>()/10);
    int num_of_cyls = number_of_joints;
    int lastLinkIndex = bones.size() - 1;
    int firstLinkIndex = 0;
    float link_len = joint_length;
    if (1) {
        std::vector<Eigen::Vector3f> p;
        p.resize(num_of_cyls + 1);
        Eigen::Vector3f target = sphere1->GetAggregatedTransform().col(3).head(3);
        Eigen::Vector3f p1 = ikGetPosition(firstLinkIndex, -link_len / 2);
        target = ikGetPosition(lastLinkIndex,joint_length/2) + Eigen::Vector3f (0,0,-0.5);

        int curr = lastLinkIndex;
        while (curr != -1) {
            p[curr] = ikGetPosition(curr, -link_len / 2);
            curr = curr - 1;
        }
        p[lastLinkIndex + 1] = ikGetPosition(lastLinkIndex, link_len / 2);


        std::vector<double> ris_Array;
        std::vector<double> lambdaI_Array;

        ris_Array.resize(num_of_cyls + 1);
        lambdaI_Array.resize(num_of_cyls + 1);

        //if unreachable
//        if ((target - p1).norm() > link_len * float(num_of_cyls)) {
//            std::cout << "cannot reach" << std::endl;
//            //shouldAnimateFabrik = false;
//            return;
//        }
    if(1)  {
            //1.10
            //the target is reachable
            Eigen::Vector3f b = p[firstLinkIndex];

            //the distance from object more then  tolerance .
            Eigen::Vector3f endEffector = p[lastLinkIndex + 1];
            float tolerance = 0.05;

            float diffA = (endEffector - target).norm();
            if (diffA < tolerance) {
                std::cout << "distance : " << diffA << "\n" << std::endl;
                //shouldAnimateFabrik = false;
                return;
        }
             //   while (diffA > tolerance) {
                    //	1.19 foward to reach
                    p[lastLinkIndex + 1] = target;
                    int parent = lastLinkIndex;
                    int child = lastLinkIndex + 1;
                    while (parent != -1) {

                        // Find the distance ri between the new joint position pi+1 and the joint pi
                        //steps 1.24-1.25
                        ris_Array[parent] = (p[child] - p[parent]).norm();
                        lambdaI_Array[parent] = link_len / ris_Array[parent];

                        //step 1.27
                        //calculate the new position.
                        p[parent] = (1 - lambdaI_Array[parent]) * p[child] + lambdaI_Array[parent] * p[parent];
                        child = parent;
                        parent = parent - 1;
                    }
      //          }
                //rotate
                int currLink = firstLinkIndex;
                int target_id = firstLinkIndex + 1;
                while (target_id < num_of_cyls) {
                    bones[currLink]->Translate(p[target_id] - ikGetPosition(target_id,link_len/2));
                    ikRotateHelper(currLink, p[target_id]);
                    currLink = target_id;
                    target_id = target_id + 1;
                }
                ikRotateHelper(lastLinkIndex, p[lastLinkIndex + 1]);
                bones[lastLinkIndex]->Translate(p[lastLinkIndex+1] - ikGetPosition(lastLinkIndex, link_len/2));

            double distance = (target - ikGetPosition(lastLinkIndex, link_len / 2)).norm();
                if (distance < tolerance) {
                    //shouldAnimateFabrik = false;
                    std::cout << "distance: " << distance << std::endl;

                }
        }
    }
}




void Snake::ikRotateHelper(int id, const Eigen::Vector3f& t){
    Eigen::Vector3f r = ikGetPosition(id, -joint_length/2);
    Eigen::Vector3f e = ikGetPosition(id, joint_length/2);
    Eigen::Vector3f rd = t - r;
    Eigen::Vector3f re = e - r;
    Eigen::Vector3f normal = re.normalized().cross(rd.normalized());
    float dot = rd.normalized().dot(re.normalized());//get dot

    if (dot > 1)
        dot = 1;
    if (dot < -1)
        dot = 1;
    float angle = (float)acos(dot)/100 ;
    Eigen::Vector3f rotationVec = (bones[id]->GetAggregatedTransform()).block<3, 3>(0, 0).inverse() * normal;

    Eigen::Matrix3f rot = (Eigen::AngleAxisf(angle,rotationVec.normalized())).toRotationMatrix();
    Eigen::Vector3d oi = rot.cast<double>().eulerAngles(2,0,2);

    bones[id]->Rotate(oi[0],Movable::Axis::Z);
    bones[id]->Rotate(oi[1], Movable::Axis::X);
    bones[id]->Rotate(oi[2],Movable::Axis::Z);
    bones[id]->Rotate(angle, rotationVec);
}
void Snake::moveSnake2(Eigen::Vector3d t){
    //the rotation on the end -
    //translatePropogation[0] = Eigen::Vector3d(0, 0, -0.1f);
    //rotationPropogation[0] = direction;
    for (int i = 0; i < number_of_joints; i++) {
        Eigen::Matrix3f system = bones[i]->GetRotation().transpose();
        bones[i]->TranslateInSystem(system, translatePropogation[i].cast<float>());
        //bones[i]->Rotate(move,rotationPropogation[i].cast<float>());
    }
    IKFabric();

    //
    for (int i = 1; i < number_of_joints; i++) {
        rotationPropogation[i] = rotationPropogation[i-1];
        translatePropogation[i-1] = translatePropogation[i];
    }

}
void Snake::propTranslate(Eigen::Vector3d t ,int id){
    for (int i = id+1; i < number_of_joints; i++) {
        bones[i]->Translate(ikGetPosition(i-1,number_of_joints/2)-ikGetPosition(i,number_of_joints/2));
    }
}
