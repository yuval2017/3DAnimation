# 3DAnimation

  ## Assignment 1 
  Implementing mesh reduction for a given shape represented by vertices and faces. For each reduction, 10% of the mesh number will be reduced. 
The changes are in the classes:
  

  ### BasicScene.cpp
  The main scene file. Implementing the mehods below: 


  #### Init
  Adding the initialization of the variables in the .h file, and reading the mesh detaild as V,F, etc.


  #### Simp
  handling the animation of collapsing edges in a mesh.
The function first checks if the application is currently animating and if there are edges to collapse in the Qt.
If both of these conditions are true, the function will attempt to collapse 10% of the edges in the Qt.
If any edges are collapsed, the function updates the mesh with the new vertex and face information and marks the current collapse index as done in the done_collaps array.


  #### evalQ
  calculating the QPerVertex data structure, which is a matrix that represents the local geometry around each vertex in a mesh. The QPerVertex matrix is initialized with zeros and then a matrix Q is calculated for each vertex in the mesh.


  #### calculateEdgesCost
  calculating the cost of collapsing an edge in a mesh and for determining the new vertex position for the collapsed edge. The edge to be collapsed is specified by the input integer edge.


  #### collapse_edge
  This function is responsible for collapsing the lowest cost edge in a mesh using the Iterative Vertex Contraction (IVC) algorithm. The IVC algorithm is a   method for simplifying a mesh by iteratively collapsing the edge with the lowest cost until a desired level of simplification is achieved.


  #### set_mesh
  This function is responsible for updating the mesh data in an application. The mesh data is specified by the input matrices OV and OF, which represent the  vertex positions and face indices of the mesh, respectively, and the input matrix vertexNormals, which represents the normal vector for each vertex in the   mesh.

  ### BasicScene.h
  Adding the variables: 

  - igl::min_heap< std::tuple<double,int,int> > Q  =>  priority queue that holds the list of the edges to remove by cost.
  - int num_collapsed  =>  counter for number of collapses in each iteration to reach 10%. 
  - PriorityQueue Qt  =>  priority queue for each edge.
  - PriorityQueue::iterator inf => value of deleted edges.
  - int curr_collaps_index = 0;
  - int max. => max number of collapsed needed to be done.
  - std::vector <bool> done_collaps  =>  flag for the collapse action.
  
  
  ## Assignment 2
  Implementing a collision detection mechanism using AABB and Eigen boxes. 
  The chnges was in the files below : 
  
  ### Solution.cpp
  The main scene file. Implementing the mehods below: 
  
  
  ####isMeshCollision
  This function is responsible for detecting whether two meshes are in collision with each other using an Accelerated AABB Tree (AABB tree) data structure. AABB trees are used to efficiently compute collision detection between two meshes by recursively comparing the bounding boxes of each mesh.
  If the bounding boxes of the two meshes intersect and both AABB trees are leaf nodes, the function returns true to indicate that the meshes are in collision. If either AABB tree is not a leaf node, the function recursively calls itself on each possible combination of the left and right children of the two AABB trees. If any of the recursive calls returns true, the function returns true to indicate that the meshes are in collision. Otherwise, the function returns false to indicate that the meshes are not in collision.
  
  #### isBoxesIntersect
  This function is checking if two bunding boxes intersect. It does this by first checking if the distance between the centers of the two boxes is smaller than the sum of the lengths of their sides. If this is not the case, the boxes do not intersect and the function returns false. If the distance is smaller, the function continues to check 15 conditions involving the orientation of the boxes and the distance between their centers, and returns true if any of these conditions is satisfied. If none of the conditions are satisfied, the function returns false.
  
 
  #### createDataFromBox
  This method returns mesh data out of a box such as : V, F, NV, T
  
  
  #### createBox
  Method for creating the bounding box of the models in the scene.
  
  
  #### setCube
  This method shows the cube in the mesh.
  
  
  
  ### Solution.h
  Adding the variables: 
  - std::shared_ptr<cg3d::Model> cyl, sphere1 ,cube1,cube2, sphere2  => The two objects, the boxis that contains them, and the boxes who collided.
  - igl::AABB<Eigen::MatrixXd, 3> treeA1  => AABB tree of the first object. 
  - igl::AABB<Eigen::MatrixXd, 3> treeB1 => AABB tree of the second object 
  - float movement_speed = 0.01  =>  setting the the speed the two object will move towards each other.
  
  The rest are callback variables, and init data. 
  
  - std::shared_ptr<cg3d::Material> material;
  - float scaleMesh1 = 1;
  - float scaleMesh2 = 1;
  - bool is_intersec;
  - bool d_key = false;
  - bool a_key = false;
  - bool w_key = false;
  - bool s_key = false;
  - bool left_key = false;
  - bool right_key = false;
  - bool down_key = false;
  - bool up_key = false;
  - bool only_move = true;
  
  
  

