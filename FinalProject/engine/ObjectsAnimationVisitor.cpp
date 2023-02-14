//
// Created by יובל היטר on 12/02/2023.
//

#include "ObjectsAnimationVisitor.h"
#include "../tutorial/Final/IglMeshLoader.h"
#include <igl/per_vertex_normals.h>
#include <iostream>
#include <random>
#include <utility>
void ObjectsAnimationVisitor::Run(Scene *scene, Camera *camera) {
    basicScene = (BasicScene *)scene;
    if(!is_visitor_inited){
        //don't need to init it anymore
        is_visitor_inited = true;
        program = std::make_shared<Program>("../tutorial/shaders/phongShader");
        //material1
        material =  std::make_shared<Material>("material", program); // empty material
        material->AddTexture(0, "textures/box0.bmp", 2);
        GenerateSphereObject(material);
        GenerateSphereObject(material);
        GenerateCubeObject(material);
    }
    Visitor::Run(scene, camera);
}

void ObjectsAnimationVisitor::Visit(Model *model) {
    if(basicScene->animate && model->name.substr(0,15) == std::string("CollisionObject")) {
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

void ObjectsAnimationVisitor::GenerateCubeObject(const std::shared_ptr<Material>& _material) {
    auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","../tutorial/data/cube_old.obj")};
    auto cube = Model::Create( "CollisionObject cube", cubeMesh, _material);
    basicScene->GetRoot()->AddChild(cube);
    cube->showWireframe = true;
    Eigen::Vector3f location = Eigen::Vector3f (generate_random_number(minx,maxx),generate_random_number(miny,maxy),generate_random_number(minz,maxz));
    cube->Translate(location);
    setModelBezier(location,cube.get());
}

void ObjectsAnimationVisitor::GenerateSphereObject(const std::shared_ptr<Material>& _material) {
    auto sphereMesh{IglLoader::MeshFromFiles("sphere_igl", "../tutorial/data/sphere.obj")};
    shared_ptr<Model> sphere1 = Model::Create( "CollisionObject sphere",sphereMesh, _material);
    basicScene->GetRoot()->AddChild(sphere1);
    sphere1->showWireframe = true;
    Eigen::Vector3f location = Eigen::Vector3f (generate_random_number(minx,maxx),generate_random_number(miny,maxy),generate_random_number(minz,maxz));
    sphere1->Translate(location);
    setModelBezier(location,sphere1.get());
}
void ObjectsAnimationVisitor::setModelBezier(Eigen::Vector3f vectors, Model *model){
    generateBeizerCurve(model,std::move(vectors));
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


    Eigen::Vector3f               currentPosition;
    currentPosition = (Ti * model->MG_Result);

    Eigen::Vector3f oldPositionOfObject = model->GetPosition();
    model->Translate(currentPosition - oldPositionOfObject);

}

void ObjectsAnimationVisitor::drawTheBeizerCurve(Model *model) {
    int number_of_points_in_bezier = 100;
    std::vector<double> line_space = linspace(0,1,number_of_points_in_bezier);
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

void ObjectsAnimationVisitor::generateBeizerCurve(Model *model, Eigen::Vector3f vector) {
    //Slide 15 Beizer_Curves_And_surfaces.pdf
    Eigen::Vector3f pRow1;
    Eigen::Vector3f pRow2;
    Eigen::Vector3f pRow3;
    Eigen::Vector3f pRow4;
    float xCoordinate = vector[0];
    float yCoordinate = vector[1];
    float zCoordinate = vector[2];

    std::random_device dev;
    std::mt19937       rng(dev());
    std::uniform_int_distribution<int> distributeX(xCoordinate, xCoordinate + 4);
    std::uniform_int_distribution<int> distributeY(yCoordinate - 1, yCoordinate + 5);
    std::uniform_int_distribution<int> distributeZ(zCoordinate - 5, zCoordinate + 5);

    pRow1 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));
    pRow2 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));
    pRow3 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));
    pRow4 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));
    Eigen::Matrix <float, 4, 3 > curvePoints;
    curvePoints.row(0) = pRow1;
    curvePoints.row(1) = pRow2;
    curvePoints.row(2) = pRow3;
    curvePoints.row(3) = pRow4;
    Eigen::Matrix4f               M;
    M << -1, 3, -3, 1,
            3, -6, 3, 0,
            -3, 3, 0, 0,
            1, 0, 0, 0;

    model->MG_Result = M * curvePoints;
}
std::vector<double> ObjectsAnimationVisitor::linspace(float start_in, float end_in, int num_in)
{

    std::vector<double> linspaced;

    double start = static_cast<double>(start_in);
    double end = static_cast<double>(end_in);
    double num = static_cast<double>(num_in);

    if (num == 0) { return linspaced; }
    if (num == 1)
    {
        linspaced.push_back(start);
        return linspaced;
    }

    double delta = (end - start) / (num - 1);

    for(int i=0; i < num-1; ++i)
    {
        linspaced.push_back(start + delta * i);
    }
    linspaced.push_back(end);
    return linspaced;
}





