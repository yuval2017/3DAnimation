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
void ObjectsAnimationVisitor::Run(Scene *scene, Camera *camera) {
    basicScene = (BasicScene *)scene;
    if(!is_visitor_inited){
        //don't need to init it anymore
        is_visitor_inited = true;
        program = std::make_shared<Program>("../tutorial/shaders/phongShader");
        //material1
        material =  std::make_shared<Material>("material", program); // empty material
        material->AddTexture(0, "textures/box0.bmp", 2);
        GenerateSphereObject(material,BEZIER_OBJECT_NAME);
        GenerateSphereObject(material, BEZIER_OBJECT_NAME);
        GenerateCubeObject(material, BEZIER_OBJECT_NAME);
    }
    Visitor::Run(scene, camera);
}

void ObjectsAnimationVisitor::Visit(Model *model) {
    if(basicScene->animate && model->name.substr(0,strlen(BEZIER_OBJECT_NAME)) == std::string(BEZIER_OBJECT_NAME)) {
        if (model->t <= 1 && !model->moveBackwards) {
            model->t += 0.004;
            moveAccordingToBeizerCurve(model);
        } else {
            if (!model->moveBackwards)
                model->moveBackwards = true;
            if (model->moveBackwards) {
                model->t -= 0.004;
                moveAccordingToBeizerCurve(model);
                if (model->t <= 0)
                    model->moveBackwards = false;
            }
        }
    }
}

void ObjectsAnimationVisitor::GenerateCubeObject(const std::shared_ptr<Material>& _material, std::string prefix) {
    auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","../tutorial/data/cube_old.obj")};
    auto cube = Model::Create( prefix + " cube", cubeMesh, _material);
    basicScene->GetRoot()->AddChild(cube);
    cube->showWireframe = true;
    Eigen::Vector3f location = Eigen::Vector3f (generate_random_number(minx,maxx),generate_random_number(miny,maxy),generate_random_number(minz,maxz));
    cube->Translate(location);
    cube->Scale(3.0f,Movable::Axis::XYZ);
    setModelBezier(location,cube.get());
}

void ObjectsAnimationVisitor::GenerateSphereObject(const std::shared_ptr<Material>& _material, std::string prefix) {
    auto sphereMesh{IglLoader::MeshFromFiles("sphere_igl", "../tutorial/data/sphere.obj")};
    shared_ptr<Model> sphere1 = Model::Create( prefix + " sphere",sphereMesh, _material);
    basicScene->GetRoot()->AddChild(sphere1);
    sphere1->showWireframe = true;
    Eigen::Vector3f location = Eigen::Vector3f (generate_random_number(minx,maxx),generate_random_number(miny,maxy),generate_random_number(minz,maxz));
    sphere1->Translate(location);
    sphere1->Scale(3.0f,Movable::Axis::XYZ);
    //call this for complicate meshes
    sphere1->GetTreeWithCube();
    setModelBezier(location,sphere1.get());
}
void ObjectsAnimationVisitor::setModelBezier(Eigen::Vector3f vectors, Model *model){
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

    Eigen::MatrixXd vertices(number_of_points_in_bezier,3);
    for (int i = 0; i < number_of_points_in_bezier; ++i) {
        Eigen::Vector3d curr_v = calcForDraw(line_space[i],model).cast<double>();
        vertices.row(i) = curr_v;
    }

    Eigen::MatrixXi faces(number_of_points_in_bezier-1,2);
    for (int i = 0; i < number_of_points_in_bezier-1; ++i) {

        faces.row(i) = Eigen::Vector2i(i,i+1);
    }

    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(number_of_points_in_bezier,3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(number_of_points_in_bezier,2);
    auto program1 = std::make_shared<Program>("shaders/basicShader");

    auto material1{ std::make_shared<Material>("material", program1)}; // empty material

    std::shared_ptr<Mesh> coordsys = std::make_shared<Mesh>("coordsys",vertices,faces,vertexNormals,textureCoords);
    std::shared_ptr<Model> bezier = Model::Create("bez",coordsys,material1);
    bezier->mode = 1;
    model->bezier = bezier;
    basicScene->GetRoot()->AddChild(bezier);


}
Eigen::Vector3f ObjectsAnimationVisitor::calcForDraw(float ti, Model *model){
    Eigen::RowVector4f Ti;
    Ti[0] = powf(ti, 3);
    Ti[1] = powf(ti, 2);
    Ti[2] = ti;
    Ti[3] = 1;
    return Eigen::Vector3f(Ti * model->MG_Result);
}





