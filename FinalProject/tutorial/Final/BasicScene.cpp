#include "BasicScene.h"
#include <Eigen/src/Core/Matrix.h>
#include <edges.h>
#include <memory>
#include <per_face_normals.h>
#include <read_triangle_mesh.h>
#include <utility>
#include <vector>
#include "GLFW/glfw3.h"
#include "Mesh.h"
#include "PickVisitor.h"
#include "Renderer.h"
#include "ObjLoader.h"
#include "IglMeshLoader.h"

#include "igl/per_vertex_normals.h"
#include "igl/per_face_normals.h"
#include "igl/unproject_onto_mesh.h"
#include "igl/edge_flaps.h"
#include "igl/loop.h"
#include "igl/upsample.h"
#include "igl/AABB.h"
#include "igl/parallel_for.h"
#include "igl/shortest_edge_and_midpoint.h"
#include "igl/circulation.h"
#include "igl/edge_midpoints.h"
#include "igl/collapse_edge.h"
#include "igl/edge_collapse_is_valid.h"
#include "igl/write_triangle_mesh.h"

// #include "AutoMorphingModel.h"

using namespace cg3d;

void BasicScene::Init(float fov, int width, int height, float near, float far)
{
    camera = Camera::Create( "camera", fov, float(width) / height, near, far);

    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes
    auto daylight{std::make_shared<Material>("daylight", "shaders/cubemapShader")}; 
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{Model::Create("background", Mesh::Cube(), daylight)};
    AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();

    shouldAnimateFabrik= false;
    shouldAnimateCCD = false;
    decent = true;
    float scaleFactor = 1;
    angle = 0.3f;
    num_of_cyls = 3;
    lastLinkIndex = num_of_cyls -1;
    firstLinkIndex = 0 ;
    auto program = std::make_shared<Program>("shaders/phongShader");
    auto program1 = std::make_shared<Program>("shaders/pickingShader");
    auto material{ std::make_shared<Material>("material", program)}; // empty material
    auto material1{ std::make_shared<Material>("material", program1)}; // empty material
    material->AddTexture(0, "textures/box0.bmp", 2);
    auto sphereMesh{IglLoader::MeshFromFiles("sphere_igl", "data/sphere.obj")};
    auto cylMesh {IglLoader::MeshFromFiles("cyl_igl","data/zcylinder.obj")};
    auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","data/cube_old.obj")};
    sphere1 = Model::Create( "sphere",sphereMesh, material);    
    cube = Model::Create( "cube", cubeMesh, material);


    //Axis
    Eigen::MatrixXd vertices(6,3);
    vertices << -1,0,0,1,0,0,0,-1,0,0,1,0,0,0,-1,0,0,1;
    Eigen::MatrixXi faces(3,2);
    faces << 0,1,2,3,4,5;
    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(6,3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(6,2);
    auto coordsys = std::make_shared<Mesh>("coordsys",vertices,faces,vertexNormals,textureCoords);
    axis.push_back(Model::Create("axis",coordsys,material1));
    axis[0]->mode = 1;   
    axis[0]->Scale(4,Axis::XYZ);
    auto system = camera->GetRotation().transpose();

    root->AddChild(axis[0]);
    parents = *new std::vector<int>(1);
    children = *new std::vector<int>(1);
    //set parents and children vectors to number of cylinders.
    num_of_cyls = 3;
    lastLinkIndex = num_of_cyls -1;
    firstLinkIndex = 0 ;
    parents.resize(num_of_cyls);
    children.resize(num_of_cyls);
    cyls.push_back( Model::Create("cyl",cylMesh, material));
    cyls[firstLinkIndex]->Scale(scaleFactor,Axis::Z);
    cyls[firstLinkIndex]->SetCenter(Eigen::Vector3f(0,0,-0.8f*scaleFactor));
    root->AddChild(cyls[firstLinkIndex]);

    for(int i = 1;i < num_of_cyls; i++)
    { 
        cyls.push_back( Model::Create("cyl", cylMesh, material));
        cyls[i]->Scale(scaleFactor,Axis::Z);
        cyls[i]->Translate(1.6f*scaleFactor,Axis::Z);
        cyls[i]->SetCenter(Eigen::Vector3f(0,0,-0.8f*scaleFactor));
        cyls[i-1]->AddChild(cyls[i]);
        axis.push_back(Model::Create("axis",coordsys,material1));
        axis[i]->mode = 1;
        axis[i]->Scale(4,Axis::XYZ);
        axis[i]->Translate(0.8f*scaleFactor,Axis::Z);
        cyls[i-1]->AddChild(axis[i]);
    }

    for (int i =0 ; i < num_of_cyls; i++ ){
        //set parent/children values in vectors
        if (i == 0) {
            parents[i] = -1;
        }
        else {
            parents[i] = i - 1;
        }//set parent
        if (i  < lastLinkIndex) {
            children[i] = i + 1;
        }
        else
            children[i] = -1;
    }


    cyls[firstLinkIndex]->Translate({0,0, 0.8f*scaleFactor});
    root->RotateByDegree(90,Eigen::Vector3f(-1,0,0));

    auto morphFunc = [](Model* model, cg3d::Visitor* visitor) {
      return model->meshIndex;//(model->GetMeshList())[0]->data.size()-1;
    };
    autoCube = AutoMorphingModel::Create(*cube, morphFunc);

  
    sphere1->showWireframe = true;
    autoCube->Translate({-6,0,0});
    autoCube->Scale(1.5f);
    sphere1->Translate({3,1,0});
    link_len = cyls[firstLinkIndex]->GetMeshList()[0]->data[cyls[firstLinkIndex]->meshIndex].vertices.colwise().maxCoeff()[2]*2;
    autoCube->showWireframe = true;
    camera->Translate(22, Axis::Z);
    root->AddChild(sphere1);
    root->AddChild(autoCube);

    cube->mode =1;
    auto mesh = cube->GetMeshList();
    root->Translate(Eigen::Vector3f(0,0,0));
}

void BasicScene::IKFabric(){
    {
        if (shouldAnimateFabrik) {
            std::vector<Eigen::Vector3f> p;
            p.resize(num_of_cyls + 1 );
            Eigen::Vector3f target = sphere1->GetAggregatedTransform().col(3).head(3);
            Eigen::Vector3f p1 = ikGetPosition(firstLinkIndex, -link_len/2);


            int curr = lastLinkIndex;
            while (curr != -1) {
                p[curr] = ikGetPosition(curr, -link_len/2);
                curr = parents[curr];
            }
             p[lastLinkIndex + 1] = ikGetPosition(lastLinkIndex, link_len/2);
            std::vector<double> ris_Array;
            std::vector<double> lambdaI_Array;

            ris_Array.resize(num_of_cyls + 1 );
            lambdaI_Array.resize(num_of_cyls + 1);
            //if unreachable
            if ((target - p1).norm() > link_len * float(num_of_cyls)) {
                std::cout << "cannot reach" << std::endl;
                shouldAnimateFabrik = false;
                return;
            }
            else
            {
                //1.10
                //the target is reachable
                Eigen::Vector3f b = p[firstLinkIndex];

                //the distance from object more then  tolerance .
                Eigen::Vector3f endEffector = p[lastLinkIndex +1];
                float tolerance = 0.05;

                float diffA = (endEffector - target).norm();
                if (diffA < tolerance) {
                    std::cout << "distance : " << diffA << "\n" << std::endl;
                    shouldAnimateFabrik = false;
                    return;
                }
                else {
                    while (diffA > tolerance) {
                        //	1.19 foward to reach
                        p[lastLinkIndex + 1] = target;
                        int parent = lastLinkIndex;
                        int child = lastLinkIndex + 1;
                        while (parent != -1) {

                            // Find the distance ri between the new joint position pi+1 and the joint pi
                            //steps 1.24-1.25
                            ris_Array[parent] = (p[child] - p[parent]).norm();
                            lambdaI_Array[parent] = link_len / ris_Array[parent];

                            //step 1.27
                            //calculate the new position.
                            p[parent] = (1 - lambdaI_Array[parent]) * p[child] + lambdaI_Array[parent] * p[parent];
                            child = parent;
                            parent = parents[parent];

                        }

                        //Stage 2: 1.29 - reach backward
                        //Set p1 p0 to its initial position
                        p[firstLinkIndex] = b;
                        parent = firstLinkIndex;
                        child = children[firstLinkIndex];
                        while (child != -1) {
                            //1.33-1.36 Find the distance ri between the new joint position pi and the joint pi+1
                            ris_Array[parent] = (p[child] - p[parent]).norm();
                            lambdaI_Array[parent] = link_len / ris_Array[parent];
                            //1.37
                            p[child] = (1 - lambdaI_Array[parent]) * p[parent] + lambdaI_Array[parent] * p[child];
                            parent = child;
                            child = children[child];
                        }

                        ris_Array[lastLinkIndex] = (p[lastLinkIndex + 1] - p[lastLinkIndex]).norm();
                        lambdaI_Array[lastLinkIndex] = link_len / ris_Array[lastLinkIndex];
                   //1.27
                        p[lastLinkIndex + 1] = (1 - lambdaI_Array[lastLinkIndex]) * p[lastLinkIndex] +
                                               lambdaI_Array[lastLinkIndex] * p[lastLinkIndex + 1];
                  diffA = (p[lastLinkIndex + 1] - target).norm();
                    }
                    //rotate
                    int currLink = firstLinkIndex;
                    int target_id = children[firstLinkIndex];
                    while (target_id != -1) {
                        ikRotateHelper(currLink, p[target_id]);
                        currLink = target_id;
                        target_id = children[target_id];
                    }
                    ikRotateHelper(lastLinkIndex, p[lastLinkIndex + 1]);
                    double distance = (target - ikGetPosition(lastLinkIndex, link_len/2)).norm();
                    if (distance < tolerance){
                        shouldAnimateFabrik = false;
                        std::cout << "distance: " << distance << std::endl;

                    }
                }
            }
            animate = false;
        }
    }
}


void BasicScene::ikRotateHelper(int id, const Eigen::Vector3f& t){
    Eigen::Vector3f r = ikGetPosition(id, -link_len/2);
    Eigen::Vector3f e = ikGetPosition(id, link_len/2);
    Eigen::Vector3f rd = t - r;
    Eigen::Vector3f re = e - r;
    Eigen::Vector3f normal = re.normalized().cross(rd.normalized());
    float dot = rd.normalized().dot(re.normalized());//get dot

    if (dot > 1)
        dot = 1;
    if (dot < -1)
        dot = 1;
    float angle = (float)acos(dot)/100 ;
    Eigen::Vector3f rotationVec = (cyls[id]->GetAggregatedTransform()).block<3, 3>(0, 0).inverse() * normal;
    int parent = parents[id];
    Eigen::Matrix3f rot = (Eigen::AngleAxisf(angle,rotationVec.normalized())).toRotationMatrix();
    Eigen::Vector3d oi = rot.cast<double>().eulerAngles(2,0,2);

    cyls[id]->Rotate(oi[0],Axis::Z);
    cyls[id]->Rotate(oi[1], Axis::X);
    cyls[id]->Rotate(oi[2],Axis::Z);
    cyls[id]->Rotate(angle, rotationVec);
}

void BasicScene::IKCoordinateDecent(){
    if (shouldAnimateCCD) {
        Eigen::Vector3f target_des = sphere1->GetAggregatedTransform().col(3).head(3);
        Eigen::Vector3f first_link_pos = ikGetPosition(firstLinkIndex, -link_len/2);
        if ((target_des - first_link_pos).norm() > link_len * float(num_of_cyls)) {
            std::cout << "cannot reach" << std::endl;
            shouldAnimateCCD = false;
            return;
        }
        int currLink = lastLinkIndex;
        while (currLink != -1) {
            Eigen::Vector3f r = ikGetPosition(currLink, -link_len/2);
            Eigen::Vector3f e = ikGetPosition(lastLinkIndex, link_len/2);
            Eigen::Vector3f rd = target_des - r;
            Eigen::Vector3f re = e - r;


            Eigen::Vector3f normal = re.normalized().cross(rd.normalized());//returns the plane normal

            double distance = (target_des - e).norm();
            if (distance < 0.05) {
                std::cout << "distance: " << distance << std::endl;

                shouldAnimateCCD = false;
                return;
            }
            float dot = rd.normalized().dot(re.normalized());
            //check the angel
            if (dot > 1)
                dot = 1;
            if (dot < -1)
                dot = -1;
            double angle = acosf(dot) / 100;

            Eigen::Vector3f rotationVec = (cyls[currLink]->GetAggregatedTransform().block<3, 3>(0, 0).inverse()) * normal;
            int parent = parents[currLink];


            Eigen::Matrix3f rot = (Eigen::AngleAxisf(angle,rotationVec.normalized())).toRotationMatrix();
            Eigen::Vector3d oi = rot.cast<double>().eulerAngles(2,0,2);


            cyls[currLink]->Rotate(oi[0],Axis::Z);
            cyls[currLink]->Rotate(oi[1], Axis::X);
            cyls[currLink]->Rotate(oi[2],Axis::Z);
            currLink = parents[currLink];

        }
        animate = false;
    }
}



Eigen::Vector3f BasicScene::ikGetPosition(int id, float length){
    Eigen::Vector3f cen = cyls[id]->GetAggregatedTransform().col(3).head(3);
    Eigen::Vector3f mov = Eigen::Vector3f(0,0,length);
    return (cen+(cyls[id]->GetRotation()*mov));
}


void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    program.SetUniform4f("lightColor", 0.8f, 0.3f, 0.0f, 0.5f);
    program.SetUniform4f("Kai", 1.0f, 0.3f, 0.6f, 1.0f);
    program.SetUniform4f("Kdi", 0.5f, 0.5f, 0.0f, 1.0f);
    program.SetUniform1f("specular_exponent", 5.0f);
    program.SetUniform4f("light_position", 0.0, 15.0f, 0.0, 1.0f);
//    cyl->Rotate(0.001f, Axis::Y);
    cube->Rotate(0.1f, Axis::XYZ);

}


void BasicScene::Draw_changes() {
    IKCoordinateDecent();
    IKFabric();
}

void BasicScene::MouseCallback(Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[])
{
    // note: there's a (small) chance the button state here precedes the mouse press/release event

    if (action == GLFW_PRESS) { // default mouse button press behavior
        PickVisitor visitor;
        visitor.Init();
        renderer->RenderViewportAtPos(x, y, &visitor); // pick using fixed colors hack
        auto modelAndDepth = visitor.PickAtPos(x, renderer->GetWindowHeight() - y);
        renderer->RenderViewportAtPos(x, y); // draw again to avoid flickering
        pickedModel = modelAndDepth.first ? std::dynamic_pointer_cast<Model>(modelAndDepth.first->shared_from_this()) : nullptr;
        pickedModelDepth = modelAndDepth.second;
        camera->GetRotation().transpose();
        xAtPress = x;
        yAtPress = y;

        // if (pickedModel)
        //     debug("found ", pickedModel->isPickable ? "pickable" : "non-pickable", " model at pos ", x, ", ", y, ": ",
        //           pickedModel->name, ", depth: ", pickedModelDepth);
        // else
        //     debug("found nothing at pos ", x, ", ", y);

        if (pickedModel && !pickedModel->isPickable)
            pickedModel = nullptr; // for non-pickable models we need only pickedModelDepth for mouse movement calculations later

        if (pickedModel)
            pickedToutAtPress = pickedModel->GetTout();
        else
            cameraToutAtPress = camera->GetTout();
    }
}

void BasicScene::ScrollCallback(Viewport* viewport, int x, int y, int xoffset, int yoffset, bool dragging, int buttonState[])
{
    // note: there's a (small) chance the button state here precedes the mouse press/release event
    auto system = camera->GetRotation().transpose();
    if (pickedModel) {
        int i = 0;
        bool found = false;
        while (i < cyls.size() && !found) {
            if (pickedModel == cyls[i]) {
                found = true;
            } else {
                i++;
            }
        }
        if (found){
            cyls[firstLinkIndex]->TranslateInSystem(system * cyls[firstLinkIndex]->GetRotation(), { 0, 0, -float(yoffset) });
            pickedToutAtPress = pickedModel->GetTout();
        } else{
            pickedModel->TranslateInSystem(system * pickedModel->GetRotation(), { 0, 0, -float(yoffset) });
            pickedToutAtPress = pickedModel->GetTout();
        }
    } else {
        camera->TranslateInSystem(system, {0, 0, -float(yoffset)});
        cameraToutAtPress = camera->GetTout();
    }
}

void BasicScene::CursorPosCallback(Viewport* viewport, int x, int y, bool dragging, int* buttonState)
{
    if (dragging) {
        auto system = camera->GetRotation().transpose() * GetRotation();
        auto moveCoeff = camera->CalcMoveCoeff(pickedModelDepth, viewport->width);
        auto angleCoeff = camera->CalcAngleCoeff(viewport->width);
        if (pickedModel) {
            //pickedModel->SetTout(pickedToutAtPress);
            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE){
                bool found = false;
                int i = 0;
                while (i < cyls.size() && !found) {
                    if (pickedModel == cyls[i]) {
                        found = true;
                    } else {
                        i++;
                    }
                }
                if(found) {
                    cyls[firstLinkIndex]->TranslateInSystem(system * cyls[firstLinkIndex]->GetRotation(), { -float(xAtPress - x) / moveCoeff, float(yAtPress - y) / moveCoeff, 0 });
                } else{
                    pickedModel->TranslateInSystem(system * pickedModel->GetRotation(), { -float(xAtPress - x) / moveCoeff, float(yAtPress - y) / moveCoeff, 0 });
                }
            }
            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
                pickedModel->RotateInSystem(system, float(xAtPress - x) / angleCoeff, Axis::Z);
            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE) {
                bool found = false;
                int i = 0;
                while (i < cyls.size() && !found) {
                    if (pickedModel == cyls[i]) {
                        found = true;
                    } else {
                        i++;
                    }
                }
                if (found){
                    float z_angle = -float(xAtPress - x) / angleCoeff;
                    float y_angle = float(yAtPress - y) / angleCoeff;

                    pickedModel->Rotate(create_new_Rotation_q(pickedModel,1,z_angle));
                    pickedModel->Rotate(create_new_Rotation_q(pickedModel,2,y_angle));

                } else{
                    pickedModel->RotateInSystem(system * pickedModel->GetRotation(), -float(xAtPress - x) / angleCoeff, Axis::Y);
                    pickedModel->RotateInSystem(system * pickedModel->GetRotation(), -float(yAtPress - y) / angleCoeff, Axis::X);
                }
            }
        } else {
           // camera->SetTout(cameraToutAtPress);
            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE)
                root->TranslateInSystem(system, {-float(xAtPress - x) / moveCoeff/10.0f, float( yAtPress - y) / moveCoeff/10.0f, 0});
            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
                root->RotateInSystem(system, float(x - xAtPress) / 180.0f, Axis::Z);
            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE) {
                root->RotateInSystem(system, float(x - xAtPress) / angleCoeff, Axis::Y);
                root->RotateInSystem(system, float(y - yAtPress) / angleCoeff, Axis::X);
            }
        }
        xAtPress =  x;
        yAtPress =  y;
    }
}

void BasicScene::KeyCallback(Viewport* viewport, int x, int y, int key, int scancode, int action, int mods)
{
    auto system = camera->GetRotation().transpose();

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) // NOLINT(hicpp-multiway-paths-covered)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_SPACE:
                if(decent){
                    shouldAnimateCCD = !shouldAnimateCCD;
                }else{
                    shouldAnimateFabrik= !shouldAnimateFabrik;
                }
                break;
            case GLFW_KEY_C:
                decent = !decent;
                if(decent){
                    std::cout<< "in decent method \n" << std::endl;
                } else{
                    std::cout<< "in Fabrik method \n" << std::endl;
                }
                break;
            case GLFW_KEY_D:
                std::cout<< "destination position: \n" << sphere1->GetAggregatedTransform().col(3).head(3) <<std::endl;
                break;
            case GLFW_KEY_N: {
                bool found = false;
                int i = 0;
                while (i < cyls.size() && !found) {
                    if (pickedModel == cyls[i]) {
                        found = true;
                    } else {
                        i++;
                    }
                }
                int index = 0;
                if (!found) {
                    pickedModel = cyls[index];
                    pickedIndex = index;
                } else {
                    index = (i+1) % cyls.size();
                    pickedModel = cyls[index];
                    pickedIndex = index;
                }
                std::cout<< "num of cyl is: \n" << (index) <<std::endl;

                break;
            }
            case GLFW_KEY_UP:

                if(pickedModel) {
                    pickedModel->Rotate(  create_new_Rotation_q(pickedModel,2, angle) );
                } else{

                    root->RotateInSystem(system, angle, Axis::X);
                }
                break;
            case GLFW_KEY_DOWN:
                if (pickedModel) {
                    pickedModel->Rotate(  create_new_Rotation_q(pickedModel,2, -angle) );

                } else{
                    root->RotateInSystem(system, -angle, Axis::X);
                }
                break;
            case GLFW_KEY_LEFT:
                if (pickedModel) {
                    pickedModel->Rotate(   create_new_Rotation_q(pickedModel,1, -angle) );

                } else{
                    root->RotateInSystem(system, angle, Axis::Y);
                }
                break;
            case GLFW_KEY_RIGHT:
                if (pickedModel) {
                    pickedModel->Rotate(  create_new_Rotation_q(pickedModel,1, angle) );
                } else{
                    root->RotateInSystem(system, -angle, Axis::Y);
                }
                break;
            case GLFW_KEY_T:
                print_positions();
                break;
            case GLFW_KEY_P:
                print_rotation();
                break;
            case GLFW_KEY_W:
                camera->TranslateInSystem(system, {0, 0.1f, 0});
                break;
            case GLFW_KEY_R:
                num_of_cyls = 3;
                lastLinkIndex = num_of_cyls -1;
                firstLinkIndex = 0;
                reset();
                break;
            case GLFW_KEY_E:
                num_of_cyls++;
                lastLinkIndex = num_of_cyls - 1;
                firstLinkIndex = 0;
                reset();
                break;
            case GLFW_KEY_Q:
                if(num_of_cyls > 1) {
                    num_of_cyls--;
                    lastLinkIndex = num_of_cyls - 1;
                    firstLinkIndex = 0;
                    reset();
                } else{
                    std::cout<< "num of cycles must be greater then 0 \n" << std::endl;
                }
                break;
            case GLFW_KEY_S:
                camera->TranslateInSystem(system, {0, -0.1f, 0});
                break;
            case GLFW_KEY_A:
                camera->TranslateInSystem(system, {-0.1f, 0, 0});
                break;
            case GLFW_KEY_B:
                camera->TranslateInSystem(system, {0, 0, 0.1f});
                break;
            case GLFW_KEY_F:
                camera->TranslateInSystem(system, {0, 0, -0.1f});
                break;
            case GLFW_KEY_M:
                addCyl = true;
                break;
            case GLFW_KEY_1:
                if( pickedIndex > 0)
                  pickedIndex--;
                break;
            case GLFW_KEY_2:
                if(pickedIndex < cyls.size()-1)
                    pickedIndex++;
                break;
            case GLFW_KEY_3:
                if( tipIndex >= 0)
                {
                  if(tipIndex == cyls.size())
                    tipIndex--;
                  sphere1->Translate(GetSpherePos());
                  tipIndex--;
                }
                break;
            case GLFW_KEY_4:
                if(tipIndex < cyls.size())
                {
                    if(tipIndex < 0)
                      tipIndex++;
                    sphere1->Translate(GetSpherePos());
                    tipIndex++;
                }
                break;
        }
    }
}

Eigen::Vector3f BasicScene::GetSpherePos()
{
      Eigen::Vector3f l = Eigen::Vector3f(0.8f,0,0);
      Eigen::Vector3f res;
      res = cyls[tipIndex]->GetRotation()*l;   
      return res;  
}


void BasicScene::print_positions() {

    int currLink = firstLinkIndex;
    int counter =0;
    while (currLink != -1) {
        Eigen::Vector3f r = ikGetPosition(currLink, link_len / 2);
        printf("Tip %d position: (%f, %f , %f)\n",counter,r.data()[0],r.data()[1],r.data()[2]);
        currLink = children[currLink];
        counter++;
    }
}
//secne = cyls[id]
Eigen::Matrix3f BasicScene::create_new_Rotation_q(std::shared_ptr<Model> scene, int xyz, float add_angle) {
    Eigen::Matrix3f rot_from_euler_t;
    Eigen::Matrix3f Ri = root->GetRotation().transpose()*scene->GetRotation();
    Eigen::Vector3f euler_t = Ri.eulerAngles(2, 0, 2);
    Eigen::AngleAxis<float>::Scalar z1 = euler_t(0);
    Eigen::AngleAxis<float>::Scalar x = euler_t(1);
    Eigen::AngleAxis<float>::Scalar z2 = euler_t(2);
    if (xyz == 1){
        z1 = z1 + add_angle;
    } else if (xyz == 2){
        x = x + add_angle;
    } else{
        z2 = z2 + add_angle;
    }
    Eigen::AngleAxisf Y(z1, Eigen::Vector3f::UnitZ());
    Eigen::AngleAxisf P(x, Eigen::Vector3f::UnitX());
    Eigen::AngleAxisf R(z2, Eigen::Vector3f::UnitZ());
    Eigen::Matrix3f R_new = Eigen::Quaternionf(Y*P*R).toRotationMatrix();
    return Ri.transpose()*R_new;
}




void BasicScene::print_rotation() {

    if (pickedModel) {
        Eigen::Matrix3f rot_from_euler_t;
        Eigen::Matrix3f Ri = pickedModel->GetRotation();
        Eigen::Vector3f euler_t = Ri.eulerAngles(2, 0, 2);
        Eigen::AngleAxis<float>::Scalar z1 = euler_t(0);
        Eigen::AngleAxis<float>::Scalar x = euler_t(1);
        Eigen::AngleAxis<float>::Scalar z2 = euler_t(2);
        std::cout<< "\nz: " << z1*(180/M_PI) << std::endl;
        std::cout<< "\nx: " << x*(180/M_PI) << std::endl;
        std::cout<< "\nz: " << z2*(180/M_PI) << "\n"<< std::endl;

        Eigen::Matrix3f A0 =  Eigen::AngleAxisf(z1, Eigen::Vector3f::UnitZ()).toRotationMatrix();
        Eigen::Matrix3f A1 =  Eigen::AngleAxisf(x, Eigen::Vector3f::UnitX()).toRotationMatrix();
        Eigen::Matrix3f A2 = Eigen::AngleAxisf (z2, Eigen::Vector3f::UnitZ()).toRotationMatrix();
        std::cout<< "A0:\n" << A0 << std::endl;
        std::cout<< "\nA1:\n" << A1 << std::endl;
        std::cout<< "\nA2:\n" << A2 << std::endl;

    }
    else{
        Eigen::Matrix3f R = root->GetRotation();
        printf("Rotation matrix for the system is:\n"
               "%f ,    %f ,    %f\n%f ,    %f ,    %f\n%f ,    %f ,    %f\n",
               R.data()[0], R.data()[1], R.data()[2], R.data()[3], R.data()[4], R.data()[5],
               R.data()[6], R.data()[7], R.data()[8]);
    }
}

Eigen::Matrix3f
BasicScene::get_euler_rotate_matrix(std::shared_ptr<cg3d::Model> currModel, float phi, float theta, float psi) {
    Eigen::Matrix3f rot_from_euler_t;
    Eigen::Matrix3f Ri = root->GetRotation().transpose()*currModel->GetRotation();
    Eigen::Vector3f euler_t = Ri.eulerAngles(2, 0, 2);
    Eigen::AngleAxis<float>::Scalar z1 = euler_t(0);
    Eigen::AngleAxis<float>::Scalar x = euler_t(1);
    Eigen::AngleAxis<float>::Scalar z2 = euler_t(2);

    Eigen::AngleAxisf Y(z1+phi, Eigen::Vector3f::UnitZ());
    Eigen::AngleAxisf P(x+theta, Eigen::Vector3f::UnitX());
    Eigen::AngleAxisf R(z2+psi, Eigen::Vector3f::UnitZ());
    Eigen::Matrix3f R_new = Eigen::Quaternionf(Y*P*R).toRotationMatrix();
    return Ri.transpose()*R_new;
}

void BasicScene::reset() {
    shouldAnimateFabrik= false;
    shouldAnimateCCD = false;
    decent = true;
    angle = 0.3f;
    parents.clear();
    children.clear();
    root->RemoveChild(cyls[firstLinkIndex]);
    root->RemoveChild(axis[0]);
    cyls.clear();
    axis.clear();
    pickedModel = NULL;
    auto program = std::make_shared<Program>("shaders/phongShader");
    auto program1 = std::make_shared<Program>("shaders/pickingShader");
    auto material{ std::make_shared<Material>("material", program)}; // empty material
    auto material1{ std::make_shared<Material>("material", program1)}; // empty material
    material->AddTexture(0, "textures/box0.bmp", 2);
    auto cylMesh {IglLoader::MeshFromFiles("cyl_igl","data/zcylinder.obj")};

    float scaleFactor = 1;
    //Axis
    Eigen::MatrixXd vertices(6,3);
    vertices << -1,0,0,1,0,0,0,-1,0,0,1,0,0,0,-1,0,0,1;
    Eigen::MatrixXi faces(3,2);
    faces << 0,1,2,3,4,5;
    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(6,3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(6,2);
    auto coordsys = std::make_shared<Mesh>("coordsys",vertices,faces,vertexNormals,textureCoords);
    axis.push_back(Model::Create("axis",coordsys,material1));
    axis[0]->mode = 1;
    axis[0]->Scale(4,Axis::XYZ);
    auto system = camera->GetRotation().transpose();

    root->AddChild(axis[0]);
    parents = *new std::vector<int>(1);
    children = *new std::vector<int>(1);
    //set parents and children vectors to number of cylinders.

    parents.resize(num_of_cyls);
    children.resize(num_of_cyls);
    cyls.push_back( Model::Create("cyl",cylMesh, material));
    cyls[firstLinkIndex]->Scale(scaleFactor,Axis::Z);
    cyls[firstLinkIndex]->SetCenter(Eigen::Vector3f(0,0,-0.8f*scaleFactor));
    root->AddChild(cyls[firstLinkIndex]);

    for(int i = 1;i < num_of_cyls; i++)
    {
        cyls.push_back( Model::Create("cyl", cylMesh, material));
        cyls[i]->Scale(scaleFactor,Axis::Z);
        cyls[i]->Translate(1.6f*scaleFactor,Axis::Z);
        cyls[i]->SetCenter(Eigen::Vector3f(0,0,-0.8f*scaleFactor));
        cyls[i-1]->AddChild(cyls[i]);
        axis.push_back(Model::Create("axis",coordsys,material1));
        axis[i]->mode = 1;
        axis[i]->Scale(4,Axis::XYZ);
        axis[i]->Translate(0.8f*scaleFactor,Axis::Z);
        cyls[i-1]->AddChild(axis[i]);
    }

    for (int i =0 ; i < num_of_cyls; i++ ){
        //set parent/children values in vectors
        if (i == 0) {
            parents[i] = -1;
        }
        else {
            parents[i] = i - 1;
        }//set parent
        if (i  < lastLinkIndex) {
            children[i] = i + 1;
        }
        else
            children[i] = -1;
    }
    cyls[firstLinkIndex]->Translate({0,0, 0.8f*scaleFactor});
    link_len = cyls[firstLinkIndex]->GetMeshList()[0]->data[cyls[firstLinkIndex]->meshIndex].vertices.colwise().maxCoeff()[2]*2;
    root->Translate(Eigen::Vector3f(0,0,0));
}



