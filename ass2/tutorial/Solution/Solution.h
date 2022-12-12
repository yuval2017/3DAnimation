#pragma once

#include "Scene.h"

#include <utility>
#include <AABB.h>

class Solution : public cg3d::Scene
{
public:
    explicit Solution(std::string name, cg3d::Display* display) : Scene(std::move(name), display) {};
    void Init(float fov, int width, int height, float near, float far);
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;
    bool isMeshCollision(auto mesh1, auto mesh2, igl::AABB<Eigen::MatrixXd, 3>* treeA, igl::AABB<Eigen::MatrixXd, 3>* treeB);
    bool isBoxesIntersect(Eigen::AlignedBox<double, 3>& boxA, Eigen::AlignedBox<double, 3>& boxB);
    std::shared_ptr<cg3d::Model> createBox(Eigen::AlignedBox<double, 3>& box);
    cg3d::MeshData createDataFromBox(Eigen::AlignedBox<double, 3>& box);
    void KeyCallback(cg3d::Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods) override;
private:
    std::shared_ptr<Movable> root;
    std::shared_ptr<cg3d::Model> cyl, sphere1 ,cube1,cube2, sphere2;
    igl::AABB<Eigen::MatrixXd, 3> treeA1;
    igl::AABB<Eigen::MatrixXd, 3> treeB1;
    std::shared_ptr<cg3d::Material> material;
    float scaleMesh1 = 1;
    float scaleMesh2 = 1;
    bool is_intersec;
    bool d_key = false;
    bool a_key = false;
    bool w_key = false;
    bool s_key = false;
    bool left_key = false;
    bool right_key = false;
    bool down_key = false;
    bool up_key = false;
    bool only_move = true;
    float movement_speed = 0.01;



    void setCube(auto mesh, Eigen::AlignedBox<double, 3> &box, bool showWireframe, bool showFaces,bool showTextures);
};
