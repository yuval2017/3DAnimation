#include "BasicScene.h"
#include <utility>
#include "ObjLoader.h"
#include "IglMeshLoader.h"
#include "igl/edge_flaps.h"
#include <igl/vertex_triangle_adjacency.h>
#include "AutoMorphingModel.h"
#include <per_vertex_normals.h>
#include <per_face_normals.h>
#include "Mesh.h"

#include <igl/circulation.h>
#include <igl/collapse_edge.h>
#include <igl/parallel_for.h>
#include <Eigen/Core>
#include <iostream>
#include <set>
#include <functional>


using namespace cg3d;
using namespace std;
using namespace Eigen;
using namespace igl;

void BasicScene::Init(float fov, int width, int height, float near, float far) {
    camera = Camera::Create("camera", fov, float(width) / height, near, far);

    AddChild(root = Movable::Create("root")); // a common (invisible) parent object for all the shapes
    auto daylight{std::make_shared<Material>("daylight", "shaders/cubemapShader")};
    daylight->AddTexture(0, "textures/cubemaps/Daylight Box_", 3);
    auto background{Model::Create("background", Mesh::Cube(), daylight)};
    AddChild(background);
    background->Scale(120, Axis::XYZ);
    background->SetPickable(false);
    background->SetStatic();


    auto program = std::make_shared<Program>("shaders/basicShader");
    auto material{std::make_shared<Material>("material", program)}; // empty material
//    SetNamedObject(cube, Model::Create, Mesh::Cube(), material, shared_from_this());

    material->AddTexture(0, "textures/box0.bmp", 2);
    auto sphereMesh{IglLoader::MeshFromFiles("sphere_igl", "data/bunny.off")};
    auto sphere{Model::Create("sphere", sphereMesh, material)};
    auto morphFunc = [this](Model* model, cg3d::Visitor* visitor) {
        return Simp();
    };

    sphere1 = AutoMorphingModel::Create(*sphere, morphFunc);
    sphere1->Scale(40);
    sphere1->showWireframe = true;
    sphere1->Translate({-3, 0, 0});
    root->AddChild(sphere1);

    /*
    auto cylMesh{IglLoader::MeshFromFiles("cyl_igl","data/camel_b.obj")};
    cyl = Model::Create( "cyl", cylMesh, material);
    cyl->Translate({3,0,0});
    cyl->Scale(0.12f);
    cyl->showWireframe = true;
    root->AddChild(cyl);
    */
    /*
    auto cubeMesh{IglLoader::MeshFromFiles("cube_igl","data/cube.off")};
    cube = Model::Create( "cube", cubeMesh, material);
    cube->showWireframe = true;
    root->AddChild(cube);
     */

    curr_mesh = sphere1;
    curr_mesh_name = "sphere1";


    auto mesh = curr_mesh->GetMeshList();
    max = 200;

    // Function to reset original mesh and data structures
    V = mesh[0]->data[0].vertices;
    F = mesh[0]->data[0].faces;

    camera->Translate(20, Axis::Z);
    core().is_animating = false;

    reset();


    /*
    std::cout<< "faces: \n" << F <<std::endl;
    std::cout<< "edges: \n" << E.transpose() <<std::endl;
    std::cout<< "edges to faces: \n" << EF.transpose() <<std::endl;
    std::cout<< "faces to edges: \n "<< EMAP.transpose()<<std::endl;
    std::cout<< "edges indices: \n" << EI.transpose() <<std::endl;
     */

}

void BasicScene::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    program.SetUniform4f("lightColor", 1.0f, 1.0f, 1.0f, 0.5f);
    program.SetUniform4f("Kai", 1.0f, 1.0f, 1.0f, 1.0f);
    //sphere1->Rotate(0.01f, Axis::XYZ);
}
void BasicScene::KeyCallback(Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_LEFT){
            if(curr_collaps_index < max-1){
                curr_collaps_index++;
            }
            core().is_animating =true ;
        }
        if (key == GLFW_KEY_RIGHT){
            if(curr_collaps_index > 0){
                curr_collaps_index--;
            }
            core().is_animating =true ;
        }
        if(key == GLFW_KEY_1){
            max+=10;
        }
        if (key == GLFW_KEY_0){
            curr_collaps_index = 0;
            core().is_animating =true;
        }

    }
}


void BasicScene::reset(){

    done_collaps.resize(max);
    done_collaps[0] = true;


    igl::edge_flaps(F,E,EMAP,EF,EI);
    C.resize(E.rows(),V.cols());
    VectorXd costs(E.rows());

    num_collapsed = 0;
    curr_collaps_index = 0;


    //add this functionality
    QtIterator.resize(E.rows()); //number of edges

    //insert junk edge with  weight
    std::pair<double,int> p = std::pair<double, int>(std::numeric_limits<double>::infinity(),E.rows());
    inf = Qt.insert(p).first;
    //eval Q
    evalQ();
    Qt.clear();
    for (int j = 0; j < E.rows(); j++) {
        calculateEdgesCost(j);
    }
}


int BasicScene::Simp(){

    // If animating then collapse 10% of edges
    if(core().is_animating && !Qt.empty() && (curr_collaps_index < max && curr_collaps_index >= 0))
    {
        if(!done_collaps[curr_collaps_index]) {
            bool something_collapsed = false;
            const int max_iter = std::ceil(0.1 * Qt.size());
            for (int j = 0; j < max_iter; j++) {
                if (!collapse_edge()) {
                    break;
                }
                something_collapsed = true;
                num_collapsed++;
            }
            if (something_collapsed) {
                Eigen::MatrixXd vertexNormals;
                igl::per_vertex_normals(V,F,vertexNormals);
                set_mesh(V, F,vertexNormals);
                done_collaps[curr_collaps_index] = true;
            }
        }
        std::cout<< "index isssss: " << curr_collaps_index << "\n";
    }
    core().is_animating = false;
    return curr_collaps_index;
}

void BasicScene::evalQ() {
    std::vector<std::vector<int> > VF;// vertex to faces
    std::vector<std::vector<int> > VFi;//only for vertex_triangle_adjacency use.

    int n = V.rows();
    //queue size
    QPerVertex.resize(n);
    //create vertex to faces
    vertex_triangle_adjacency(n, F, VF, VFi);

    //faces normals calculation
    Eigen::MatrixXd F_normals;
    igl::per_face_normals(V, F, F_normals);
    Eigen::Vector3d face_normal;
    //every vertex get all faces and calculate Q
    for (int i = 0; i < n; i++) {
        //initialize with zeroes
        QPerVertex[i] = Eigen::Matrix4d::Zero();
        // calculate vertex  Q matrix
        for (int j = 0; j < VF[i].size(); j++) {
            Eigen::Vector4d P;
            //get face normal
            face_normal = F_normals.row(VF[i][j]).normalized();
            // ax+by+cz+d=0
            double a = face_normal[0];
            double b = face_normal[1];
            double c = face_normal[2];
            double d = V.row(i) * face_normal;
            d *= -1;
            P = Eigen::Vector4d(a, b, c, d);
            //init K_p = PP^T
            //Q^hat = Q = K_p
            QPerVertex[i] += P*P.transpose();;
        }

    }
}
void BasicScene:: calculateEdgesCost(int edge){
    //vertexes of the edge
    int v1 = E(edge, 0);
    int v2 = E(edge, 1);

    // compute (Q1 + Q2) = Q
    Eigen::Matrix4d QPerEdge= QPerVertex[v1] + QPerVertex[v2];


    Eigen::Matrix4d tempQ = QPerEdge; //we will use this to find v` position
    tempQ.row(3) = Eigen::Vector4d(0, 0, 0, 1);
    // v^-
    Eigen::Vector4d VHat;
    double cost;
    bool inversable;
    tempQ.computeInverseWithCheck(tempQ, inversable);
    if (inversable) {
        VHat = tempQ * (Eigen::Vector4d(0, 0, 0, 1));
        cost = VHat.transpose() * QPerEdge * VHat;
    }
    else {
        //min error from v1 v2 v1+v2/2
        Eigen::Vector4d p1;
        p1 << V.row(v1), 1;
        double cost1 = p1.transpose() * QPerEdge * p1;

        Eigen::Vector4d p2;
        p2 << V.row(v2), 1;
        double cost2 = p2.transpose() * QPerEdge * p2;

        Eigen::Vector4d p12;
        p12 << ((V.row(v1)+ V.row(v2))/2), 1;

        //threshold parameter
        double cost3 = p12.transpose() * QPerEdge * p12;
        //
        if (cost1 < cost2 && cost1 < cost3) {
            VHat = p1;
            cost = cost1;
        }
        else if (cost2 < cost1 && cost2 < cost3) {
            VHat = p2;
            cost = cost2;
        }
        else {
            VHat = p12;
            cost = cost3;
        }
    }
    //choose the position for v
    Eigen::Vector3d new_pos;
    new_pos[0] = VHat[0];
    new_pos[1] = VHat[1];
    new_pos[2] = VHat[2];
    //the new edge
    C.row(edge) = new_pos;
    //every edge the first the first one to remove
    QtIterator[edge] = Qt.insert(std::pair<double, int>(cost, edge)).first;
}
bool BasicScene::collapse_edge(){
    //for igl function
    int e1, e2, f1, f2;

    //no edges to collapse
    if (Qt.empty())
    {
        return false;
    }
    std::pair<double, int> pair = *(Qt.begin());
    if (pair.first == std::numeric_limits<double>::infinity())
    {
        return false;
    }
    //delete lowest edge in the queue
    Qt.erase(Qt.begin());
    //min age cost
    int e = pair.second;
    //two vertices of the edge
    int v1 = E.row(e)[0];
    int v2 = E.row(e)[1];

    QtIterator[e] = inf;

    //get the  list of faces around the end point the edge
    std::vector<int> N = igl::circulation(e, true, EMAP, EF, EI);
    std::vector<int> Nd = igl::circulation(e, false, EMAP, EF, EI);
    N.insert(N.begin(), Nd.begin(), Nd.end());

    //collapse the edge and return the two edges
    bool is_collapsed = igl::collapse_edge(e, C.row(e), V, F, E, EMAP, EF, EI, e1, e2, f1, f2);
    if(is_collapsed){

        // Erase the edges we need to collapse
        Qt.erase(QtIterator[e1]);
        QtIterator[e1] = inf;


        Qt.erase(QtIterator[e2]);
        QtIterator[e2] = inf;

        //update the Q matrix for the 2 veterixes we collapsed
        QPerVertex[v1] = QPerVertex[v1] + QPerVertex[v2];
        QPerVertex[v2] = QPerVertex[v1] + QPerVertex[v2];

        Eigen::VectorXd newAdgesPosition;
        // update  neighbors
        // loop over original face neighbors
        for (auto n : N)
        {
            if (F(n, 0) != IGL_COLLAPSE_EDGE_NULL ||
                F(n, 1) != IGL_COLLAPSE_EDGE_NULL ||
                F(n, 2) != IGL_COLLAPSE_EDGE_NULL)
            {
                for (int v = 0; v < 3; v++)
                {
                    //edge id
                    const  int ei = EMAP(v * F.rows() + n);
                    // erase old entry
                    Qt.erase(QtIterator[ei]);
                    // compute cost and potential placement and place in queue
                    calculateEdgesCost(ei);
                }
            }
            newAdgesPosition = C.row(e);
        }
        std::cout << "edge " << e << ",cost " << pair.first << ",new position (" << newAdgesPosition[0] << ","
                  << newAdgesPosition[1] << "," << newAdgesPosition[2] << ")" << std::endl;
    }
    else
    {
        // if not collapse it infinity we wont collps it anymore...
        pair.first = std::numeric_limits<double>::infinity();
        QtIterator[e] = Qt.insert(pair).first;
    }
    return is_collapsed;

}
void BasicScene::set_mesh(Eigen::MatrixXd OV,Eigen::MatrixXi OF,Eigen::MatrixXd vertexNormals) {
    curr_mesh->AddDataToMEsh(0,OV,OF,vertexNormals,curr_mesh->GetMeshList()[0]->data[0].textureCoords);
}
