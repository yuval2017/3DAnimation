//
// Created by יובל היטר on 12/02/2023.
//


#pragma once
#include "../tutorial/Final/BasicScene.h"
#include "Visitor.h"
#include "Model.h"
#include <vector>
namespace cg3d {
    class ObjectsAnimationVisitor : public Visitor {

    public:
        void Run(Scene *scene, Camera *camera) override;
        void Visit(Model *model) override;
        void GenerateCubeObject(const std::shared_ptr<Material>& _material);
        void GenerateSphereObject(const std::shared_ptr<Material>& _material);

    private:
        std::shared_ptr<Material> material;
        std::shared_ptr<Program> program;
        BasicScene *basicScene;
        bool is_visitor_inited = false;
        int minx = -6;
        int maxx = 6;
        int generate_random_number(int min, int max);

    };
}


