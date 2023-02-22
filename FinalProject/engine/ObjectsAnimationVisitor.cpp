//
// Created by יובל היטר on 12/02/2023.
//

#include "ObjectsAnimationVisitor.h"
#include "../tutorial/Final/IglMeshLoader.h"
#include <igl/per_vertex_normals.h>
#include <iostream>
#include <random>
#include <utility>
#include "../tutorial/Final/Calculates.h"
#include "../tutorial/Final//ObjectsNames.h"
#include "../tutorial/Final//ModelsFactory.h"

void ObjectsAnimationVisitor::Run(Scene *scene, Camera *camera) {
    basicScene = (BasicScene *)scene;
    if(!is_visitor_inited){
        //don't need to init it anymore
        is_visitor_inited = true;
        program = std::make_shared<Program>("../tutorial/shaders/phongShader");
        //material1
        material =  std::make_shared<Material>("material", program); // empty material
        material->AddTexture(0, "textures/box0.bmp", 2);


        generateObjectBezier(BRICKS_MATERIAL,SPHERE, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
        std::shared_ptr<Model> truck = generateObjectBezier(PHONG_MATERIAL,TRUCK, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
        generateObjectBezier(PHONG_MATERIAL,SPHERE, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
        generateObjectBezier(PHONG_MATERIAL,CUBE, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
//        auto coin = ModelsFactory::getInstance()->CreateModel(BASIC_MATERIAL,COIN,"coin");
//       auto program = std::make_shared<Program>("shaders/basicShader");
//
        // health
//        auto material1 = std::make_shared<Material>("material", program);
//        auto coin = Model::Create("health", "data/coin.obj", material1);
//        basicScene->GetRoot()->AddChild(coin);
//        coin->Translate(Eigen::Vector3f(0.0f,0.0f,0.0f) - coin->GetPosition());
//        coin->Scale(3.0f);
        std::vector<std::shared_ptr<Model>> models;
        std::vector<Eigen::Vector3f> coords;
        //CreateLevel1(models, coords);

    }
    Visitor::Run(scene, camera);
}

void ObjectsAnimationVisitor::Visit(Model *model) {
    if(basicScene->animate && model->name.substr(0,strlen(BEZIER_OBJECT_NAME)) == std::string(BEZIER_OBJECT_NAME)) {
        if (model->t <= 1 && !model->moveBackwards) {
            model->t += 0.04*model->bezier_speed;
            moveAccordingToBeizerCurve(model);
        } else {
            if (!model->moveBackwards)
                model->moveBackwards = true;
            if (model->moveBackwards) {
                model->t -= 0.04*model->bezier_speed;
                moveAccordingToBeizerCurve(model);
                if (model->t <= 0)
                    model->moveBackwards = false;
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
    cube->stopper.start();
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

void ObjectsAnimationVisitor::CreateLevel1(std::vector<shared_ptr<Model>> &models, std::vector<Eigen::Vector3f> &coords) {
    int n = 200;

    Eigen::Vector3d max = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().maxCoeff();
    Eigen::Vector3d min = basicScene->level1->GetMeshList()[0]->data[0].vertices.colwise().minCoeff();

    double z_length = max[2]*basicScene->level1->scale_factor[2] - min[2]*basicScene->level1->scale_factor[2];
    double y_length = max[1]*basicScene->level1->scale_factor[1] - min[1]*basicScene->level1->scale_factor[1];
    double x_length = max[0]*basicScene->level1->scale_factor[0] - min[0]*basicScene->level1->scale_factor[0];
    double dist_between_objects_z = z_length/n;
    double dist_between_objects_y = y_length/n;
    double dist_between_objects_x = x_length/n;
    std::vector<double> objects_in_space_z = Calculates::getInstance()->linspace(5,z_length/2,n);
    std::vector<double> objects_in_space_y = Calculates::getInstance()->linspace(5,y_length/2,n);
    std::vector<double> objects_in_space_x = Calculates::getInstance()->linspace(5,x_length/2,n);
    int scale = 3;
    std::vector<Eigen::Vector3d> cubes;
    Calculates::getInstance()->setRandomCubeLocations(x_length, y_length, z_length, n,  scale, cubes);


    for (int i = 0; i < n; i++) {
        //Eigen::Vector3f position = {objects_in_space_x[i],objects_in_space_y[i],objects_in_space_z[i]};
        Eigen::Vector3f position = cubes[i].cast<float>();
        coords.push_back(position);
        std::shared_ptr<Model> cube = ModelsFactory::getInstance()->CreateModel(BRICKS_MATERIAL,CUBE,"test");
        basicScene->GetRoot()->AddChild(cube);
        cube->Translate(position);
        cube->Scale(scale);
    }

}







