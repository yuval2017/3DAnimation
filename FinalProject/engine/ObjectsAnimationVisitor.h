//
// Created by יובל היטר on 12/02/2023.
//


#pragma once
#include "../tutorial/Final/BasicScene.h"
#include "Visitor.h"
#include "Model.h"
#include <queue>
#include <random>
#include <algorithm>
#include <cmath>
#include "../tutorial/Final/Calculates.h"
namespace cg3d {
    class ObjectsAnimationVisitor : public Visitor {
    public:
        void Run(Scene *scene, Camera *camera) override;
        void Visit(Model *model) override;
        void moveAccordingToBeizerCurve(Model *model);
        void setModelBezier(Eigen::Vector3f vectors, std::shared_ptr<Model> model);
        std::shared_ptr<Model> generateObjectBezier(int material_id, int model_id, std::string name, float scale);
        void drawTheBeizerCurve(std::shared_ptr<Model> model);
        void CreateLevel1(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords);
        void CreateLevel2(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords);
        void CreateLevel3(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords);
        std::vector<std::shared_ptr<Model>> models;
        std::vector<Eigen::Vector3f> coords;
        void removeFormerlevel();
        void loadNextLevel(int nextLevel);
        shared_ptr<Model> createFrog();
        shared_ptr<Model> createMouse();
        shared_ptr<Model> createCoin();

    private:
        void GetCurrMapMaxLength(float &length_x, float &length_y, float &length_z);
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
        void init_point_givers(float x_length , float y_length, float z_length);
        Stopper* stopperFrog;
        Stopper* stopperMouse;
        Stopper* stopperCoin;
        Stopper* snakeSpeed;
        std::queue<Eigen::Vector3f > frogPoints;
        std::queue<Eigen::Vector3f > mousePoints;
        std::queue<Eigen::Vector3f > coinPoints;
        std::vector<shared_ptr<Model>> frogs_available;
        std::vector<shared_ptr<Model>> mouses_available;
        std::vector<shared_ptr<Model>> coins_available;
        // Generate 3 points every sec1, sec2, sec3 seconds for len1, len2, len3 seconds
        double sec1 = 2.0; // generate frog points every 2 seconds
        double len1 = 4.0; // frog points should disappear after 10 seconds
        double sec2 = 2.0; // generate mouse points every 3 seconds
        double len2 = 5.0; // mouse points should disappear after 7 seconds
        double sec3 = 2.0; // generate cube points every 5 seconds
        double len3 = 4.0; // cube points should disappear after 5 seconds
        int min_dist = 10;
        int num_of_points = 70;
        int num_of_models = 15;
        Calculates* calculates ;
    };
}


