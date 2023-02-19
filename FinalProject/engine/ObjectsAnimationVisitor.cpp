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
        auto truck = generateObjectBezier(PHONG_MATERIAL,TRUCK, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
        generateObjectBezier(PHONG_MATERIAL,SPHERE, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
        generateObjectBezier(PHONG_MATERIAL,CUBE, std::string(BEZIER_OBJECT_NAME) + " cube", 3.0f);
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
    moveAccordingToBeizerCurve(model.get());
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






