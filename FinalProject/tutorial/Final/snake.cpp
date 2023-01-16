#include "snake.h"
#include "sandBox.h"
#include <igl/vertex_triangle_adjacency.h>
#include <igl/directed_edge_orientations.h>
#include <igl/directed_edge_parents.h>
#include <igl/forward_kinematics.h>
#include <igl/PI.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/lbs_matrix.h>
#include <igl/deform_skeleton.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <imgui/imgui.h>

#include <igl/dqs.h>
#include <igl/readOBJ.h>
#include <igl/opengl/glfw/Viewer.h>
#include "Eigen/dense"
#include <igl/circulation.h>
#include <functional>



Snake::Snake(igl::opengl::glfw::Viewer* viewer, int numberOfJoints) 
{
	this->viewer = viewer;
	doubleVariable = 0;
	anim_t_dir = 0.015;
	anim_t = 0.0;
	this->viewer->numberOfLinks = numberOfJoints;
	idHead = 1;
	idTail = numberOfJoints ;
	this->numberOfJoints = numberOfJoints;
}

void Snake::initSnake(const std::string& config) {
	std::string item_name;
	std::ifstream nameFileout;
	doubleVariable = 0;
	nameFileout.open(config);
	if (!nameFileout.is_open())
	{
		std::cout << "Can't open file " << config << std::endl;
	}
	else
	{

		while (nameFileout >> item_name)
		{
			std::cout << "openning " << item_name << std::endl;
			viewer->load_mesh_from_file(item_name, "C:\\Users\\sabin\\Downloads\\EngineForAnimationCourse\\tutorial\\textures\\snake.png");

			viewer->data().add_points(Eigen::RowVector3d(0, 0, 0), Eigen::RowVector3d(0, 0, 1));
			viewer->data().point_size = 10;
			viewer->data().line_width = 2;
			viewer->data().show_lines = 1;
			viewer->data().set_visible(false, 1);
			V = viewer->data().V;
			float lengthOfSnake = viewer->data().V.colwise().maxCoeff()[2] - viewer->data().V.colwise().minCoeff()[2];
			viewer->link_length = lengthOfSnake / viewer->numberOfLinks;
			jointLength= lengthOfSnake / viewer->numberOfLinks;
			initJoints();
			viewer->data().show_overlay_depth = 1;
			viewer->data().show_overlay =0;
			viewer->data().set_visible(true, 2);
		}
		nameFileout.close();
	}

	viewer->data(0).set_colors(Eigen::RowVector3d(0.9, 0.1, 0.1));
}



void Snake::initJoints(){
	vC.resize(numberOfJoints+1);
	vT.resize(numberOfJoints + 1);
	vQ.resize(numberOfJoints+1,Eigen::Quaterniond::Identity());
	Eigen::Vector3d min = V.colwise().minCoeff();
	double min_z = min[2];
	//set parents 
	viewer->parents.push_back(-1);
	for (int i = 1; i < numberOfJoints+1; i++) {
		viewer->data_list.emplace_back();
		viewer->data_list.back().id = i;
	    viewer->parents.push_back(-1);
	
	}
	BE.resize(16, 2);
	Cp.resize(17, 3);
	CT.resize(32, 3);
	BE << 0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 5,
		5, 6,
		6, 7,
		7, 8,
		8, 9,
		9,10,
		10, 11,
		11, 12,
		12, 13,
		13, 14,
		14, 15,
		15, 16;
       

	restartSnake();
	//viewer->data_list[0].add_points(points, Eigen::RowVector3d(0, 0, 1));
	calcWeight(V, min_z);
	snakeJointBoxes.resize(numberOfJoints);
	initBoundingBoxesForJoints();
	//jointLength = 0.1;
}

void Snake::restartSnake(){
	viewer->data(0).set_vertices(V);
	Eigen::Vector3d min = V.colwise().minCoeff();
	Eigen::Vector3d max = V.colwise().maxCoeff();
	double min_z = min[2];
	float max_z = max[2];
	Eigen::Vector3d pos(0, 0, min_z);
	Eigen::MatrixXd points(17, 3);
	for (int i = 0; i < numberOfJoints + 1; i++) {
		vC[i] = pos;
		vT[i] = pos;
		points.row(i) = pos;
		pos = pos + Eigen::Vector3d(0, 0, jointLength);
	}
	for (int i = 1; i < numberOfJoints + 1; i++) {
		viewer->data_list[i].resetTranslation();
		viewer->data_list[i].SetCenterOfRotation(Eigen::Vector3d(0, 0, min_z + (i - 1) * jointLength));
		viewer->data_list[i].MyTranslate(vC[i - 1], true);
	}
	for (int i = 0; i < numberOfJoints + 1; i++) {
		Cp.row(i) = vC[i];
	}
	for (int i = 1; i < numberOfJoints + 1; i++) {
		CT.row(i * 2 - 1) = vC[i];
	}
}

void Snake::calcWeight(Eigen::MatrixXd& V, double min_z){
	int n = V.rows();
	W=Eigen::MatrixXd::Zero( n,numberOfJoints+1);
	for (int i = 0; i < n; i++) {
		double curr_z = V.row(i)[2];
		for (int j = 0; j < numberOfJoints+1; j++) {
			if (curr_z >= min_z + jointLength * j && curr_z <= min_z + jointLength * (j + 1)) {
				double res = abs(curr_z - (min_z + jointLength * (j + 1))) * 10;
				W.row(i)[j] = res;
				W.row(i)[j + 1] = 1-res ;
				break;
			}
		}

	}

}


void Snake::initBoundingBoxesForJoints() {
	for (int meshID = 1; meshID < numberOfJoints + 1; meshID++)
	{
		Eigen::Vector3d pos = vC[meshID - 1];
		Eigen::Vector3d m = pos + Eigen::Vector3d(-0.2, -0.2, -0.2);
		Eigen::Vector3d M = pos + Eigen::Vector3d(0.2, 0.2, 0.2);

		Eigen::AlignedBox<double, 3> boxForCurrentJoint;
		boxForCurrentJoint = Eigen::AlignedBox<double, 3>(m, M);
	   //  CreateBBFromBox(boxForCurrentJoint, 0);
		snakeJointBoxes[meshID - 1] = boxForCurrentJoint;
	}
}



void Snake::CreateBBFromBox(Eigen::AlignedBox <double, 3>& box, int index)
{

	Eigen::MatrixXd V_box(8, 3);
	V_box << box.corner(box.BottomLeftCeil).transpose(),
		box.corner(box.BottomLeftFloor).transpose(),
		box.corner(box.BottomRightCeil).transpose(),
		box.corner(box.BottomRightFloor).transpose(),
		box.corner(box.TopLeftCeil).transpose(),
		box.corner(box.TopLeftFloor).transpose(),
		box.corner(box.TopRightCeil).transpose(),
		box.corner(box.TopRightFloor).transpose();

	Eigen::MatrixXi E_box(12, 2);
	E_box <<
		0, 1,
		1, 3,
		2, 3,
		2, 0,
		4, 5,
		5, 7,
		6, 7,
		6, 4,
		0, 4,
		1, 5,
		2, 6,
		7, 3;
	for (unsigned i = 0; i < E_box.rows(); ++i)
		viewer->data_list[index].add_edges
		(
			V_box.row(E_box(i, 0)),
			V_box.row(E_box(i, 1)),
			Eigen::RowVector3d(1, 0, 0)
		);
}
void Snake::skinnig(Eigen::Vector3d t) {
	moveSnake(t);
	const int dim = Cp.cols();
	Eigen::MatrixXd T(BE.rows() * (dim + 1), dim);
	for (int e = 0; e < BE.rows(); e++)
	{
		Eigen::Affine3d a = Eigen::Affine3d::Identity();
		a.translate(vT[e]);
		a.rotate(vQ[e]);
		T.block(e * (dim + 1), 0, dim + 1, dim) =
			a.matrix().transpose().block(0, 0, dim + 1, dim);
	}
	igl::dqs(V, W, vQ, vT, U);
	//move joints according to T, returns new position in CT and BET
	igl::deform_skeleton(Cp, BE, T, CT, BET);
	 viewer->data(0).set_vertices(U);
	 viewer->data(0).set_edges(CT, BET,Eigen::RowVector3d(70. / 255., 252. / 255., 167. / 255.));
	 for (int i = 0; i < numberOfJoints + 1; i++) 
		 vC[i] = vT[i];

	 for (int i =1; i < numberOfJoints + 1; i++)
		 viewer->data_list[i].SetTranslation(CT.row(2*i-1));

	
	
}

Eigen::Matrix4d Snake::getHeadMakeTransd()
{

	Eigen::Affine3d a = Eigen::Affine3d::Identity();
	a.translate((Eigen::Vector3d)CT.row(31));
	a.rotate(quat);
	return a.matrix();
}


void Snake::moveSnake(Eigen::Vector3d t){
	for (int i = 0; i < numberOfJoints; i++) 
		vT[i] = vT[i] + (vT[i + 1] - vT[i]) / 10;
	vT[idTail] = vT[idTail] + t;
	//update the joints
	for (int i = 1; i < numberOfJoints + 1; i++) {
		quat = Eigen::Quaterniond::FromTwoVectors(vC[i] - vC[i - 1], vT[i] - vC[i - 1]);
		Eigen::Matrix3d mat =viewer->data_list[i].MakeTransd().block<3, 3>(0, 0);
		viewer->data_list[i].RotateInSystem(mat,quat);		
	}
	
}
