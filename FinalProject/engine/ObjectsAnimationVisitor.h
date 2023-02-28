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
#include <set>
#include <iterator>
namespace cg3d {
    class ObjectsAnimationVisitor : public Visitor {
    public:
        void Run(Scene *scene, Camera *camera) override;
        void Visit(Model *model) override;
        void Visit(Scene *scene) override;
        void moveAccordingToBeizerCurve(Model *model);
        void setModelBezier(Eigen::Vector3f vectors, Model *model);
        std::shared_ptr<Model> generateObjectBezier(int material_id, int model_id, std::string name, float scale);
        void drawTheBeizerCurve(Model *model);
        void CreateLevel1(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords);
        void CreateLevel2(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords);
        void CreateLevel3(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords);
        std::vector<std::shared_ptr<Model>> models;
        std::vector<Eigen::Vector3f> coords;
        void removeFormerlevel(int index );
        void loadNextLevel(int nextLevel);
        //create
        shared_ptr<Model> createFrog();
        shared_ptr<Model> createMouse();
        shared_ptr<Model> createCoin();
        shared_ptr<Model> createSphere();
        shared_ptr<Model> createBrick();
        shared_ptr<Model> createFrogBezier();
        //remove
        void removeFrog(const shared_ptr<Model>& frog);
        void removeMouse(const shared_ptr<Model>& mouse);
        void removeCoin(const shared_ptr<Model>& coin);
        void removeBezier(const shared_ptr<Model>& coin);
        void removeSphere(const shared_ptr<Model>& sphere);
        void removeBricks(const shared_ptr<Model>& cube);



        void get_map_max_min(Eigen::Vector3f &max, Eigen::Vector3f &min);

    private:
        std::vector<std::shared_ptr<Model>> checkForRemove(const std::vector<std::shared_ptr<Model>>& objects_in_use);
        void rescaleObjects(float bez_speed);
        void GetCurrMapMaxLength(float &length_x, float &length_y, float &length_z);
        std::shared_ptr<Material> material;
        std::shared_ptr<Program> program;
        BasicScene *basicScene;
        bool is_visitor_inited = false;
        float generate_random_number(float min, float max);
        void init_point_givers(float div_map);
        Stopper* stopperFrog;
        Stopper* stopperMouse;
        Stopper* stopperCoin;
        Stopper* snakeSpeed;
        Stopper* stopperSpecialBezier;
        Stopper *stopperBezier;

        std::queue<Eigen::Vector3f > frogPoints;
        std::queue<Eigen::Vector3f > bezierPoints;
        std::queue<Eigen::Vector3f > mousePoints;
        std::queue<Eigen::Vector3f > coinPoints;
        std::vector<std::shared_ptr<Model>> frogs_not_in_use;
        std::vector<std::shared_ptr<Model>> frogs_in_use;
        std::vector<std::shared_ptr<Model>> mouses_not_in_use;
        std::vector<std::shared_ptr<Model>> mouses_in_use;
        std::vector<std::shared_ptr<Model>> coins_not_in_use;
        std::vector<std::shared_ptr<Model>> coins_in_use;
        std::vector<std::shared_ptr<Model>> special_bezier_in_use;
        std::vector<std::shared_ptr<Model>> special_bezier_not_in_use;
        std::vector<std::shared_ptr<Model>> bezier_in_use;
        std::vector<std::shared_ptr<Model>> bezier_not_in_use;
        std::vector<std::shared_ptr<Model>> bricks_in_use;
        std::vector<std::shared_ptr<Model>> bricks_not_in_use;
        std::shared_ptr<Model> addModelToGame(std::vector<std::shared_ptr<Model>> &objects_in_use, std::vector<std::shared_ptr<Model>> &objects_not_in_use);
        void removeModelFromGame(std::vector<std::shared_ptr<Model>> &objects_in_use, std::vector<std::shared_ptr<Model>> &objects_not_in_use, const std::shared_ptr<Model>& model);
        void clearAllAliveObjects(std::vector<std::shared_ptr<Model>> &objects_in_use, std::vector<std::shared_ptr<Model>> &objects_not_in_use);
        void generatePointsInMapLevel(std::vector<Eigen::Vector3f> &coords, int n);


        // Generate 3 points every sec1, sec2, sec3 seconds for len1, len2, len3 seconds
        int sec1 = 5; // generate frog points every 2 seconds
        int len1 = 70; // frog points should disappear after 10 seconds
        int sec2 = 5; // generate mouse points every 3 seconds
        int len2 = 70; // mouse points should disappear after 7 seconds
        int sec3 = 7; // generate cube points every 5 seconds
        int len3 = 30; // cube points should disappear after 5 seconds
        int sec_between_specialbezier = 10;
        int sec_bezier = 10;
        int len_bezier = 100;
        int number_of_points = 45;
        Calculates* calculates;
        float frog_scale = 0.8f;
        float mouse_scale = 0.7f;
        float coin_scale = 1.0f;
        float brick_scale = 2.0f;
        float sphere_scale = 1.0f;
        Eigen::Vector3f get_point( std::queue<Eigen::Vector3f> &coords);
    };
}


