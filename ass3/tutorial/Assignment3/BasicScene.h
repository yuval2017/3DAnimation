#pragma once
#include "AutoMorphingModel.h"
#include "Scene.h"

#include <memory>
#include <utility>

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
     void IKCoordinateDecent();
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
    int lastLinkIndex ;
    int firstLinkIndex ;
    bool ikSolverConstrainDegree = false;
    bool shouldAnimateFabrik = false;
    int num_of_cyls;
    std::vector<int> parents ;
    std::vector<int> children;

    Eigen::Vector3f ikGetPosition(int id, float length);

    Eigen::Matrix4f MakeTransd(std::shared_ptr<cg3d::Model> shape);

    void fix_rotate();

    void ikSolverHelper(int id, Eigen::Vector3f t);

    void IKFabric();

    Eigen::Matrix4f CalcParentTransWithOutRoot(int index);
};
