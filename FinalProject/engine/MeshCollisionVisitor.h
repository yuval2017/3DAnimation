//
// Created by יובל היטר on 16/01/2023.
//
#pragma once
#include "Visitor.h"
#include <vector>
#include <AABB.h>
#include "igl/read_triangle_mesh.cpp"
#include "igl/edge_flaps.h"
#include "igl/per_vertex_normals.h"
#include "../tutorial/Final/BasicScene.h"


namespace cg3d
{

class MeshCollisionVisitor :public Visitor
    {
public:
            void Run(Scene *scene, Camera *camera)
            override;
            void Visit(Scene *scene)
            override;

protected:
    bool isMeshCollision (std::shared_ptr<cg3d::Model> mesh1, std::shared_ptr<cg3d::Model> mesh2,igl::AABB<Eigen::MatrixXd, 3>* treeA, igl::AABB<Eigen::MatrixXd, 3>* treeB);
    bool isBoxesIntersect(Eigen::AlignedBox<double, 3>& boxA, Eigen::AlignedBox<double, 3>& boxB, const std::shared_ptr<cg3d::Model>& mesh1, const std::shared_ptr<cg3d::Model> &mesh2);
    BasicScene *basicScene;
    };
}

