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
#include "../tutorial/Final/Calculates.h"


namespace cg3d
{

class MeshCollisionVisitor :public Visitor
    {
public:
            void Run(Scene *scene, Camera *camera)
            override;
//            void Visit(Scene *scene)
//            override;
            void Visit(Model *model) override;
            void handle_object_hit(Model *model);
            void handle_self_hit();
            void handle_eating(Model* model);

protected:
    BasicScene *basicScene;
    };
}

