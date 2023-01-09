#include "Solution.h"
#include <AABB.h>
#include "ObjLoader.h"
#include "IglMeshLoader.h"
#include "igl/read_triangle_mesh.cpp"
#include "igl/edge_flaps.h"
#include "igl/per_vertex_normals.h"

// #include "AutoMorphingModel.h"

using namespace cg3d;

void Solution::Init(float fov, int width, int height, float near, float far)
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

 
    auto program = std::make_shared<Program>("shaders/basicShader");
    material = { std::make_shared<Material>("material", program)}; // empty material
//    SetNamedObject(cube, Model::Create, Mesh::Cube(), material, shared_from_this());
 
    material->AddTexture(0, "textures/box0.bmp", 2);
    auto sphereMesh1{IglLoader::MeshFromFiles("bunny_igl", "data/bunny.off")};
    //auto sphereMesh1{IglLoader::MeshFromFiles("sphere_igl","data/sphere.obj")};

    auto sphereMesh2{IglLoader::MeshFromFiles("bunny_igl", "data/bunny.off")};

    sphere1 = Model::Create( "sphere1",sphereMesh1, material);
    sphere2 = Model::Create( "sphere2",sphereMesh2, material);

    scaleMesh1 = 20;
    scaleMesh2 = 20;
    sphere1->Scale(scaleMesh1);
    sphere2->Scale(scaleMesh2);






    is_intersec = false;





    camera->Translate(20, Axis::Z);
    root->AddChild(sphere1);
    root->AddChild(sphere2);
    sphere1->showWireframe = true;
    sphere2->showWireframe = true;
    sphere1->showFaces = false;
    sphere2->showFaces = false;





    //root->AddChild(cube);
    
    auto mesh1 = sphere1->GetMeshList();
    auto mesh2 = sphere2->GetMeshList();


    //init the trees
    treeA1.init(mesh1[0]->data[0].vertices,mesh1[0]->data[0].faces);
    treeB1.init(mesh2[0]->data[0].vertices,mesh2[0]->data[0].faces);


    //show the big box
    setCube(sphere1,treeA1.m_box, true, false, false);
    setCube(sphere2,treeB1.m_box, true, false, false);



    //then we can translate
    sphere1->Translate({-2,-2,0});
    sphere2->Translate({2,-2,0});

}

void Solution::Update(const Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model)
{
    Scene::Update(program, proj, view, model);
    program.SetUniform4f("lightColor", 1.0f, 1.0f, 1.0f, 0.5f);
    program.SetUniform4f("Kai", 1.0f, 1.0f, 1.0f, 1.0f);

    if(!is_intersec && !isMeshCollision(sphere1, sphere2, &treeA1, &treeB1)){
        if(only_move){
            sphere1->Translate(movement_speed,Axis::X);
        }
        if(a_key){
            sphere1->Translate(-movement_speed,Axis::X);
            a_key = false;
        }
        if(d_key){
            sphere1->Translate(movement_speed,Axis::X);
            d_key = false;
        }
        if(s_key){
            sphere1->Translate(-movement_speed,Axis::Y);
            s_key = false;
        }
        if(w_key){
            sphere1->Translate(movement_speed,Axis::Y);
            w_key = false;
        }
        if(left_key){
            sphere2->Translate(-movement_speed,Axis::X);
            left_key = false;
        }
        if(right_key){
            sphere2->Translate(movement_speed,Axis::X);
            right_key = false;
        }
        if(up_key){
            sphere2->Translate(movement_speed,Axis::Y);
            up_key = false;
        }
        if(down_key){
            sphere2->Translate(-movement_speed,Axis::Y);
            down_key = false;
        }
        //sphere2->Translate(-0.001,Axis::X);

    }else{
        is_intersec = true;
    }


}
bool Solution::isMeshCollision(auto mesh1, auto mesh2, igl::AABB<Eigen::MatrixXd, 3>* treeA, igl::AABB<Eigen::MatrixXd, 3>* treeB){
    //base cases
    if (treeA == nullptr || treeB == nullptr)
        return false;

    if (!isBoxesIntersect(treeA->m_box, treeB->m_box)) {
        return false;
    }
if (treeA->is_leaf() && treeB->is_leaf()) {
    //if the smallest box intersect so draw the boxes on each mesh
        std::cout << "collapse" << std::endl;
        setCube(mesh1 ,treeA->m_box,true,true,true);
        setCube(mesh2 ,treeB->m_box,true,true,true);
        return true;
    }

    if (treeA->is_leaf() && !treeB->is_leaf()) {

        return isMeshCollision(mesh1, mesh2, treeA, treeB->m_right) ||
               isMeshCollision(mesh1, mesh2, treeA, treeB->m_left);
    }
    if (!treeA->is_leaf() && treeB->is_leaf()) {
        return isMeshCollision(mesh1, mesh2, treeA->m_right, treeB) ||
               isMeshCollision(mesh1, mesh2, treeA->m_left, treeB);
    }
    return isMeshCollision(mesh1, mesh2, treeA->m_left, treeB->m_left) |
           isMeshCollision(mesh1, mesh2, treeA->m_left, treeB->m_right) |
           isMeshCollision(mesh1, mesh2, treeA->m_right, treeB->m_left) |
           isMeshCollision(mesh1, mesh2, treeA->m_right, treeB->m_right);
}

bool Solution::isBoxesIntersect(Eigen::AlignedBox<double, 3>& boxA, Eigen::AlignedBox<double, 3>& boxB){
    // matrix A
    Eigen::Matrix3d A = sphere1->GetRotation().cast<double>();
    Eigen::Vector3d A0 = A.col(0);
    Eigen::Vector3d A1 = A.col(1);
    Eigen::Vector3d A2 = A.col(2);

    // matrix B
    Eigen::Matrix3d B = sphere2->GetRotation().cast<double>();
    Eigen::Vector3d B0 = B.col(0);
    Eigen::Vector3d B1 = B.col(1);
    Eigen::Vector3d B2 = B.col(2);


    //C=A^T*B
    //calculate C so that: C_ij = A_iXB_j
    Eigen::Matrix3d C = A.transpose() * B;

    //length sides of bounding box
    Eigen::Vector3d a = boxA.sizes() * scaleMesh1;
    Eigen::Vector3d b = boxB.sizes() * scaleMesh2;

    a = a / 2;
    b = b / 2;
    //create matrix D
    Eigen::Vector4d CA = Eigen::Vector4d(boxA.center()[0], boxA.center()[1], boxA.center()[2], 1);
    Eigen::Vector4d CB = Eigen::Vector4d(boxB.center()[0], boxB.center()[1], boxB.center()[2], 1);
    Eigen::Vector4d DOfVector4d = sphere2->GetTransform().cast<double>() * CB - sphere1->GetTransform().cast<double>() * CA;
    Eigen::Vector3d D = DOfVector4d.head(3);
//    Eigen::Vector3d CA = Eigen::Vector3d(boxA.center()[0], boxA.center()[1], boxA.center()[2]);
//    Eigen::Vector3d CB = Eigen::Vector3d(boxB.center()[0], boxB.center()[1], boxB.center()[2]);
//    Eigen::Vector3d D = sphere2->Tout.cast<double>() * CB - sphere1->Tout.cast<double>()*CA;


    double left, right, R;


    //check the 15 conditions for R > R_0 + R_1

    //A conditions:

    //A_0
    return !(a(0) + (b(0) * abs(C.row(0)(0)) + b(1) * abs(C.row(0)(1)) + b(2) * abs(C.row(0)(2))) < abs(A0.transpose() * D)
        ||
    //A_1
        (a(1) + (b(0) * abs(C.row(1)(0)) + b(1) * abs(C.row(1)(1)) + b(2) * abs(C.row(1)(2))) < abs(A1.transpose() * D))
        ||
    //A_2
        (a(2) + (b(0) * abs(C.row(2)(0)) + b(1) * abs(C.row(2)(1)) + b(2) * abs(C.row(2)(2))) < abs(A2.transpose() * D))
        ||

    //B conditions:

    //B_0
        (b(0) + (a(0) * abs(C.row(0)(0)) + a(1) * abs(C.row(1)(0)) + a(2) * abs(C.row(2)(0))) < abs(B0.transpose() * D))
        ||
    //B_1
        (b(1) + (a(0) * abs(C.row(0)(1)) + a(1) * abs(C.row(1)(1)) + a(2) * abs(C.row(2)(1))) < abs(B1.transpose() * D))
        ||
    //B_2
        (b(2) + (a(0) * abs(C.row(0)(2)) + a(1) * abs(C.row(1)(2)) + a(2) * abs(C.row(2)(2))) < abs(B2.transpose() * D))

        ||

    //A_0XB_0
    (left = C.row(1)(0) * A2.transpose() * D,
            right = C.row(2)(0) * A1.transpose() * D,
    R = left-right,
     (a(1) * abs(C.row(2)(0)) + a(2) * abs(C.row(1)(0)) + b(1) * abs(C.row(0)(2))+ b(2) * abs(C.row(0)(1)) < abs(R)))

        ||

    //A_0XB_1
            (left = C.row(1)(1) * A2.transpose() * D,
    right = C.row(2)(1) * A1.transpose() * D,
    R = left-right,
    (a(1) * abs(C.row(2)(1)) + a(2) * abs(C.row(1)(1)) + b(0) * abs(C.row(0)(2)) + b(2) * abs(C.row(0)(0)) < abs(R)))

        ||

    //A_0XB_2
    (left = C.row(1)(2) * A2.transpose() * D,
    right = C.row(2)(2) * A1.transpose() * D,
    R = left-right,
    (a(1) * abs(C.row(2)(2)) + a(2) * abs(C.row(1)(2)) + b(0) * abs(C.row(0)(1)) + b(1) * abs(C.row(0)(0)) < abs(R)))

        ||

    //A_1XB_0

    (left = C.row(2)(0) * A0.transpose() * D,
    right = C.row(0)(0) * A2.transpose() * D,
    R = left-right,
    (a(0) * abs(C.row(2)(0)) + a(2) * abs(C.row(0)(0)) + b(1) * abs(C.row(1)(2)) + b(2) * abs(C.row(1)(1)) < abs(R)))

        ||

    //A_1XB_1

                (left = C.row(2)(1) * A0.transpose() * D,
    right = C.row(0)(1) * A2.transpose() * D,
    R = left-right,
    (a(0) * abs(C.row(2)(1)) + a(2) * abs(C.row(0)(1)) + b(0) * abs(C.row(1)(2)) + b(2) * abs(C.row(1)(0)) < abs(R)))

        ||

    //A_1XB_2


    (left = C.row(2)(2) * A0.transpose() * D,
    right = C.row(0)(2) * A2.transpose() * D,
    R = left-right,
    (a(0) * abs(C.row(2)(2)) + a(2) * abs(C.row(0)(2)) + b(0) * abs(C.row(1)(1)) + b(1) * abs(C.row(1)(0)) < abs(R)))

        ||
    //A2 conditions:


    //A_2XB_0

    (left = C.row(0)(0) * A1.transpose() * D,
    right = C.row(1)(0) * A0.transpose() * D,
    R = left-right,
    (a(0) * abs(C.row(1)(0)) + a(1) * abs(C.row(0)(0)) + b(1) * abs(C.row(2)(2)) + b(2) * abs(C.row(2)(1)) < abs(R)))

        ||

    //A_2XB_1
    (left = C.row(0)(1) * A1.transpose() * D,
    right= C.row(1)(1) * A0.transpose() * D,
    R = left-right,
    (a(0) * abs(C.row(1)(1)) + a(1) * abs(C.row(0)(1)) + b(0) * abs(C.row(2)(2)) + b(2) * abs(C.row(2)(0)) < abs(R)))

        ||
    //A_2XB_2
    (left = C.row(0)(2) * A1.transpose() * D,
    right = C.row(1)(2) * A0.transpose() * D,
    R = left-right,
    (a(0) * abs(C.row(1)(2)) + a(1) * abs(C.row(0)(2)) + b(0) * abs(C.row(2)(1)) + b(1) * abs(C.row(2)(0)) < abs(R)))
    );
}
MeshData Solution::createDataFromBox(Eigen::AlignedBox<double, 3>& box){
    Eigen::MatrixXd V, NV, T;
    Eigen::MatrixXi F;

    V.resize(8,3);
    F.resize(12,3);

    Eigen::RowVector3d BottomRightCeil = box.corner(box.BottomRightCeil);
    Eigen::RowVector3d BottomRightFloor = box.corner(box.BottomRightFloor);
    Eigen::RowVector3d BottomLeftCeil = box.corner(box.BottomLeftCeil);
    Eigen::RowVector3d BottomLeftFloor = box.corner(box.BottomLeftFloor);
    Eigen::RowVector3d TopRightCeil = box.corner(box.TopRightCeil);
    Eigen::RowVector3d TopRightFloor = box.corner(box.TopRightFloor);
    Eigen::RowVector3d TopLeftCeil = box.corner(box.TopLeftCeil);
    Eigen::RowVector3d TopLeftFloor = box.corner(box.TopLeftFloor);


    V.row(0) = BottomLeftFloor;
    V.row(1) = BottomRightFloor;
    V.row(2) = TopLeftFloor;
    V.row(3) = TopRightFloor;
    V.row(4) = BottomLeftCeil;
    V.row(5) = BottomRightCeil;
    V.row(6) = TopLeftCeil;
    V.row(7) = TopRightCeil;

    F.row(0) = Eigen::Vector3i(0,1,3);
    F.row(1) = Eigen::Vector3i(3,2,0);
    F.row(2) = Eigen::Vector3i(4,5,7);
    F.row(3) = Eigen::Vector3i(7,6,4);
    F.row(4) = Eigen::Vector3i(0,4,6);
    F.row(5) = Eigen::Vector3i(6,2,0);
    F.row(6) = Eigen::Vector3i(5,7,3);
    F.row(7) = Eigen::Vector3i(7,3,1);
    F.row(8) = Eigen::Vector3i(2,6,7);
    F.row(9) = Eigen::Vector3i(7,3,2);
    F.row(10) = Eigen::Vector3i(4,5,1);
    F.row(11) = Eigen::Vector3i(1,0,4);


    igl::per_vertex_normals(V,F,NV);
    T = Eigen::MatrixXd::Zero(V.rows(),2);

    return {V,F,NV,T};
}

std::shared_ptr<Model> Solution::createBox(Eigen::AlignedBox<double, 3>& box){

    std::vector<std::shared_ptr<Mesh>> meshList;
    std::vector<MeshData> new_data;
    new_data.push_back(createDataFromBox(box));
    auto m = std::make_shared<Mesh>("cube",new_data);
    meshList.push_back(m);
    return Model::Create( "cube",meshList, material);
}
//show the cube in the mesh
void Solution::setCube(auto mesh, Eigen::AlignedBox<double, 3>& box, bool showWireframe, bool showFaces, bool showTextures){
    auto cube = cube1 = createBox(box);
    cube -> aggregatedTransform = mesh->aggregatedTransform;
    mesh->AddChild(cube);
    cube->showWireframe = showWireframe;
    cube->showFaces = showTextures;
    cube->showTextures = showTextures;
}

void Solution::KeyCallback(cg3d::Viewport *_viewport, int x, int y, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT){
        if(key == GLFW_KEY_0){
            only_move = !only_move;
        }
        if(key == GLFW_KEY_I){
            movement_speed = movement_speed * 2;
        }
        if(key == GLFW_KEY_L){
            movement_speed = movement_speed / 2;
        }
        if(key == GLFW_KEY_W){
            w_key = true;
        }
        if(key == GLFW_KEY_A){
            a_key = true;
        }
        if(key == GLFW_KEY_S){
            s_key = true;
        }
        if(key == GLFW_KEY_D){
            d_key = true;
        }if(key == GLFW_KEY_RIGHT){
            right_key = true;
        }if(key == GLFW_KEY_DOWN){
            down_key = true;
        }if(key == GLFW_KEY_LEFT){
            left_key = true;
        }if(key == GLFW_KEY_UP){
            up_key = true;
        }
    }
}

//void KeyCallback(cg3d::Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods) {
//    if (action == GLFW_PRESS || action == GLFW_REPEAT){
//        if(key == GLFW_KEY_LEFT){
//            int i = 1;
//              sphere1->Translate(0.001,Axis::X);
//        }
//    }
//}
