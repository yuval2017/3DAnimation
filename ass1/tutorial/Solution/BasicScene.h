#pragma once

#include "Scene.h"
#include <utility>

#include <igl/collapse_edge.h>

#include "igl/opengl/glfw/Viewer.h"
#include "igl/aabb.h"
#include <igl/circulation.h>
#include <igl/collapse_edge.h>
#include <igl/edge_flaps.h>
#include <igl/decimate.h>
#include <igl/shortest_edge_and_midpoint.h>
#include <igl/parallel_for.h>
#include <igl/read_triangle_mesh.h>
#include <Eigen/Core>
#include <iostream>
#include <set>
#include <igl/vertex_triangle_adjacency.h>

class BasicScene : public cg3d::Scene
{
public:
    explicit BasicScene(std::string name, cg3d::Display* display) : Scene(std::move(name), display) {};
    void Init(float fov, int width, int height, float near, float far);
    void Update(const cg3d::Program& program, const Eigen::Matrix4f& proj, const Eigen::Matrix4f& view, const Eigen::Matrix4f& model) override;
    void KeyCallback(cg3d::Viewport* _viewport, int x, int y, int key, int scancode, int action, int mods) override;
    int Simp();
    void reset();
    void evalQ();
    void calculateEdgesCost(int edge);

private:
    std::vector <Eigen::Matrix4d> QPerVertex;
    std::shared_ptr<Movable> root;
    std::shared_ptr<cg3d::Model> cyl, sphere1 ,cube, curr_mesh;
    std::string curr_mesh_name;
    //EMAP - connects faces to ages
    //EF connect edge to face
    //EI connects edge to vertex index
    Eigen::VectorXi EMAP;
    Eigen::MatrixXi F,IF,E,EF,EI;
    Eigen::VectorXi EQ;
    //C - position of the new vertex after collapsing corresponding edge
    Eigen::MatrixXd V,IV, C;
    igl::min_heap< std::tuple<double,int,int> > Q;
    typedef std::set<std::pair<double, int> > PriorityQueue;
    std::vector<PriorityQueue::iterator > QtIterator;
    int num_collapsed;
    //priority queue for each edge
    PriorityQueue Qt;
    PriorityQueue::iterator inf;

    int curr_collaps_index = 0;
    int max;
    std::vector <bool> done_collaps;
    bool collapse_edge();
    void set_mesh( Eigen::MatrixXd OV,  Eigen::MatrixXi OF, Eigen::MatrixXd vertexNormals);
};
