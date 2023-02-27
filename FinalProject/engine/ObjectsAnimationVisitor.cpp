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
    if (!is_visitor_inited && basicScene->init_flags[0]) {
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
        stopperFrog->start(sec1);
        stopperMouse = new Stopper();
        stopperMouse->start(sec2);
        stopperCoin = new Stopper();
        stopperCoin->start(sec3);
        stopperSpecialBezier = new Stopper();
        stopperSpecialBezier->start(sec_between_specialbezier);

        //don't need to init it anymore
        is_visitor_inited = true;
        program = std::make_shared<Program>("../tutorial/shaders/phongShader");
        //material1
        material = std::make_shared<Material>("material", program); // empty material
        material->AddTexture(0, "textures/box0.bmp", 2);
        //initiating the system length for the objects positions.

        //generating the Bezier objects.

        //init the first level.
        CreateLevel1(models, coords);
        basicScene->init_flags[1] = true;
        basicScene->done++;

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
                basicScene->GetRoot()->AddChild(sphere);
                stopperSpecialBezier->start(sec_between_specialbezier);
            }
            if (!(stopperFrog->is_countdown_running())) {
                if(!frogs_available.empty()){
                    Model *frog = frogs_available.front();
                    frogs_available.pop_back();
                    frog->isHidden = false;
                    Eigen::Vector3f pos = get_point(frogPoints);
                    frog->Translate(pos);
                    frog->stopper.start(len1);
                } else{
                    std::shared_ptr<Model> frog = createFrog();
                    frog->isHidden = false;
                    basicScene->GetRoot()->AddChild(frog);
                    Eigen::Vector3f pos = get_point(frogPoints);
                    frog->Translate(pos);
                    frog->stopper.start(len1);
                }
                stopperFrog->start(sec1);
            }
            if (!(stopperMouse->is_countdown_running())) {

                if(mouses_available.size()>0){
                    Model *mouse = mouses_available.front();
                    mouses_available.pop_back();
                    mouse->isHidden = false;
                    Eigen::Vector3f pos = get_point(mousePoints);
                    mouse->Translate(pos);
                    mouse->stopper.start(len2);
                } else{
                    std::shared_ptr<Model> mouse = createMouse();
                    mouse->isHidden = false;
                    basicScene->GetRoot()->AddChild(mouse);
                    Eigen::Vector3f pos = get_point(mousePoints);
                    mouse->Translate(pos);
                    mouse->stopper.start(len2);
                }
                stopperMouse->start(sec2);
            }
            if (!(stopperCoin->is_countdown_running())) {
                Model *coin;
                if(!coins_available.empty()){
                    coin = coins_available.front();
                    coins_available.pop_back();
                    coin->isHidden = false;
                    Eigen::Vector3f pos = get_point(coinPoints);
                    coin->Translate(pos);
                    coin->stopper.start(len1);
                } else{
                    std::shared_ptr<Model> coin = createCoin();
                    coin->isHidden = false;
                    basicScene->GetRoot()->AddChild(coin);
                    Eigen::Vector3f pos = get_point(coinPoints);
                    coin->Translate(pos);
                    coin->stopper.start(len1);
                }
                stopperCoin->start(sec1);
            }
        }
    }
    if(basicScene->getStatistics()->levelUp){
        basicScene->animate = false;
        basicScene->start_time= 0.0f;
        //removeFormerlevel();
        loadNextLevel(basicScene->getStatistics()->level+1);
        basicScene->snake->reset_sake();
        basicScene->resetCameras();
        basicScene->getStatistics()->levelUp = false;
    }
    else if(basicScene->getStatistics()->restart){
        basicScene->animate = false;
        //removeFormerlevel();
        loadNextLevel(1);
        basicScene->snake->reset_sake();
        basicScene->resetCameras();
        basicScene->getStatistics()->restart = false;
        int size = basicScene->getData()->back_to_main.size();
        if( size ==0) {
//            basicScene->getStatistics()->menu_flags[MainMenu_OP] = true;
        }else{
//            int next = basicScene->getData()->back_to_main.front();
//            basicScene->getData()->back_to_main.pop_back();
//            basicScene->getStatistics()->menu_flags[next] = true;
        }
//        basicScene->start_time = 0.0;
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
    Eigen::Vector3f max_to_gen = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff().cast<float>();
    max << max_to_gen[0]*basicScene->level1->scale_factor[0], max_to_gen[1]*basicScene->level1->scale_factor[1], max_to_gen[2]*basicScene->level1->scale_factor[2];
    Eigen::Vector3f min_to_gen = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().minCoeff().cast<float>();
    min << min_to_gen[0]*basicScene->level1->scale_factor[0], min_to_gen[1]*basicScene->level1->scale_factor[1], min_to_gen[2]*basicScene->level1->scale_factor[2];
}


void ObjectsAnimationVisitor::removeFormerlevel(){

    for(const shared_ptr<Model>& model : models) {
        if (model != nullptr) {
            basicScene->GetRoot()->RemoveChild(model);
        }
    }
    models.clear();
    coords.clear();
}

void ObjectsAnimationVisitor::loadNextLevel(int nextLevel){
    for (auto & sphere : spheres_in_use) {
        if(sphere->bezier != nullptr){
            basicScene->GetRoot()->RemoveChild(sphere->bezier);
            sphere->bezier = nullptr;
        }
        sphere->isHidden = true;
        spheres_not_in_use.push_back(sphere);
    }
    switch (nextLevel) {
        case 1:
            CreateLevel1(models, coords);
            break;
        case 2:
            CreateLevel2(models, coords);
            break;
        case 3:
            CreateLevel3(models, coords);
            break;
        default:
            //level 1;
            CreateLevel1(models, coords);
            break;
    }
}

void ObjectsAnimationVisitor::Visit(Model *model) {
    if(!model->isHidden && basicScene->animate) {
        if(model->name.find(TIMING) != std::string::npos){
            if(!model->stopper.is_countdown_running()){
                //change the position.
                model->Translate(Eigen::Vector3f(0.0f,0.0f,0.0f) - model->GetPosition());
                //hide him
                model->isHidden =true;
                if(model->name.find(FROG_NAME) != std::string::npos){
                    //return to list
                    frogs_available.push_back(model);
                }
                if(model->name.find(MOUSE_NAME) != std::string::npos){
                    //return to list
                    mouses_available.push_back(model);
                }
                if(model->name.find(COIN_NAME) != std::string::npos){
                    //return to list
                    coins_available.push_back(model);
                }
                model->stopper.reset();
            }
        }else if (model->name.substr(0, strlen(SPECIAL_BEZIER_OBJECT_NAME)) == std::string(SPECIAL_BEZIER_OBJECT_NAME)) {
            int i = 0;
            float t = model->t + 0.04 * model->bezier_speed;
            //keep going
            if(t <= 1){
                moveAccordingToBeizerCurve(model);
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
                drawTheBeizerCurve(model);
                model->t = 0.0f;
            }
        }
        else if (model->name.substr(0, strlen(BEZIER_OBJECT_NAME)) == std::string(BEZIER_OBJECT_NAME)) {
            if (model->t <= 1 && !model->moveBackwards) {
                moveAccordingToBeizerCurve(model);
                model->t += 0.04 * model->bezier_speed;
            } else {
                if (!model->moveBackwards)
                    model->moveBackwards = true;
                if (model->moveBackwards) {
                    moveAccordingToBeizerCurve(model);
                    model->t -= 0.04 * model->bezier_speed;
                    if (model->t <= 0)
                        model->moveBackwards = false;
                }
            }
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
    ModelsFactory* factory = ModelsFactory::getInstance();
    shared_ptr<Model> frog =  factory->CreateModel(GREEN_MATERIAL , FROG , std::string (EATING_OBJECT) + std::string(TIMING)+ std::string(FROG_NAME));
    frog->material->program->name = "green";
    frog->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[FROG],ModelsFactory::getInstance()->trees[FROG]);
    frog->Rotate(-M_PI/2.0f, Movable::Axis::X);
    frog->Rotate(M_PI, Movable::Axis::Z);
    return frog;
}

shared_ptr<Model> ObjectsAnimationVisitor::createSphere(){
    if(spheres_not_in_use.empty()){
        std::shared_ptr<Model> sphere = ModelsFactory::getInstance()->CreateModel(GREEN_MATERIAL,SPHERE,std::string(SPECIAL_BEZIER_OBJECT_NAME) + "SPHERE");
        sphere->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[SPHERE],ModelsFactory::getInstance()->trees[SPHERE]);
        spheres_in_use.push_back(sphere);
        return sphere;
    }else{
        std::shared_ptr<Model> sphere = spheres_not_in_use.front();
        spheres_not_in_use.pop_back();
        sphere->isHidden = false;
        spheres_in_use.push_back(sphere);
        return sphere;
    }
}


shared_ptr<Model> ObjectsAnimationVisitor::createMouse(){
    ModelsFactory* factory = ModelsFactory::getInstance();
    shared_ptr<Model> mouse = factory->CreateModel(GREY_MATERIAL , MOUSE ,std::string(EATING_OBJECT)+ std::string(TIMING)+ std::string(MOUSE_NAME));
    mouse->material->program->name = "grey";
    mouse->Rotate(-M_PI/2.0f,Movable::Axis::X);
    mouse->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[MOUSE],ModelsFactory::getInstance()->trees[MOUSE]);
    return mouse;
}

shared_ptr<Model> ObjectsAnimationVisitor::createCoin(){
    ModelsFactory* factory = ModelsFactory::getInstance();
    shared_ptr<Model> coin =  factory->CreateModel(GOLD_MATERIAL , COIN ,std::string(EATING_OBJECT)+  std::string(TIMING)+ std::string(COIN_NAME));
    coin->material->program->name = "gold";
    coin->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[COIN],ModelsFactory::getInstance()->trees[COIN]);
    coin->Rotate(-M_PI/2.0f, Movable::Axis::X);
    return coin;
}

void ObjectsAnimationVisitor::CreateLevel1(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords) {
    init_point_givers();
    int n = 20;
    models.resize(0);
    coords.resize(0);
    Eigen::Vector3d max = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff();
    Eigen::Vector3d min = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
    double z_length = max[2]*basicScene->level1->scale_factor[2] - min[2]*basicScene->level1->scale_factor[2];
    double y_length = max[1]*basicScene->level1->scale_factor[1] - min[1]*basicScene->level1->scale_factor[1];
    double x_length = max[0]*basicScene->level1->scale_factor[0] - min[0]*basicScene->level1->scale_factor[0];
    std::vector<double> objects_in_space_z = Calculates::getInstance()->linspace(5,z_length/2,n);
    std::vector<double> objects_in_space_y = Calculates::getInstance()->linspace(5,y_length/2,n);
    std::vector<double> objects_in_space_x = Calculates::getInstance()->linspace(5,x_length/2,n);
    int scale = 3;
    std::vector<Calculates::ObjectInfo> cubes;
    Calculates::getInstance()->setRandomCubeLocations(x_length, y_length, z_length, n,  scale, cubes);

    for (int i = 0; i < n; i++) {
        //Eigen::Vector3f position = {objects_in_space_x[i],objects_in_space_y[i],objects_in_space_z[i]};
        Eigen::Vector3f position = cubes[i].position.cast<float>();
        coords.push_back(position);
        shared_ptr<Model> cube = ModelsFactory::getInstance()->CreateModel(BRICKS_MATERIAL,CUBE,std::string(COLLISION_OBJECT) + " bricks");
        models.push_back(cube);
        cube->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[CUBE],ModelsFactory::getInstance()->trees[CUBE]);
        basicScene->GetRoot()->AddChild(cube);
        cube->showWireframe = true;
        cube->Translate(position);
        cube->Scale(scale);
    }
    shared_ptr<Model> tree = ModelsFactory::getInstance()->CreateModel2(TREE_MATERIAL,TREE_PATH,std::string(COLLISION_OBJECT) + " tree");
    basicScene->AddChild(tree);
    tree->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[TREE],ModelsFactory::getInstance()->trees[TREE]);
    tree->Translate({10.0,0,10.0});

    for (int i = 0; i < 5; i++) {
        std::shared_ptr<Model> frog = createFrog();
        frog->isHidden = true;
        basicScene->GetRoot()->AddChild(frog);
        frogs_available.push_back(frog.get());
        std::shared_ptr<Model> mouse = createMouse();
        mouse->isHidden = true;
        basicScene->GetRoot()->AddChild(mouse);
        frogs_available.push_back(mouse.get());
        std::shared_ptr<Model> coin = createCoin();
        coin->isHidden = true;
        basicScene->GetRoot()->AddChild(coin);
        frogs_available.push_back(coin.get());
    }
}
void ObjectsAnimationVisitor::CreateLevel2(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords) {
    init_point_givers();
    int n = 30;
    models.resize(0);
    coords.resize(0);
    Eigen::Vector3d max = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff();
    Eigen::Vector3d min = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
    double z_length = max[2]*basicScene->level1->scale_factor[2] - min[2]*basicScene->level1->scale_factor[2];
    double y_length = max[1]*basicScene->level1->scale_factor[1] - min[1]*basicScene->level1->scale_factor[1];
    double x_length = max[0]*basicScene->level1->scale_factor[0] - min[0]*basicScene->level1->scale_factor[0];
    std::vector<double> objects_in_space_z = Calculates::getInstance()->linspace(5,z_length/2,n);
    std::vector<double> objects_in_space_y = Calculates::getInstance()->linspace(5,y_length/2,n);
    std::vector<double> objects_in_space_x = Calculates::getInstance()->linspace(5,x_length/2,n);
    int scale = 4;
    std::vector<Calculates::ObjectInfo> cubes;
    Calculates::getInstance()->setRandomCubeLocations(x_length, y_length, z_length, n,  scale, cubes);

    for (int i = 0; i < n; i++) {
        Eigen::Vector3f position = cubes[i].position.cast<float>();
        coords.push_back(position);
        std::shared_ptr<Model> cube = ModelsFactory::getInstance()->CreateModel(BRICKS_MATERIAL,CUBE,std::string(COLLISION_OBJECT) + "bricks");
        cube->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[CUBE],ModelsFactory::getInstance()->trees[CUBE]);
        basicScene->GetRoot()->AddChild(cube);
        cube->Translate(position);
        cube->Scale(scale);
    }
}



void ObjectsAnimationVisitor::CreateLevel3(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords) {
    init_point_givers();
    int n = 40;
    models.resize(0);
    coords.resize(0);
    Eigen::Vector3d max = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff();
    Eigen::Vector3d min = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
    double z_length = max[2]*basicScene->level1->scale_factor[2] - min[2]*basicScene->level1->scale_factor[2];
    double y_length = max[1]*basicScene->level1->scale_factor[1] - min[1]*basicScene->level1->scale_factor[1];
    double x_length = max[0]*basicScene->level1->scale_factor[0] - min[0]*basicScene->level1->scale_factor[0];
    std::vector<double> objects_in_space_z = Calculates::getInstance()->linspace(5,z_length/2,n);
    std::vector<double> objects_in_space_y = Calculates::getInstance()->linspace(5,y_length/2,n);
    std::vector<double> objects_in_space_x = Calculates::getInstance()->linspace(5,x_length/2,n);
    int scale = 5;
    std::vector<Calculates::ObjectInfo> cubes;
    Calculates::getInstance()->setRandomCubeLocations(x_length, y_length, z_length, n,  scale, cubes);

    for (int i = 0; i < n; i++) {
        Eigen::Vector3f position = cubes[i].position.cast<float>();
        coords.push_back(position);
        std::shared_ptr<Model> cube = ModelsFactory::getInstance()->CreateModel(BRICKS_MATERIAL,CUBE,std::string(COLLISION_OBJECT) + "bricks");
        cube->SetTreeAndCube(ModelsFactory::getInstance()->bounding_boxes[CUBE],ModelsFactory::getInstance()->trees[CUBE]);
        basicScene->GetRoot()->AddChild(cube);
        cube->Translate(position);
        cube->Scale(scale);
    }
}


void ObjectsAnimationVisitor::GetCurrMapMaxLength(float &length_x, float &length_y, float &length_z){
    Eigen::Vector3d max = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff();
    Eigen::Vector3d min = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();
    length_z = max[2]*basicScene->level1->scale_factor[2] - min[2]*basicScene->level1->scale_factor[2];
    length_y = max[1]*basicScene->level1->scale_factor[1] - min[1]*basicScene->level1->scale_factor[1];
    length_x = max[0]*basicScene->level1->scale_factor[0] - min[0]*basicScene->level1->scale_factor[0];
}


void ObjectsAnimationVisitor::init_point_givers() {
    float dist = basicScene->level1->scale_factor[0];
    int half = dist/2;
    Eigen::Vector3f min_to_gen, max_to_gen;
    get_map_max_min(max_to_gen, min_to_gen);
    for (int i = 0; i < num_of_models; i++) {
        Eigen::Vector3f frog_point, mouse_point, coin_point;
        if (i % 2 == 0){
            frog_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},0 ,half);
            mouse_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},0 ,half);
            coin_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},0 ,half);
        }else{
            frog_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},half ,dist);
            mouse_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},half ,dist);
            coin_point = Calculates::getInstance()->generateRandomPoint(max_to_gen,min_to_gen,{0.0f,0.0f,0.0f},half ,dist);
        }
        frogPoints.push(frog_point);
        mousePoints.push(mouse_point);
        coinPoints.push(coin_point);
    }

}
Eigen::Vector3f ObjectsAnimationVisitor::get_point(std::queue<Eigen::Vector3f> &coords){
    //create new point.
    if(coords.size()<=0){
        int dist = basicScene->level1->scale_factor[0];
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
