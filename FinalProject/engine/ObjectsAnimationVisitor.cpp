//
// Created by יובל היטר on 12/02/2023.
//

#include "ObjectsAnimationVisitor.h"
#include "../tutorial/Final/IglMeshLoader.h"
#include "../tutorial/Final/ModelsFactory.h"
#include <igl/per_vertex_normals.h>
#include <iostream>
#include <random>
#include <utility>
#include <igl/per_vertex_normals.h>

//#include "../tutorial/Final/Calculates.h"
#include "../tutorial/Final//ObjectsNames.h"
#include "../tutorial/Final//ModelsFactory.h"
#include "iostream"


void ObjectsAnimationVisitor::Run(Scene *scene, Camera *camera) {
    basicScene = (BasicScene *) scene;
    calculates = Calculates::getInstance();
    //The point objects consts for the run method.
    static std::vector<Eigen::Vector3d> frog_points;
    static std::vector<Eigen::Vector3d> mouse_points;
    static std::vector<Eigen::Vector3d> coin_points;


    //The init part.
    if (!is_visitor_inited) {
        float x_length;
        float y_length;
        float z_length;
        //system coordinates.
        GetCurrMapMaxLength(x_length, y_length, z_length);
        //Setting time intervals and elapsing times for the points objects.
        x_length /= 2;
        y_length /= 2;
        z_length /= 2;
        //stopperCoin = new Stopper();
        stopperFrog = new Stopper();
        stopperFrog->start(0);
        stopperMouse = new Stopper();
        stopperMouse->start(0);
        stopperCoin = new Stopper();
        stopperCoin->start(0);
        stopperSpecialBezier = new Stopper();
        stopperSpecialBezier->start(sec_between_specialbezier);
        stopperBezier = new Stopper();
        stopperBezier->start(0);

        for (int i = 0; i < 5; i++) {
            ModelsFactory *factory = ModelsFactory::getInstance();
            std::shared_ptr<Model> frog = factory->CreateModel(GREEN_MATERIAL, FROG,
                                                               std::string(EATING_OBJECT) + std::string(TIMING) +
                                                               std::string(FROG_NAME));
            frog->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[FROG],
                                 ModelsFactory::getInstance()->trees[FROG]);
            frog->Rotate(-M_PI / 2.0f, Movable::Axis::X);
            frog->Rotate(M_PI, Movable::Axis::Z);
            frogs_not_in_use.push_back(frog);
            basicScene->GetRoot()->AddChild(frog);
            frog->isHidden = true;


            std::shared_ptr<Model> mouse = factory->CreateModel(GREY_MATERIAL, MOUSE,
                                                                std::string(EATING_OBJECT) + std::string(TIMING) +
                                                                std::string(MOUSE_NAME));
            mouse->Rotate(-M_PI / 2.0f, Movable::Axis::X);
            mouse->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[MOUSE],
                                  ModelsFactory::getInstance()->trees[MOUSE]);
            basicScene->GetRoot()->AddChild(mouse);
            mouses_not_in_use.push_back(mouse);
            mouse->isHidden = true;



            std::shared_ptr<Model> coin = factory->CreateModel(GOLD_MATERIAL, COIN,
                                                               std::string(EATING_OBJECT) + std::string(TIMING) +
                                                               std::string(COIN_NAME));
            coin->material->program->name = "gold";
            coin->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[COIN],
                                 ModelsFactory::getInstance()->trees[COIN]);
            coin->Rotate(-M_PI / 2.0f, Movable::Axis::X);
            coins_not_in_use.push_back(coin);
            basicScene->GetRoot()->AddChild(coin);
            coin->isHidden = true;



            std::shared_ptr<Model> frog2 =  factory->CreateModel(GREEN_MATERIAL, FROG,
                                                                 std::string(EATING_OBJECT) + std::string(TIMING) +
                                                                 std::string(FROG_NAME));
            frog2->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[FROG],
                                 ModelsFactory::getInstance()->trees[FROG]);
            frog2->Rotate(-M_PI / 2.0f, Movable::Axis::X);
            frog2->Rotate(M_PI/2.0f, Movable::Axis::Z);
            bezier_not_in_use.push_back(frog2);
            basicScene->GetRoot()->AddChild(frog2);
            frog2->Scale(0.5);
            frog2->isHidden = true;
            frog2->bezier = nullptr;


            std::shared_ptr<Model> mouse2 = factory->CreateModel(GREY_MATERIAL, MOUSE,
                                                                std::string(EATING_OBJECT) + std::string(TIMING) +
                                                                std::string(MOUSE_NAME));
            mouse2->Rotate(-M_PI / 2.0f, Movable::Axis::X);
            mouse2->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[MOUSE],
                                  ModelsFactory::getInstance()->trees[MOUSE]);
            basicScene->GetRoot()->AddChild(mouse2);
            bezier_not_in_use.push_back(mouse2);
            mouse2->isHidden = true;
            mouse2->bezier = nullptr;
        }
        CreateLevel1(models, coords);
        basicScene->init_flags[1] = true;
        basicScene->done++;
        //don't need to init it anymore
        is_visitor_inited = true;
//        program = std::make_shared<Program>("../tutorial/shaders/phongShader");
        //material1
//        material = std::make_shared<Material>("material", program); // empty material
//        material->AddTexture(0, "textures/box0.bmp", 2);
        //initiating the system length for the objects positions.

        //generating the Bezier objects.

        //init the first level.


    } else {
        if (basicScene->animate) {

            if(!(stopperSpecialBezier->is_countdown_running())){

                std::shared_ptr<Model> sphere = createSphere();
                Eigen::Vector3f endPoint ,point2, point3, point4;

                //generate random from snake head
                Eigen::Vector3f max_to_gen, min_to_gen;
                get_map_max_min(max_to_gen, min_to_gen);
                endPoint = Calculates::getInstance()->generateRandomPoint(max_to_gen, min_to_gen, basicScene->snake->get_snake_head(), 10,10);
                point2 = Calculates::getInstance()->generateRandomPoint(max_to_gen, min_to_gen, endPoint, 5,10);
                point3 = Calculates::getInstance()->generateRandomPoint(max_to_gen, min_to_gen, point2, 5,10);
                point4 = Calculates::getInstance()->generateRandomPoint(max_to_gen, min_to_gen, point3, 5,10);


                Eigen::Matrix <float, 4, 4 > M;
                Eigen::Matrix <float, 4, 3 > curvePoints;
                M << -1, 3, -3, 1,
                        3, -6, 3, 0,
                        -3, 3, 0, 0,
                        1, 0, 0, 0;

                curvePoints << endPoint[0],endPoint[1],endPoint[2],
                        point2[0], point2[1], point2[2],
                        point3[0], point3[1] ,point3[2],
                        point4[0], point4[1], point4[2];
                sphere->MG_Result = M * curvePoints;
                drawTheBeizerCurve(sphere.get());
                sphere->t = 0.0f;
                stopperSpecialBezier->start(sec_between_specialbezier);
            }
        }
    }
    if(basicScene->getStatistics()->levelUp){
        basicScene->animate = false;
        basicScene->start_time= 0.0f;
        loadNextLevel(basicScene->getStatistics()->level+1);
        basicScene->snake->reset_sake();
        basicScene->resetCameras();
        basicScene->getStatistics()->levelUp = false;
    }
    else if(basicScene->getStatistics()->restart){
        basicScene->animate = false;
        loadNextLevel(1);
        basicScene->snake->reset_sake();
        basicScene->resetCameras();
        basicScene->getStatistics()->restart = false;
        int size = basicScene->getData()->back_to_main.size();
        if( size ==0) {
            basicScene->getStatistics()->menu_flags[MainMenu_OP] = true;
        }else{
            int next = basicScene->getData()->back_to_main.front();
            basicScene->getData()->back_to_main.pop_back();
            basicScene->getStatistics()->menu_flags[next] = true;
        }
          basicScene->start_time = 0.0;
    }
    else if(basicScene->getStatistics()->won){
        basicScene->animate = false;
        basicScene->getStatistics()->won = false;
        basicScene->getStatistics()->menu_flags[WinMenu_OP] = true;
        basicScene->start_time = 0.0;
    }
    Visitor::Run(scene, camera);


}
void ObjectsAnimationVisitor::get_map_max_min(Eigen::Vector3f &max, Eigen::Vector3f &min){
    //generate random from snake head
    Eigen::Vector3f max_to_gen = basicScene->currLevelMap->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff().cast<float>();
    max << max_to_gen[0]*basicScene->currLevelMap->scale_factor[0], max_to_gen[1]*basicScene->currLevelMap->scale_factor[1], max_to_gen[2]*basicScene->currLevelMap->scale_factor[2];
    Eigen::Vector3f min_to_gen = basicScene->currLevelMap->GetMeshList()[0]->data[0].vertices.colwise().minCoeff().cast<float>();
    min << min_to_gen[0]*basicScene->currLevelMap->scale_factor[0], min_to_gen[1]*basicScene->currLevelMap->scale_factor[1], min_to_gen[2]*basicScene->currLevelMap->scale_factor[2];
}

void ObjectsAnimationVisitor::removeFormerlevel(int index){

    for(const shared_ptr<Model>& bz : special_bezier_in_use) {
        if (bz->bezier != nullptr) {
            basicScene->GetRoot()->RemoveChild(bz->bezier);
            Eigen::Vector3f to_scale = {1/bz->scale_factor[0], 1/bz->scale_factor[1], 1/bz->scale_factor[2]};
            bz->Scale(to_scale);
            bz->Translate(-bz->GetPosition());
        }
        bz->isHidden = true;
        special_bezier_not_in_use.push_back(bz);
    }
    for(const shared_ptr<Model>& bz :bezier_in_use) {
        if (bz->bezier != nullptr) {
            basicScene->GetRoot()->RemoveChild(bz->bezier);
            Eigen::Vector3f to_scale = {1/bz->scale_factor[0], 1/bz->scale_factor[1], 1/bz->scale_factor[2]};
            bz->Scale(to_scale);
            bz->Translate(-bz->GetPosition());

        }
        bz->stopper.reset();
        bz->isHidden = true;
        bezier_not_in_use.push_back(bz);
    }

    bezier_in_use.clear();
    clearAllAliveObjects(frogs_in_use,frogs_not_in_use);
    clearAllAliveObjects(mouses_in_use,mouses_not_in_use);
    clearAllAliveObjects(coins_in_use, coins_not_in_use);
    clearAllAliveObjects(bricks_in_use, bricks_not_in_use);

}
void ObjectsAnimationVisitor::clearAllAliveObjects(std::vector<std::shared_ptr<Model>> &objects_in_use, std::vector<std::shared_ptr<Model>> &objects_not_in_use){
    for(const shared_ptr<Model>& model : objects_in_use) {
        objects_not_in_use.push_back(model);
        model->stopper.reset();
        Eigen::Vector3f to_scale = {1/model->scale_factor[0], 1/model->scale_factor[1], 1/model->scale_factor[2]};
        model->Scale(to_scale);
        model->Translate(-model->GetPosition());
        model->stopper.reset();
    }
    objects_in_use.clear();
}

void ObjectsAnimationVisitor::loadNextLevel(int nextLevel){
    switch (nextLevel) {
        case 1:
            removeFormerlevel(3);
            if(!basicScene->level2->isHidden){
                basicScene->level2->isHidden = true;
            }
            if(!basicScene->level3->isHidden){
                basicScene->level3->isHidden = true;
            }
            CreateLevel1(models, coords);
            break;
        case 2:
            removeFormerlevel(1);
            if(!basicScene->level1->isHidden){
                basicScene->level1->isHidden = true;
            }
            if(!basicScene->level3->isHidden){
                basicScene->level3->isHidden = true;
            }
            CreateLevel2(models, coords);
            break;
        case 3:
            removeFormerlevel(2);
            if(!basicScene->level1->isHidden){
                basicScene->level1->isHidden = true;
            }
            if(!basicScene->level2->isHidden){
                basicScene->level2->isHidden = true;
            }
            CreateLevel3(models, coords);
            break;
        default:
            removeFormerlevel(3);
            if(!basicScene->level2->isHidden){
                basicScene->level2->isHidden = true;
            }
            if(!basicScene->level3->isHidden){
                basicScene->level3->isHidden = true;
            }
            CreateLevel1(models, coords);
            break;
    }
}

void ObjectsAnimationVisitor::Visit(Model *model) {
    if(!model->isHidden && basicScene->animate) {

    }
}
std::vector<std::shared_ptr<Model>>  ObjectsAnimationVisitor::checkForRemove(const std::vector<std::shared_ptr<Model>>& objects_in_use) {
    std::vector<std::shared_ptr<Model>> objects_to_remove;
    for (const std::shared_ptr<Model> &model: objects_in_use) {
        if (!model->stopper.is_countdown_running()) {
            objects_to_remove.push_back(model);
        }
    }
    return objects_to_remove;
}
void ObjectsAnimationVisitor::Visit(Scene *scene) {

    std::vector<std::shared_ptr<Model>> to_remove;
    for(const std::shared_ptr<Model>& model: bezier_in_use) {
        if (model->stopper.is_countdown_running()) {
            if (model->t <= 1 && !model->moveBackwards) {
                moveAccordingToBeizerCurve(model.get());
                model->t += 0.04 * model->bezier_speed;
            } else {
                if (!model->moveBackwards)
                    model->moveBackwards = true;
                if (model->moveBackwards) {
                    moveAccordingToBeizerCurve(model.get());
                    model->t -= 0.04 * model->bezier_speed;
                    if (model->t <= 0)
                        model->moveBackwards = false;
                }
            }
        } else{
            to_remove.push_back(model);
        }
    }
    for(const std::shared_ptr<Model>& model: to_remove){

        basicScene->GetRoot()->RemoveChild(model->bezier);
        removeBezier(model);
    }

    if(basicScene->animate) {
        std::vector<std::shared_ptr<Model>> frogs_to_remove = checkForRemove(frogs_in_use);
        for(const std::shared_ptr<Model>& frog: frogs_to_remove){
            removeFrog(frog);
        }
        std::vector<std::shared_ptr<Model>> mousses_to_remove = checkForRemove(mouses_in_use);
        for(const std::shared_ptr<Model>& mouse: mousses_to_remove){
            removeMouse(mouse);
        }
        std::vector<std::shared_ptr<Model>> coins_to_remove;
        for (const std::shared_ptr<Model> &coin: coins_in_use) {
            if (!coin->stopper.is_countdown_running()) {
                coins_to_remove.push_back(coin);
            }else{
               // coin->Rotate(0.3f, Movable::Axis::Z);
            }
        }
        for(const std::shared_ptr<Model>& coin: coins_to_remove){
            removeCoin(coin);
        }
        if(!(stopperBezier->is_countdown_running())){
            std::shared_ptr<Model> frog = createFrogBezier();
            stopperBezier->start(sec_bezier);
            frog->stopper.start(len_bezier);
        }
        if (!(stopperFrog->is_countdown_running())) {
            std::shared_ptr<Model> frog = createFrog();
            Eigen::Vector3f pos = get_point(frogPoints);
            frog->Translate(pos);
            frog->stopper.start(len1);
            stopperFrog->start(sec1);
        }
        if (!(stopperMouse->is_countdown_running())) {
            std::shared_ptr<Model> mouse = createMouse();
            Eigen::Vector3f pos = get_point(mousePoints);
            mouse->Translate(pos);
            mouse->stopper.start(len2);
            stopperMouse->start(sec2);
        }
        if (!(stopperCoin->is_countdown_running())) {
            std::shared_ptr<Model> coin = createCoin();
            Eigen::Vector3f pos = get_point(coinPoints);
            coin->Translate(pos);
            coin->stopper.start(len3);
            stopperCoin->start(sec3);
        }
    }
    for(const std::shared_ptr<Model>& model: special_bezier_in_use) {
        float t = model->t + 0.04 * model->bezier_speed;
        //keep going
        if(t <= 1){
            moveAccordingToBeizerCurve(model.get());
            model->t += 0.04 * model->bezier_speed;
        }
            //generate new bezier from the end point and draw curve
        else{
            Eigen::RowVector4f Ti;
            Ti[0] = powf(1, 3);
            Ti[1] = powf(1, 2);
            Ti[2] = 1;
            Ti[3] = 1;
            Eigen::Vector3f endPoint ,point2, point3, point4;
            endPoint = (Ti * model->MG_Result);

            Eigen::Vector3f max_to_gen,  min_to_gen;
            get_map_max_min(max_to_gen, min_to_gen);
            point2 = Calculates::getInstance()->generateRandomPoint(max_to_gen, min_to_gen, endPoint, 5,10);
            point3 = Calculates::getInstance()->generateRandomPoint(max_to_gen, min_to_gen, point2, 5,10);
            point4 = Calculates::getInstance()->generateRandomPoint(max_to_gen, min_to_gen, point3, 5,10);


            Eigen::Matrix <float, 4, 4 > M;
            Eigen::Matrix <float, 4, 3 > curvePoints;
            M << -1, 3, -3, 1,
                    3, -6, 3, 0,
                    -3, 3, 0, 0,
                    1, 0, 0, 0;

            curvePoints << endPoint[0],endPoint[1],endPoint[2],
                    point2[0], point2[1], point2[2],
                    point3[0], point3[1] ,point3[2],
                    point4[0], point4[1], point4[2];
            model->MG_Result = M * curvePoints;
            basicScene->GetRoot()->RemoveChild(model->bezier);
            drawTheBeizerCurve(model.get());
            model->t = 0.0f;
        }
    }
}
void ObjectsAnimationVisitor::setModelBezier(Eigen::Vector3f vectors,Model *model){
    Calculates::getInstance()->generateRandomBeizierCurve(std::move(vectors),model->MG_Result);
    drawTheBeizerCurve(model);
}

float ObjectsAnimationVisitor::generate_random_number(float min, float max) {
    return min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
}

void ObjectsAnimationVisitor::moveAccordingToBeizerCurve(Model *model) {
    Eigen::RowVector4f Ti;
    Ti[0] = powf(model->t, 3);
    Ti[1] = powf(model->t, 2);
    Ti[2] = model->t;
    Ti[3] = 1;

    Eigen::Vector3f currentPosition;
    currentPosition = (Ti * model->MG_Result);

    Eigen::Vector3f oldPositionOfObject = model->GetPosition();
    model->Translate(currentPosition - oldPositionOfObject);
}

void ObjectsAnimationVisitor::drawTheBeizerCurve(Model *model) {
    int number_of_points_in_bezier = 100;
    std::vector<double> line_space = Calculates::getInstance()->linspace(0,1,number_of_points_in_bezier);
    Eigen::Vector3d drawingColor = Eigen::RowVector3d(0, 0, 2);
    Eigen::MatrixXf vertices(number_of_points_in_bezier,3);
    for (int i = 0; i < number_of_points_in_bezier; ++i) {
        double ti = line_space[i];
        Eigen::RowVector4f Ti;
        Ti[0] = powf(ti, 3);
        Ti[1] = powf(ti, 2);
        Ti[2] = ti;
        Ti[3] = 1;
        vertices.row(i) = Ti * model->MG_Result;
    }

    Eigen::MatrixXi faces(number_of_points_in_bezier-1,2);
    for (int i = 0; i < number_of_points_in_bezier-1; ++i) {

        faces.row(i) = Eigen::Vector2i(i,i+1);
    }

    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(number_of_points_in_bezier,3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(number_of_points_in_bezier,2);// empty material

    std::shared_ptr<Mesh> coordsys = std::make_shared<Mesh>("coordsys",vertices.cast<double>(),faces,vertexNormals,textureCoords);
    std::shared_ptr<Model> bezier = Model::Create("bez",coordsys,ModelsFactory::getInstance()->materials[BASIC_MATERIAL]);
    bezier->mode = 1;
    model->bezier = bezier;
    basicScene->GetRoot()->AddChild(bezier);

}

shared_ptr<Model> ObjectsAnimationVisitor::createFrog(){
    shared_ptr<Model> frog = addModelToGame(frogs_in_use,frogs_not_in_use);
    if(frog == nullptr) {
        ModelsFactory *factory = ModelsFactory::getInstance();
        frog = factory->CreateModel(GREEN_MATERIAL, FROG,
                                                      std::string(EATING_OBJECT) + std::string(TIMING) +
                                                      std::string(FROG_NAME));
        frog->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[FROG],
                             ModelsFactory::getInstance()->trees[FROG]);
        frog->Rotate(-M_PI / 2.0f, Movable::Axis::X);
        frog->Rotate(M_PI, Movable::Axis::Z);
        frogs_in_use.push_back(frog);
        basicScene->GetRoot()->AddChild(frog);
        frog->Scale(frog_scale);
    }
    return frog;
}
shared_ptr<Model> ObjectsAnimationVisitor::createBrick(){
    shared_ptr<Model> cube = addModelToGame(bricks_in_use, bricks_not_in_use);
    if(cube == nullptr){
        cube = ModelsFactory::getInstance()->CreateModel(BRICKS_MATERIAL,CUBE,std::string(COLLISION_OBJECT) + " bricks");
        cube->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[CUBE],ModelsFactory::getInstance()->trees[CUBE]);
        basicScene->GetRoot()->AddChild(cube);
        bricks_in_use.push_back(cube);
        cube->Scale(brick_scale);
    }
    return cube;

}

shared_ptr<Model> ObjectsAnimationVisitor::createSphere(){
    std::shared_ptr<Model> sphere = addModelToGame(special_bezier_in_use, special_bezier_not_in_use);
    if(sphere == nullptr) {
        sphere = ModelsFactory::getInstance()->CreateModel(GREEN_MATERIAL, SPHERE, std::string(
                SPECIAL_BEZIER_OBJECT_NAME) + " " + "SPHERE");
        sphere->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[SPHERE],
                               ModelsFactory::getInstance()->trees[SPHERE]);
        basicScene->GetRoot()->AddChild(sphere);
        special_bezier_in_use.push_back(sphere);
        sphere->Scale(sphere_scale);
    }
    return sphere;
}



shared_ptr<Model> ObjectsAnimationVisitor::createMouse(){
    std::shared_ptr<Model> mouse = addModelToGame(mouses_in_use, mouses_not_in_use);
    if(mouse == nullptr){
        ModelsFactory* factory = ModelsFactory::getInstance();
        mouse = factory->CreateModel(GREY_MATERIAL , MOUSE ,std::string(EATING_OBJECT)+ std::string(TIMING)+ std::string(MOUSE_NAME));
        mouse->material->program->name = "grey";
        mouse->Rotate(-M_PI/2.0f,Movable::Axis::X);
        mouse->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[MOUSE],ModelsFactory::getInstance()->trees[MOUSE]);
        mouses_in_use.push_back(mouse);
        basicScene->GetRoot()->AddChild(mouse);
        mouse->Scale(mouse_scale);
    }
    return mouse;
}

shared_ptr<Model> ObjectsAnimationVisitor::createCoin(){
    shared_ptr<Model> coin = addModelToGame(coins_in_use, coins_not_in_use);
    if(coin == nullptr) {
        ModelsFactory *factory = ModelsFactory::getInstance();
        coin = factory->CreateModel(GOLD_MATERIAL, COIN,
                                                      std::string(EATING_OBJECT) + std::string(TIMING) +
                                                      std::string(COIN_NAME));
        coin->material->program->name = "gold";
        coin->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[COIN],
                             ModelsFactory::getInstance()->trees[COIN]);
        coin->Rotate(-M_PI / 2.0f, Movable::Axis::X);
        coins_in_use.push_back(coin);
        basicScene->GetRoot()->AddChild(coin);
        coin->Scale(coin_scale);
    }
    return coin;
}
shared_ptr<Model> ObjectsAnimationVisitor::createFrogBezier(){
    shared_ptr<Model> frog = addModelToGame(bezier_in_use, bezier_not_in_use);
    if(frog == nullptr) {
        ModelsFactory *factory = ModelsFactory::getInstance();
        frog =  factory->CreateModel(GREEN_MATERIAL, FROG,
                                                                    std::string(EATING_OBJECT) + std::string(TIMING) +
                                                                    std::string(FROG_NAME));
        frog->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[FROG],
                             ModelsFactory::getInstance()->trees[FROG]);
        frog->Rotate(-M_PI / 2.0f, Movable::Axis::X);
        frog->Rotate(M_PI/2.0f, Movable::Axis::Z);
        frog->Scale(0.5);
        bezier_in_use.push_back(frog);
        basicScene->GetRoot()->AddChild(frog);
        frog->bezier = nullptr;

        frog->Scale(frog_scale);
    }
    Eigen::Vector3f pos = get_point(bezierPoints);
    setModelBezier(pos, frog.get());
    frog->Translate(pos);
    return frog;
}
std::shared_ptr<Model> ObjectsAnimationVisitor::addModelToGame(std::vector<std::shared_ptr<Model>> &objects_in_use, std::vector<std::shared_ptr<Model>> &objects_not_in_use){
    std::shared_ptr<Model> model = nullptr;
    if(!objects_not_in_use.empty()){
        model = objects_not_in_use.front();
        objects_not_in_use.erase(objects_not_in_use.begin());
        model->isHidden = false;
        objects_in_use.push_back(model);
    }
    return model;
}



void ObjectsAnimationVisitor::removeFrog(const shared_ptr<Model>& frog){
    removeModelFromGame(frogs_in_use, frogs_not_in_use,frog);
}
void ObjectsAnimationVisitor::removeMouse(const shared_ptr<Model>& mouse){
    removeModelFromGame(mouses_in_use, mouses_not_in_use,mouse);
}
void ObjectsAnimationVisitor::removeCoin(const shared_ptr<Model>& coin){
    removeModelFromGame(coins_in_use, coins_not_in_use,coin);
}
void ObjectsAnimationVisitor::removeSphere(const shared_ptr<Model>& sphere){
    removeModelFromGame(special_bezier_in_use, special_bezier_not_in_use, sphere);
}
void ObjectsAnimationVisitor::removeBricks(const shared_ptr<Model>& cube){
    removeModelFromGame(bricks_in_use, bricks_not_in_use, cube);
}
void ObjectsAnimationVisitor::removeBezier(const shared_ptr<Model>& bezier){
    removeModelFromGame(bezier_in_use, bezier_not_in_use, bezier);
}


void ObjectsAnimationVisitor::removeModelFromGame(std::vector<std::shared_ptr<Model>> &objects_in_use, std::vector<std::shared_ptr<Model>> &objects_not_in_use, const std::shared_ptr<Model>& model){
    model->isHidden = true;
    model->Translate(Eigen::Vector3f(0.0f, 0.0f, 0.0f) - model->GetPosition());
    objects_not_in_use.push_back(model);
    auto it = std::find(objects_in_use.begin(), objects_in_use.begin(), model);
    if (it != objects_in_use.end()) {
        objects_in_use.erase(it);
    } else{
        std::cout<<"check it please.." << std::endl;
    }
}

void ObjectsAnimationVisitor::generatePointsInMapLevel(std::vector<Eigen::Vector3f> &coords, int n){
    coords.clear();
    coords.resize(0);
    Eigen::Vector3d max = basicScene->currLevelMap->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff();
    Eigen::Vector3d min = basicScene->currLevelMap->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
    double z_length = max[2]*basicScene->currLevelMap->scale_factor[2] - min[2]*basicScene->currLevelMap->scale_factor[2];
    double y_length = max[1]*basicScene->currLevelMap->scale_factor[1] - min[1]*basicScene->currLevelMap->scale_factor[1];
    double x_length = max[0]*basicScene->currLevelMap->scale_factor[0] - min[0]*basicScene->currLevelMap->scale_factor[0];
    std::vector<double> objects_in_space_z = Calculates::getInstance()->linspace(5,z_length/2,n);
    std::vector<double> objects_in_space_y = Calculates::getInstance()->linspace(5,y_length/2,n);
    std::vector<double> objects_in_space_x = Calculates::getInstance()->linspace(5,x_length/2,n);
    int scale = 3;
    Calculates::getInstance()->setRandomCubeLocations(x_length, y_length, z_length, n,  scale, coords);
}

void ObjectsAnimationVisitor::CreateLevel1(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords) {
    basicScene->currLevelMap = basicScene->level1;
    basicScene->level1->isHidden = false;
    frog_scale = 0.5f;
    mouse_scale = 0.5f;
    coin_scale = 1.0f;
    brick_scale = 2.0f;
    sphere_scale = 1.0f;
    int n = 50;
    init_point_givers(2);
    rescaleObjects(1);
    generatePointsInMapLevel(coords, n);
    for (int i = 0; i < n; i++) {
        Eigen::Vector3f position = coords[i];
        coords.push_back(position);
        shared_ptr<Model> cube = createBrick();
        cube->Translate(position - cube->GetPosition());
    }
    shared_ptr<Model> tree = ModelsFactory::getInstance()->CreateModel2(TREE_MATERIAL, TREE_PATH,
                                                                        std::string(COLLISION_OBJECT) + " tree");
    basicScene->GetRoot()->AddChild(tree);
    tree->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[TREE], ModelsFactory::getInstance()->trees[TREE]);
    tree->Translate({10.0, 0, 10.0});

basicScene->getStatistics()->speed=1;
basicScene->snake->speed=1;

}
void ObjectsAnimationVisitor::CreateLevel2(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords) {
    basicScene->currLevelMap = basicScene->level2;
    basicScene->level2->isHidden =false;
    frog_scale = 0.6f;
    mouse_scale = 0.7f;
    coin_scale = 0.8f;
    brick_scale = 3.0f;
    sphere_scale = 1.5f;
    init_point_givers(4);
    int n = 75;
    rescaleObjects(2);
    generatePointsInMapLevel(coords, n);
    for (int i = 0; i < n; i++) {
        Eigen::Vector3f position = coords[i];
        coords.push_back(position);
        std::shared_ptr<Model> cube = createBrick();
        cube->Translate(position - cube->GetPosition());
    }
    basicScene->getStatistics()->speed=1.5;
    basicScene->snake->speed=1.5;
}



void ObjectsAnimationVisitor::CreateLevel3(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords) {
    basicScene->currLevelMap = basicScene->level3;
    basicScene->level3->isHidden =false;
    frog_scale = 0.5f;
    mouse_scale = 0.5f;
    coin_scale = 0.4f;
    brick_scale = 4.0f;
    sphere_scale = 2.0f;
    init_point_givers(4);
    int n = 100;
    rescaleObjects(3);
    generatePointsInMapLevel(coords, n);
    for (int i = 0; i < n; i++) {
        Eigen::Vector3f position = coords[i];
        coords.push_back(position);
        std::shared_ptr<Model> cube = createBrick();
        cube->Translate(position - cube->GetPosition());
    }
    basicScene->getStatistics()->speed=2;
    basicScene->snake->speed=2;
}

void ObjectsAnimationVisitor::rescaleObjects(float bez_speed){
    for (const std::shared_ptr<Model> &frog: frogs_not_in_use){
        frog->Scale(frog_scale);
    }
    for (const std::shared_ptr<Model> &mouse: mouses_not_in_use){
        mouse->Scale(mouse_scale);
    }
    for (const std::shared_ptr<Model> &coin: coins_not_in_use){
        coin->Scale(coin_scale);
    }
    for (const std::shared_ptr<Model> &sphere: special_bezier_in_use){
        sphere->Scale(sphere_scale);
        sphere->bezier_speed = bez_speed;
    }
    for (const std::shared_ptr<Model> &brick: bricks_not_in_use){
        brick->Scale(brick_scale);
    }
    for (const std::shared_ptr<Model> &bez: bezier_not_in_use){
        bez->Scale(frog_scale);
        bez->bezier_speed = bez_speed;
    }
}

void ObjectsAnimationVisitor::GetCurrMapMaxLength(float &length_x, float &length_y, float &length_z){
    Eigen::Vector3d max = basicScene->currLevelMap->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff();
    Eigen::Vector3d min = basicScene->currLevelMap->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
    length_z = max[2]*basicScene->currLevelMap->scale_factor[2] - min[2]*basicScene->currLevelMap->scale_factor[2];
    length_y = max[1]*basicScene->currLevelMap->scale_factor[1] - min[1]*basicScene->currLevelMap->scale_factor[1];
    length_x = max[0]*basicScene->currLevelMap->scale_factor[0] - min[0]*basicScene->currLevelMap->scale_factor[0];
}


void ObjectsAnimationVisitor::init_point_givers(float div_map) {
    std::queue<Eigen::Vector3f> empty_junk1, empty_junk2, empty_junk3;
    std::swap( frogPoints, empty_junk1);
    std::swap( mousePoints, empty_junk2);
    std::swap( coinPoints, empty_junk3);
    float dist = basicScene->currLevelMap->scale_factor[0]/div_map;
    int half = dist/2;
    Eigen::Vector3f min_to_gen, max_to_gen;
    get_map_max_min(max_to_gen, min_to_gen);
    for (int i = 0; i < number_of_points; i++) {
        Eigen::Vector3f frog_point, mouse_point, coin_point, bezier_point;
        if (i % 2 == 0){
            frog_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},5 ,half);
            mouse_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},5 ,half);
            coin_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},5 ,half);
            bezier_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},5 ,half);

        }else{
            frog_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},half ,dist);
            mouse_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},half ,dist);
            coin_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},half ,dist);
            bezier_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},half ,dist);

        }
        frogPoints.push(frog_point);
        mousePoints.push(mouse_point);
        coinPoints.push(coin_point);
        bezierPoints.push(bezier_point);
    }

}
Eigen::Vector3f ObjectsAnimationVisitor::get_point(std::queue<Eigen::Vector3f> &coords){
    //create new point.
    if(coords.size()<=0){
        int dist = basicScene->currLevelMap->scale_factor[0];
        int half = dist/2;
        Eigen::Vector3f min_to_gen, max_to_gen;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution p(0,1);
        int max, min;
        int i = p(gen);
        if(i == 0){
            max = half;
            min = 0;
        } else{
            max = dist;
            min = half;
        }
        get_map_max_min(max_to_gen, min_to_gen);
        Eigen::Vector3f ans = Calculates::getInstance()->generateRandomPoint(max_to_gen, min_to_gen, Eigen::Vector3f::Zero(), min + 5,max);
        coords.push(ans);
        return ans;
    }else{
        Eigen::Vector3f coord = coords.front().cast<float>();
        coords.pop();
        coords.push(coord);
        return coord;
    }
}