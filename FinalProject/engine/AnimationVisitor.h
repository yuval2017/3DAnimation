//
// Created by Ofir Gilad on 06/02/2023.
//

#pragma once
#include "../tutorial/Final/BasicScene.h"
#include "Visitor.h"
#include "Model.h"
#include <vector>


namespace cg3d
{
    class AnimationVisitor : public Visitor
    {
    public:
        void Run(Scene* scene, Camera* camera) override;
        void Visit(Model* model) override;

    private:
        std::vector<Model*> models;
        BasicScene* basicScene;
        Eigen::Quaternionf quaternion;
        int bone_index = 0;
        int number_of_bones = 2; // without the first bone
        bool bones_found_flag = false;
    };
}