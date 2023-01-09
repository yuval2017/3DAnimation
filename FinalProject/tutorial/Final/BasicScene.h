#pragma once
#include "AutoMorphingModel.h"
#include "Scene.h"

#include <memory>
#include <utility>
using namespace cg3d;
class BasicScene : public cg3d::Scene
{
public:
     explicit BasicScene(std::string name, cg3d::Display* display) : Scene(std::move(name), display) {};
     void Init(float fov, int width, int height, float near, float far);
     void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;
     void MouseCallback(cg3d::Viewport* viewport, int x, int y, int button, int action, int mods, int buttonState[]) override;
     void ScrollCallback(cg3d::Viewport* viewport, int x, int y, int xoffset, int yoffset, bool dragging, int buttonState[]) override;
     void CursorPosCallback(cg3d::Viewport* viewport, int x, int y, bool dragging, int* buttonState)  override;
     void KeyCallback(cg3d::Viewport* viewport, int x, int y, int key, int scancode, int action, int mods) override;
     Eigen::Vector3f GetSpherePos();
     void Draw_changes() override;
     void IKCoordinateDecent();
     void ikRotateHelper(int id, const Eigen::Vector3f& t);
private:
    std::shared_ptr<Movable> root;
    std::shared_ptr<cg3d::Model> sphere1 ,cube;
    std::shared_ptr<cg3d::AutoMorphingModel> autoCube;
    std::vector<std::shared_ptr<cg3d::Model>> cyls, axis;
    int pickedIndex = 0;
    int tipIndex = 0;
    Eigen::VectorXi EMAP;
    Eigen::MatrixXi F,E,EF,EI;
    Eigen::VectorXi EQ;

  // If an edge were collapsed, we'd collapse it to these points:
    Eigen::MatrixXd V, C, N, T, points,edges,colors;
    float link_len;
    bool shouldAnimateCCD= false;
    int lastLinkIndex = 2 ;
    int firstLinkIndex = 0 ;
    bool shouldAnimateFabrik = false;
    int num_of_cyls = lastLinkIndex + 1;
    std::vector<int> parents ;
    std::vector<int> children;
    bool decent = true;
    float angle= 0.05f;
    bool addCyl = false;
    float scaleFactor = 1;

    Eigen::Vector3f ikGetPosition(int id, float length);
    void IKFabric();
    void reset();
    Eigen::Matrix3f get_euler_rotate_matrix(std::shared_ptr<cg3d::Model> scene,float phi, float theta, float psi);



    void print_positions();

    void print_rotation();

    Eigen::Matrix3f create_new_Rotation_q(std::shared_ptr<cg3d::Model> scene,int xyz, float add_angle);
};
