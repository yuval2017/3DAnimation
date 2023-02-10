
#include "MeshCollisionVisitor.h"
#include "Visitor.h"
#include "Camera.h"
#include "../tutorial/Final/BasicScene.h"
#include <utility>
#include <memory>
#include "iostream"
#include "Model.h"
#include "Scene.h"
using namespace std;
void cg3d::MeshCollisionVisitor::Run(cg3d::Scene *scene, cg3d::Camera *camera) {
    Visitor::Run(scene, camera);
}


void cg3d::MeshCollisionVisitor::Visit(Scene *scene){
    BasicScene *_scene = (BasicScene *)scene;
    if (false){
        auto snake = _scene->snake->GetSnakeBones()[0];
        for(std::shared_ptr<Model> model: _scene->game_models){
            igl::AABB<Eigen::MatrixXd, 3>* treeA = (snake)->GetTree();

            igl::AABB<Eigen::MatrixXd, 3>* treeB = model->GetTree();

            if(isMeshCollision(snake,model,((snake)->GetTree()),model->GetTree())){
                scene->animate = false;
                std::cout<< "collision with " <<model->name<< " \n"<< std::endl;

            }
        }
    }

}

bool cg3d::MeshCollisionVisitor::isMeshCollision(std::shared_ptr<cg3d::Model> mesh1, std::shared_ptr<cg3d::Model> mesh2, igl::AABB<Eigen::MatrixXd, 3>* treeA, igl::AABB<Eigen::MatrixXd, 3>* treeB){
    //base cases

    if (treeA == nullptr || treeB == nullptr){
        std::cout<< "not good here!!!!!" << " \n"<< std::endl;

        return false;
    }


    if (!isBoxesIntersect(treeA->m_box, treeB->m_box, mesh1, mesh2)) {
        return false;
    }
    if (treeA->is_leaf() && treeB->is_leaf()) {
        //if the smallest box intersect so draw the boxes on each mesh
        //setCube(mesh1 ,treeA->m_box,true,true,true);
        //setCube(mesh2 ,treeB->m_box,true,true,true);
        return true;
    }

    if (treeA->is_leaf() && !treeB->is_leaf()) {

        return isMeshCollision(mesh1, mesh2, treeA, treeB->m_right) ||
               isMeshCollision(mesh1, mesh2, treeA, treeB->m_left);
    }
    if (!treeA->is_leaf() && treeB->is_leaf()) {
        return isMeshCollision(mesh1, mesh2, treeA->m_right, treeB) ||
               isMeshCollision(mesh1, mesh2, treeA->m_left, treeB);
    }
    return isMeshCollision(mesh1, mesh2, treeA->m_left, treeB->m_left) |
           isMeshCollision(mesh1, mesh2, treeA->m_left, treeB->m_right) |
           isMeshCollision(mesh1, mesh2, treeA->m_right, treeB->m_left) |
           isMeshCollision(mesh1, mesh2, treeA->m_right, treeB->m_right);
}
bool cg3d::MeshCollisionVisitor::isBoxesIntersect(Eigen::AlignedBox<double, 3>& boxA, Eigen::AlignedBox<double, 3>& boxB, const std::shared_ptr<cg3d::Model>& mesh1,const std::shared_ptr<cg3d::Model> &mesh2){
    // matrix A
    Eigen::Matrix3d A = mesh1->GetRotation().cast<double>();
    Eigen::Vector3d A0 = A.col(0);
    Eigen::Vector3d A1 = A.col(1);
    Eigen::Vector3d A2 = A.col(2);

    // matrix B
    Eigen::Matrix3d B = mesh2->GetRotation().cast<double>();
    Eigen::Vector3d B0 = B.col(0);
    Eigen::Vector3d B1 = B.col(1);
    Eigen::Vector3d B2 = B.col(2);


    //C=A^T*B
    //calculate C so that: C_ij = A_iXB_j
    Eigen::Matrix3d C = A.transpose() * B;

    //length sides of bounding box
    Eigen::Vector3d a = boxA.sizes() * mesh1->scale_factor;
    Eigen::Vector3d b = boxB.sizes() * mesh2->scale_factor;

    a = a / 2;
    b = b / 2;
    //create matrix D
    Eigen::Vector4d CA = Eigen::Vector4d(boxA.center()[0], boxA.center()[1], boxA.center()[2], 1);
    Eigen::Vector4d CB = Eigen::Vector4d(boxB.center()[0], boxB.center()[1], boxB.center()[2], 1);
    Eigen::Vector4d DOfVector4d = mesh2->GetTransform().cast<double>() * CB - mesh1->GetTransform().cast<double>() * CA;
    Eigen::Vector3d D = DOfVector4d.head(3);
//    Eigen::Vector3d CA = Eigen::Vector3d(boxA.center()[0], boxA.center()[1], boxA.center()[2]);
//    Eigen::Vector3d CB = Eigen::Vector3d(boxB.center()[0], boxB.center()[1], boxB.center()[2]);
//    Eigen::Vector3d D = sphere2->Tout.cast<double>() * CB - sphere1->Tout.cast<double>()*CA;


    double left, right, R;


    //check the 15 conditions for R > R_0 + R_1

    //A conditions:

    //A_0
    return !(a(0) + (b(0) * abs(C.row(0)(0)) + b(1) * abs(C.row(0)(1)) + b(2) * abs(C.row(0)(2))) < abs(A0.transpose() * D)
             ||
             //A_1
             (a(1) + (b(0) * abs(C.row(1)(0)) + b(1) * abs(C.row(1)(1)) + b(2) * abs(C.row(1)(2))) < abs(A1.transpose() * D))
             ||
             //A_2
             (a(2) + (b(0) * abs(C.row(2)(0)) + b(1) * abs(C.row(2)(1)) + b(2) * abs(C.row(2)(2))) < abs(A2.transpose() * D))
             ||

             //B conditions:

             //B_0
             (b(0) + (a(0) * abs(C.row(0)(0)) + a(1) * abs(C.row(1)(0)) + a(2) * abs(C.row(2)(0))) < abs(B0.transpose() * D))
             ||
             //B_1
             (b(1) + (a(0) * abs(C.row(0)(1)) + a(1) * abs(C.row(1)(1)) + a(2) * abs(C.row(2)(1))) < abs(B1.transpose() * D))
             ||
             //B_2
             (b(2) + (a(0) * abs(C.row(0)(2)) + a(1) * abs(C.row(1)(2)) + a(2) * abs(C.row(2)(2))) < abs(B2.transpose() * D))

             ||

             //A_0XB_0
             (left = C.row(1)(0) * A2.transpose() * D,
              right = C.row(2)(0) * A1.transpose() * D,
              R = left-right,
                     (a(1) * abs(C.row(2)(0)) + a(2) * abs(C.row(1)(0)) + b(1) * abs(C.row(0)(2))+ b(2) * abs(C.row(0)(1)) < abs(R)))

             ||

             //A_0XB_1
             (left = C.row(1)(1) * A2.transpose() * D,
              right = C.row(2)(1) * A1.transpose() * D,
              R = left-right,
                     (a(1) * abs(C.row(2)(1)) + a(2) * abs(C.row(1)(1)) + b(0) * abs(C.row(0)(2)) + b(2) * abs(C.row(0)(0)) < abs(R)))

             ||

             //A_0XB_2
             (left = C.row(1)(2) * A2.transpose() * D,
              right = C.row(2)(2) * A1.transpose() * D,
              R = left-right,
                     (a(1) * abs(C.row(2)(2)) + a(2) * abs(C.row(1)(2)) + b(0) * abs(C.row(0)(1)) + b(1) * abs(C.row(0)(0)) < abs(R)))

             ||

             //A_1XB_0

             (left = C.row(2)(0) * A0.transpose() * D,
              right = C.row(0)(0) * A2.transpose() * D,
              R = left-right,
                     (a(0) * abs(C.row(2)(0)) + a(2) * abs(C.row(0)(0)) + b(1) * abs(C.row(1)(2)) + b(2) * abs(C.row(1)(1)) < abs(R)))

             ||

             //A_1XB_1

             (left = C.row(2)(1) * A0.transpose() * D,
              right = C.row(0)(1) * A2.transpose() * D,
              R = left-right,
                     (a(0) * abs(C.row(2)(1)) + a(2) * abs(C.row(0)(1)) + b(0) * abs(C.row(1)(2)) + b(2) * abs(C.row(1)(0)) < abs(R)))

             ||

             //A_1XB_2


             (left = C.row(2)(2) * A0.transpose() * D,
              right = C.row(0)(2) * A2.transpose() * D,
              R = left-right,
                     (a(0) * abs(C.row(2)(2)) + a(2) * abs(C.row(0)(2)) + b(0) * abs(C.row(1)(1)) + b(1) * abs(C.row(1)(0)) < abs(R)))

             ||
             //A2 conditions:


             //A_2XB_0

             (left = C.row(0)(0) * A1.transpose() * D,
              right = C.row(1)(0) * A0.transpose() * D,
              R = left-right,
                     (a(0) * abs(C.row(1)(0)) + a(1) * abs(C.row(0)(0)) + b(1) * abs(C.row(2)(2)) + b(2) * abs(C.row(2)(1)) < abs(R)))

             ||

             //A_2XB_1
             (left = C.row(0)(1) * A1.transpose() * D,
              right= C.row(1)(1) * A0.transpose() * D,
              R = left-right,
                     (a(0) * abs(C.row(1)(1)) + a(1) * abs(C.row(0)(1)) + b(0) * abs(C.row(2)(2)) + b(2) * abs(C.row(2)(0)) < abs(R)))

             ||
             //A_2XB_2
             (left = C.row(0)(2) * A1.transpose() * D,
              right = C.row(1)(2) * A0.transpose() * D,
              R = left-right,
                     (a(0) * abs(C.row(1)(2)) + a(1) * abs(C.row(0)(2)) + b(0) * abs(C.row(2)(1)) + b(1) * abs(C.row(2)(0)) < abs(R)))
    );
}
