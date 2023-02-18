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
        void GenerateCubeObject(const std::shared_ptr<Material>& _material, std::string prefix);
        void GenerateSphereObject(const std::shared_ptr<Material>& _material, std::string prefix);
        void moveAccordingToBeizerCurve(Model *model);
        void setModelBezier(Eigen::Vector3f vectors, std::shared_ptr<Model> model);
        std::shared_ptr<Model> generateObjectBezier(int material_id, int model_id, std::string name, float scale);
        void drawTheBeizerCurve(std::shared_ptr<Model> model);
        Eigen::Vector3f calcForDraw(float ti, std::shared_ptr<Model> model);
        std::vector<double> linspace(float start_in, float end_in, int num_in);
    private:
        std::shared_ptr<Material> material;
        std::shared_ptr<Program> program;
        BasicScene *basicScene;
        bool is_visitor_inited = false;
        float minx = -10.0f;
        float maxx = 10.0f;
        float miny = -5.0f;
        float maxy = 5.0f;
        float minz = -5.0f;
        float maxz = 5.0f;
        float generate_random_number(float min, float max);


    };
}


