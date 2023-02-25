//
// Created by יובל היטר on 12/02/2023.
//

#include "ObjectsAnimationVisitor.h"
#include "../tutorial/Final/IglMeshLoader.h"
#include <igl/per_vertex_normals.h>
#include <iostream>
#include <random>
#include <utility>
//#include "../tutorial/Final/Calculates.h"
#include "../tutorial/Final//ObjectsNames.h"
#include "../tutorial/Final//ModelsFactory.h"
#include "iostream"

// Keep track of elapsed time
static float elapsed_time = 0.0f;

void ObjectsAnimationVisitor::Run(Scene *scene, Camera *camera) {
    basicScene = (BasicScene *)scene;
    calculates = Calculates::getInstance();
    //The point objects consts for the run method.
    static std::vector<Eigen::Vector3d> frog_points;
    static std::vector<Eigen::Vector3d> mouse_points;
    static std::vector<Eigen::Vector3d> coin_points;

    float x_length;
    float y_length;
    float z_length;
    //system coordinates.
     GetCurrMapMaxLength(x_length, y_length,z_length);
    //Setting time intervals and elapsing times for the points objects.
    x_length/=2;
    y_length/=2;
    z_length/=2;
    //The init part.
    if(!is_visitor_inited){
        //stopperCoin = new Stopper();
        stopperFrog = new Stopper();
        stopperFrog->start(sec1);
        stopperMouse = new Stopper();
        stopperMouse->start(sec2);
        stopperCoin = new Stopper();
        stopperCoin->start(sec3);
        init_point_givers(x_length,y_length,z_length);
        //don't need to init it anymore
        is_visitor_inited = true;
        program = std::make_shared<Program>("../tutorial/shaders/phongShader");
        //material1
        material =  std::make_shared<Material>("material", program); // empty material
        material->AddTexture(0, "textures/box0.bmp", 2);
        //initiating the system length for the objects positions.

        //generating the Bezier objects.
        generateObjectBezier(BRICKS_MATERIAL,SPHERE, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
        std::shared_ptr<Model> truck = generateObjectBezier(PHONG_MATERIAL,TRUCK, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
        generateObjectBezier(PHONG_MATERIAL,SPHERE, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
        generateObjectBezier(PHONG_MATERIAL,CUBE, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);

        //init the first level.
        CreateLevel1(models, coords);
        basicScene->init_flags[1]= true;
        basicScene->done++;
        basicScene->progress = basicScene->done/basicScene->len;

    }
    else{
        if (basicScene->animate & !(stopperFrog->is_countdown_running())) {
            for(auto frog : frogs_available){
                if(frog->isHidden){
                    frog->isHidden = false;
                    Eigen::Vector3f pos = frogPoints.front();
                    frogPoints.pop();
                    frogPoints.push(pos);
                    frog->Translate(pos);
                    std::cout<< "added frog model : "<<pos[0] <<", " << pos[1] <<", " << pos[2] <<std::endl;
                    stopperFrog->start(sec1);
                    frog->stopper.start(len1);
                    break;
                }
            }
        }
        if (basicScene->animate & !(stopperMouse->is_countdown_running())) {
            for(auto mouse : mouses_available){
                if(mouse->isHidden){
                    mouse->isHidden =false;
                    Eigen::Vector3f pos = mousePoints.front();
                    mousePoints.pop();
                    mousePoints.push(pos);
                    mouse->Translate(pos);
                    std::cout<< "added mouse model : "<<pos[0] <<", " << pos[1] <<", " << pos[2] <<std::endl;
                    stopperMouse->start(sec2);
                    mouse->stopper.start(len2);
                    break;
                }
            }
        }
        if (basicScene->animate & !(stopperCoin->is_countdown_running())) {
            for(auto coin : coins_available){
                if(coin->isHidden){
                    coin->isHidden =false;
                    Eigen::Vector3f pos = coinPoints.front();
                    coinPoints.pop();
                    coinPoints.push(pos);
                    coin->Translate(pos);
                    std::cout<< "added coin model: "<<pos[0] <<", " << pos[1] <<", " << pos[2] <<std::endl;
                    stopperCoin->start(sec3);
                    coin->stopper.start(len3);
                    break;
                }
            }
        }
    }
    if(basicScene->getStatistics()->levelUp){
        basicScene->animate = false;
        removeFormerlevel();
        loadNextLevel(basicScene->getStatistics()->level+1);
    }
    if(basicScene->getStatistics()->restart){
        basicScene->animate = false;
        removeFormerlevel();
        loadNextLevel(1);
        basicScene->getStatistics()->restart = false;
    }

    Visitor::Run(scene, camera);
}

void ObjectsAnimationVisitor::removeFormerlevel(){

    for(shared_ptr<Model> model : models) {
        if (model != nullptr) {
            basicScene->GetRoot()->RemoveChild(model);
        }
    }

}
void ObjectsAnimationVisitor::loadNextLevel(int nextLevel){
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
    if(basicScene->animate) {
        if(model->name.find(TIMING) != std::string::npos){
            if(!model->stopper.is_countdown_running()){
                model->isHidden =true;
                std::cout << "model becomes hidden: "<< model->name << std::endl;
            }
        }
        else if (model->name.substr(0, strlen(BEZIER_OBJECT_NAME)) == std::string(BEZIER_OBJECT_NAME)) {
            if (model->t <= 1 && !model->moveBackwards) {
                model->t += 0.04 * model->bezier_speed;
                moveAccordingToBeizerCurve(model);
            } else {
                if (!model->moveBackwards)
                    model->moveBackwards = true;
                if (model->moveBackwards) {
                    model->t -= 0.04 * model->bezier_speed;
                    moveAccordingToBeizerCurve(model);
                    if (model->t <= 0)
                        model->moveBackwards = false;
                }
            }
        }
    }
}

std::shared_ptr<Model> ObjectsAnimationVisitor::generateObjectBezier(int material_id ,int model_id, std::string name, float scale){
    std::shared_ptr<Model> cube = ModelsFactory::getInstance()->CreateModel(material_id,model_id,name);
    basicScene->GetRoot()->AddChild(cube);
    cube->showWireframe = true;
    Eigen::Vector3f location = Eigen::Vector3f (generate_random_number(minx,maxx),generate_random_number(miny,maxy),generate_random_number(minz,maxz));
    cube->Translate(location);
    cube->Scale(scale,Movable::Axis::XYZ);
    setModelBezier(location,cube);
    cube->GetTreeWithCube();
    return cube;
}

void ObjectsAnimationVisitor::setModelBezier(Eigen::Vector3f vectors,std::shared_ptr<Model> model){
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

void ObjectsAnimationVisitor::drawTheBeizerCurve(std::shared_ptr<Model> model) {
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
    return frog;
}


shared_ptr<Model> ObjectsAnimationVisitor::createMouse(){

    ModelsFactory* factory = ModelsFactory::getInstance();
    shared_ptr<Model> mouse=  factory->CreateModel(GREY_MATERIAL , MOUSE ,std::string(EATING_OBJECT)+ std::string(TIMING)+ std::string(MOUSE_NAME));
    mouse->GetTreeWithCube();
    mouse->material->program->name = "grey";
    return mouse;
}

shared_ptr<Model> ObjectsAnimationVisitor::createCoin(){

    ModelsFactory* factory = ModelsFactory::getInstance();
    shared_ptr<Model> coin =  factory->CreateModel(GOLD_MATERIAL , COIN ,std::string(EATING_OBJECT)+  std::string(TIMING)+ std::string(COIN_NAME));
    coin->material->program->name = "gold";
    return coin;
}

void ObjectsAnimationVisitor::CreateLevel1(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords) {
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
        shared_ptr<Model> cube = ModelsFactory::getInstance()->CreateModel(BRICKS_MATERIAL,CUBE,"test");
        models.push_back(cube);
        basicScene->GetRoot()->AddChild(cube);
        cube->showWireframe = true;
        cube->Translate(position);
        cube->Scale(scale);
        cube->GetTreeWithCube();
    }


}

void ObjectsAnimationVisitor::CreateLevel2(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords) {
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
        //Eigen::Vector3f position = {objects_in_space_x[i],objects_in_space_y[i],objects_in_space_z[i]};
        Eigen::Vector3f position = cubes[i].position.cast<float>();
        coords.push_back(position);
        std::shared_ptr<Model> cube = ModelsFactory::getInstance()->CreateModel(BRICKS_MATERIAL,CUBE,"test");
        basicScene->GetRoot()->AddChild(cube);
        cube->Translate(position);
        cube->Scale(scale);
    }
}



void ObjectsAnimationVisitor::CreateLevel3(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords) {
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
        //Eigen::Vector3f position = {objects_in_space_x[i],objects_in_space_y[i],objects_in_space_z[i]};
        Eigen::Vector3f position = cubes[i].position.cast<float>();
        coords.push_back(position);
        std::shared_ptr<Model> cube = ModelsFactory::getInstance()->CreateModel(BRICKS_MATERIAL,CUBE,"test");
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


void ObjectsAnimationVisitor::init_point_givers(float x_length , float y_length, float z_length) {

    //create the points.
    frogPoints = calculates->generatePointsInSystem(x_length, y_length, z_length, num_of_points,min_dist,
                                               basicScene->snake->get_snake_head(),coords);
    mousePoints = calculates->generatePointsInSystem(x_length, y_length, z_length, num_of_points,min_dist,
                                                     basicScene->snake->get_snake_head(),coords);
    coinPoints = calculates->generatePointsInSystem(x_length, y_length, z_length, num_of_points,min_dist,
                                                    basicScene->snake->get_snake_head(),coords);

    for( int i =0; i< num_of_models ; i++){
        //add frogs.
        shared_ptr<Model> frog_model = createFrog();
        frog_model->isHidden = true;
        frogs_available.push_back(frog_model);
        basicScene->GetRoot()->AddChild(frog_model);
        frog_model->Rotate(-M_PI/2.0f, Movable::Axis::X);
        frog_model->Rotate(M_PI, Movable::Axis::Z);
        //frog_model->Scale(2.0f);
        frog_model->GetTreeWithCube();
        //std::cout << "frog model created"<< std::endl;


        //add mouses
        shared_ptr<Model> mouse_model = createMouse();
        mouse_model->isHidden = true;
        mouses_available.push_back(mouse_model);
        basicScene->GetRoot()->AddChild(mouse_model);
        mouse_model->Rotate(-M_PI/2.0f, Movable::Axis::X);
        //mouse_model->Scale(2.0f);
        mouse_model->GetTreeWithCube();
        //std::cout << "mouse model created"<< std::endl;


        //add coins
        shared_ptr<Model> coin_model = createCoin();
        coin_model->isHidden = true;
        coins_available.push_back(coin_model);
        basicScene->GetRoot()->AddChild(coin_model);
        coin_model->Rotate(-M_PI/2.0f, Movable::Axis::X);
        //coin_model->Scale(2.0f);
        coin_model->GetTreeWithCube();
        //std::cout << "coin model created"<< std::endl;
    }

}