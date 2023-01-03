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
    auto program = std::make_shared<Program>("shaders/phongShader");
    auto program1 = std::make_shared<Program>("shaders/pickingShader");
    auto material{ std::make_shared<Material>("material", program)}; // empty material
    auto material1{ std::make_shared<Material>("material", program1)}; // empty material
//    SetNamedObject(cube, Model::Create, Mesh::Cube(), material, shared_from_this());
    material->AddTexture(0, "textures/box0.bmp", 2);
    auto sphereMesh{IglLoader::MeshFromFiles("sphere_igl", "data/sphere.obj")};
    auto cylMesh{IglLoader::MeshFromFiles("cyl_igl","data/zcylinder.obj")};
    auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","data/cube_old.obj")};
    sphere1 = Model::Create( "sphere",sphereMesh, material);    
    cube = Model::Create( "cube", cubeMesh, material);
    decent = true;

    //Axis
    Eigen::MatrixXd vertices(6,3);
    vertices << -1,0,0,1,0,0,0,-1,0,0,1,0,0,0,-1,0,0,1;
    Eigen::MatrixXi faces(3,2);
    faces << 0,1,2,3,4,5;
    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(6,3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(6,2);
    std::shared_ptr<Mesh> coordsys = std::make_shared<Mesh>("coordsys",vertices,faces,vertexNormals,textureCoords);
    axis.push_back(Model::Create("axis",coordsys,material1));
    axis[0]->mode = 1;   
    axis[0]->Scale(4,Axis::XYZ);
    // axis[0]->lineWidth = 5;
    root->AddChild(axis[0]);
    float scaleFactor = 1;
    parents = *new std::vector<int>(1);
    children = *new std::vector<int>(1);
    //set parents and children vectors to number of cylinders.

    parents.resize(num_of_cyls);
    children.resize(num_of_cyls);
    cyls.push_back( Model::Create("cyl",cylMesh, material));
    cyls[0]->Scale(scaleFactor,Axis::Z);
    cyls[0]->SetCenter(Eigen::Vector3f(0,0,-0.8f*scaleFactor));
    root->AddChild(cyls[0]);
   
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
    //cyls[0]->Rotate(3.141592654/2, Axis::Z);
    for (int i =0 ; i < num_of_cyls; i++ ){
        //set parent/children values in vectors
        if (i == 0)
            parents[i] = -1;
        else
            parents[i] = i - 1; //set parent
        if (i  < lastLinkIndex) {
            children[i] = i + 1;
        }

        else
            children[i] = -1;
    }
    cyls[0]->Translate({0,0,0.8f*scaleFactor});

    auto morphFunc = [](Model* model, cg3d::Visitor* visitor) {
      return model->meshIndex;//(model->GetMeshList())[0]->data.size()-1;
    };
    autoCube = AutoMorphingModel::Create(*cube, morphFunc);

  
    sphere1->showWireframe = true;
    autoCube->Translate({-6,0,0});
    autoCube->Scale(1.5f);
    sphere1->Translate({3,1,0});
    link_len = cyls[2]->GetMeshList()[0]->data[cyls[2]->meshIndex].vertices.colwise().maxCoeff()[2]*2;
    autoCube->showWireframe = true;
    camera->Translate(22, Axis::Z);
    root->AddChild(sphere1);
    root->AddChild(autoCube);
    // points = Eigen::MatrixXd::Ones(1,3);
    // edges = Eigen::MatrixXd::Ones(1,3);
    // colors = Eigen::MatrixXd::Ones(1,3);
    
    // cyl->AddOverlay({points,edges,colors},true);
    cube->mode =1;
    auto mesh = cube->GetMeshList();

    //autoCube->AddOverlay(points,edges,colors);
    // mesh[0]->data.push_back({V,F,V,E});
    int num_collapsed;

  // Function to reset original mesh and data structures
    V = mesh[0]->data[0].vertices;
    F = mesh[0]->data[0].faces;
   // igl::read_triangle_mesh("data/cube.off",V,F);
    igl::edge_flaps(F,E,EMAP,EF,EI);
    std::cout<< "vertices: \n" << V <<std::endl;
    std::cout<< "faces: \n" << F <<std::endl;
    
    std::cout<< "edges: \n" << E.transpose() <<std::endl;
    std::cout<< "edges to faces: \n" << EF.transpose() <<std::endl;
    std::cout<< "faces to edges: \n "<< EMAP.transpose()<<std::endl;
    std::cout<< "edges indices: \n" << EI.transpose() <<std::endl;

}

void BasicScene::IKFabric(){
    {
        if (animate&&shouldAnimateFabrik) {
            std::cout<< "in Fabrik \n" << std::endl;
            std::vector<Eigen::Vector3f> p; //joint positions
            p.resize(num_of_cyls + 1 );
            Eigen::Vector3f target = sphere1->GetAggregatedTransform().col(3).head(3);
            Eigen::Vector3f p1 = ikGetPosition(firstLinkIndex, -link_len/2);

            //Set disjoint positions
            //p1 is first disjoint
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
                //1.10 : Target Is Reachable
                //the target is reachable; thus set as b the initial position of joint p0
                Eigen::Vector3f b = p[firstLinkIndex];
                //Check weather the distance between the end effector Pn and the
                //target is greater than a tolerance.
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
                        //	1.19 Stage1 Forward Reaching
                        p[lastLinkIndex + 1] = target;
                        int parent = lastLinkIndex;
                        int child = lastLinkIndex + 1;
                        while (parent != -1) {

                            // Find the distance ri between the new joint position pi+1 and the joint pi
                            //steps 1.24-1.25
                            ris_Array[parent] = (p[child] - p[parent]).norm();
                            lambdaI_Array[parent] = link_len / ris_Array[parent];

                            //find the new joint position. step 1.27
                            p[parent] = (1 - lambdaI_Array[parent]) * p[child] + lambdaI_Array[parent] * p[parent];
                            child = parent;
                            parent = parents[parent];

                        }

                        //Stage 2: Backward reaching (1.29)
                        //Set the p1 p0 its initial position*/
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
                        ikSolverHelper(currLink, p[target_id]);
                        currLink = target_id;
                        target_id = children[target_id];
                    }
                    ikSolverHelper(lastLinkIndex, p[lastLinkIndex + 1]);
                    double distance = (target - ikGetPosition(lastLinkIndex, link_len/2)).norm();
                    if (distance < tolerance) {
                        fix_rotate();
                        shouldAnimateFabrik = false;
                        std::cout << "distance: " << distance << std::endl;

                    }
                }
            }
            animate = false;
        }
    }
}


void BasicScene::ikSolverHelper(int id, const Eigen::Vector3f& t){
    Eigen::Vector3f r = ikGetPosition(id, -link_len/2);
    Eigen::Vector3f e = ikGetPosition(id, link_len/2);
    Eigen::Vector3f rd = t - r;
    Eigen::Vector3f re = e - r;
    Eigen::Vector3f normal = re.normalized().cross(rd.normalized());
    float dot = rd.normalized().dot(re.normalized());//get dot

    //Eigen::Quaternion q = Eigen::Quaternionf::FromTwoVectors(re,rd);
    //q = q.slerp(0.9f, Eigen::Quaternionf::Identity());
    if (dot > 1)
        dot = 1;
    if (dot < -1)
        dot = 1;
    float angle = (float)acos(dot)/10 ;
    Eigen::Vector3f rotationVec = (cyls[id]->GetAggregatedTransform()).block<3, 3>(0, 0).inverse() * normal;
    int parent = parents[id];

    cyls[id]->Rotate(angle, rotationVec);
}

void BasicScene::IKCoordinateDecent(){
    if (animate && shouldAnimateCCD) {
        Eigen::Vector3f target_des = sphere1->GetAggregatedTransform().col(3).head(3);
        Eigen::Vector3f first_link_pos = ikGetPosition(firstLinkIndex, -link_len/2);
        std::cout<< "edges: \n" << link_len <<std::endl;
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

            //Eigen::Quaternion q = Eigen::Quaternionf::FromTwoVectors(re,rd);
            //q = q.slerp(0.9f, Eigen::Quaternionf::Identity());


            Eigen::Vector3f normal = re.normalized().cross(rd.normalized());//returns the plane normal
            double distance = (target_des - e).norm();
            if (distance < 0.05) {
                std::cout << "distance: " << distance << std::endl;
                fix_rotate();
                shouldAnimateCCD = false;
                return;
            }
            float dot = rd.normalized().dot(re.normalized());
            //check that it is beetween -1 to 1
            if (dot > 1)
                dot = 1;
            if (dot < -1)
                dot = -1;
            double angle = acosf(dot) / 100;
            Eigen::Vector3f rotationVec = cyls[currLink]->GetAggregatedTransform().block<3, 3>(0, 0).inverse() * normal;
            int parent = parents[currLink];
            cyls[currLink]->Rotate(angle,rotationVec);
            currLink = parents[currLink];
        }
        animate = false;
    }
}

void BasicScene::fix_rotate(){
    Eigen::Vector3f Z(0, 0, 1);
    int currLink = firstLinkIndex;
    while (currLink != -1) {
        Eigen::Matrix3f R =cyls[currLink]->GetRotation();
        Eigen::Vector3f ea = R.eulerAngles(2, 0, 2);//get the rotation angles
        float angleZ = ea[2];
       cyls[currLink]->Rotate( -angleZ,Axis::Z);
        currLink = children[currLink];
        if (currLink != -1) {
            auto system = camera->GetRotation().transpose();
            cyls[currLink]->RotateInSystem(system, angleZ, Axis::Z);
        }
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
        pickedModel->TranslateInSystem(system, {0, 0, -float(yoffset)});
        pickedToutAtPress = pickedModel->GetTout();
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
            if (buttonState[GLFW_MOUSE_BUTTON_RIGHT] != GLFW_RELEASE)
                pickedModel->TranslateInSystem(system, {-float(xAtPress - x) / moveCoeff, float(yAtPress - y) / moveCoeff, 0});
            if (buttonState[GLFW_MOUSE_BUTTON_MIDDLE] != GLFW_RELEASE)
                pickedModel->RotateInSystem(system, float(xAtPress - x) / angleCoeff, Axis::Z);
            if (buttonState[GLFW_MOUSE_BUTTON_LEFT] != GLFW_RELEASE) {
                pickedModel->RotateInSystem(system, float(xAtPress - x) / angleCoeff, Axis::Y);
                pickedModel->RotateInSystem(system, float(yAtPress - y) / angleCoeff, Axis::X);
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
                    Eigen::Vector3f acEuler = cyls[pickedIndex]->GetRotation().eulerAngles(2,0,2);
                    Eigen::Matrix3f Rnew = create_new_Rotation(acEuler.data()[0],acEuler.data()[1]+angle,acEuler.data()[2]);
                    pickedModel->Rotate( Rnew * cyls[pickedIndex]->GetRotation().transpose() );
                } else{

                    root->RotateInSystem(system, angle, Axis::X);
                }
                break;
            case GLFW_KEY_DOWN:
                if (pickedModel) {
                    Eigen::Vector3f acEuler = cyls[pickedIndex]->GetRotation().eulerAngles(2,0,2);
                    Eigen::Matrix3f Rnew = create_new_Rotation(acEuler.data()[0],acEuler.data()[1]-angle,acEuler.data()[2]);
                    pickedModel->Rotate( Rnew * cyls[pickedIndex]->GetRotation().transpose() );
                } else{
                    root->RotateInSystem(system, -angle, Axis::X);
                }
                break;
            case GLFW_KEY_LEFT:
                if (pickedModel) {
                    Eigen::Vector3f acEuler = cyls[pickedIndex]->GetRotation().eulerAngles(2,0,2);
                    Eigen::Matrix3f Rnew = create_new_Rotation(acEuler.data()[0],acEuler.data()[1],acEuler.data()[2]+angle);
                    pickedModel->Rotate(  Rnew * cyls[pickedIndex]->GetRotation().transpose());
                } else{
                    root->RotateInSystem(system, angle, Axis::Y);
                }
                break;
            case GLFW_KEY_RIGHT:
                if (pickedModel) {
                    Eigen::Vector3f acEuler = cyls[pickedIndex]->GetRotation().eulerAngles(2,0,2);
                    Eigen::Matrix3f Rnew = create_new_Rotation(acEuler.data()[0],acEuler.data()[1],acEuler.data()[2]-angle);
                    pickedModel->Rotate(Rnew  * cyls[pickedIndex]->GetRotation().transpose() );
                } else{
                    root->RotateInSystem(system, -angle, Axis::Y);
                }
                break;
            case GLFW_KEY_T:
                camera->TranslateInSystem(system, {0, 0.1f, 0});
                print_positions();
                break;
                //TODO p
            case GLFW_KEY_P:
                print_rotation();
                camera->TranslateInSystem(system, {0, 0.1f, 0});
                break;

            case GLFW_KEY_W:
                camera->TranslateInSystem(system, {0, 0.1f, 0});
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
      Eigen::Vector3f l = Eigen::Vector3f(1.6f,0,0);
      Eigen::Vector3f res;
      res = cyls[tipIndex]->GetRotation()*l;   
      return res;  
}

void BasicScene::print_positions() {

    int currLink = lastLinkIndex;
    int counter =0;
    Eigen::Vector3f e = ikGetPosition(lastLinkIndex, link_len/2);
    printf("Tip %d position: (%f, %f , %f)\n",num_of_cyls-counter,e.data()[0],e.data()[1],e.data()[2]);
    counter++;
    while (currLink != -1) {
        Eigen::Vector3f r = ikGetPosition(currLink, -link_len / 2);
        printf("Tip %d position: (%f, %f , %f)\n",num_of_cyls-counter,r.data()[0],r.data()[1],r.data()[2]);
        currLink = parents[currLink];
        counter++;
    }
}

Eigen::Matrix3f BasicScene::create_new_Rotation(float phi, float theta, float psi){
    Eigen::Matrix3f A,B,C,D;
    A << cos(phi), -sin(phi), 0,
         sin(phi), cos(phi), 0,
         0 ,             0 ,             1;
    B<< 1, 0,                   0,
        0, cos(theta) , -sin(theta),
        0, sin(theta), cos(theta);
    C<< cos(psi), -sin(psi), 0,
        sin(psi), cos(psi),  0,
        0             ,0              ,   1;
    D= A*B*C;
    return D;
}

void BasicScene::print_rotation() {

    if (pickedModel) {
        Eigen::Vector3f acEuler = cyls[pickedIndex]->GetRotation().eulerAngles(2, 0, 2);
        Eigen::Matrix3f R = create_new_Rotation(acEuler.data()[0], acEuler.data()[1], acEuler.data()[2]);
        printf("Rotation matrix for the %d cylinder is:\n"
               "%f ,    %f ,    %f\n%f ,    %f ,    %f\n%f ,    %f ,    %f\n",
               pickedIndex, R.data()[0], R.data()[1], R.data()[2], R.data()[3], R.data()[4], R.data()[5],
               R.data()[6], R.data()[7], R.data()[8]);
    }
    else{
        Eigen::Vector3f acEuler = root->GetRotation().eulerAngles(2, 0, 2);
        Eigen::Matrix3f R = create_new_Rotation(acEuler.data()[0], acEuler.data()[1], acEuler.data()[2]);
        printf("Rotation matrix for the system is:\n"
               "%f ,    %f ,    %f\n%f ,    %f ,    %f\n%f ,    %f ,    %f\n",
               pickedIndex, R.data()[0], R.data()[1], R.data()[2], R.data()[3], R.data()[4], R.data()[5],
               R.data()[6], R.data()[7], R.data()[8]);
    }
}

