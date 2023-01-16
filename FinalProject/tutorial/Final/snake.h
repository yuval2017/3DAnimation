#pragma once
#include "igl/opengl/glfw/Viewer.h"
#include "igl/opengl/ViewerData.h"
class Snake{
public:
	Snake(igl::opengl::glfw::Viewer* viewer, int numberOfJoints);
	void initSnake(const std::string& config);
    void initJoints();
    void restartSnake();
    void moveSnake(Eigen::Vector3d t);
    void calcWeight(Eigen::MatrixXd& V, double min_z);
    void skinnig(Eigen::Vector3d t);
    Eigen::Matrix4d getHeadMakeTransd();
    Eigen::Vector3d getTipPos() {
        return  CT.row(31);
    }
 
    Eigen::Vector3d getJointPos(int i) {
        return CT.row(i*2-1);
    }
   
    Eigen::Matrix3d getCurrentRotation() { return quat.toRotationMatrix(); };
    void initBoundingBoxesForJoints();
    void CreateBBFromBox(Eigen::AlignedBox <double, 3>& box, int index);
    std::vector<Eigen::AlignedBox <double, 3>> snakeJointBoxes;
private:
    int idHead;
    int idTail;
    double jointLength;
    int numberOfJoints;
	igl::opengl::glfw::Viewer* viewer;
    typedef
        std::vector<Eigen::Quaterniond, Eigen::aligned_allocator<Eigen::Quaterniond> >
        RotationList;
    double doubleVariable;
    // W - weights matrix
    // BE - Edges between joints
    // Cp - joints positions
    // P - parents
    // M - weights per vertex per joint matrix
    // U - new vertices position after skinning
    Eigen::MatrixXd V;
    Eigen::MatrixXd VHead; // Vertices of the current mesh (#V x 3)
    Eigen::MatrixXi FHead; // Faces of the mesh (#F x 3)
    Eigen::MatrixXd CT;
    Eigen::MatrixXi BET;
    Eigen::MatrixXd W, Cp, U, M;
    Eigen::MatrixXi  BE;
    Eigen::VectorXi P;
    RotationList rest_pose;
    RotationList anim_pose;
    std::vector<RotationList > poses; // rotations of joints for animation
    RotationList vQ;
    std::vector<Eigen::Vector3d> vC;
    std::vector<Eigen::Vector3d> vT;
    Eigen::Quaterniond quat;
    double anim_t ;
    double anim_t_dir;
 
};