# 3DAnimation

## Assignment 1 
Implementing mesh reduction for a given shape represented by vertices and faces. For each reduction, 10% of the mesh number will be reduced. 
The changes are in the classes:

### ass1/tutorial/Solution/BasicScene.cpp
Implementing the methods bellow: 

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
This function is responsible for collapsing the lowest cost edge in a mesh using the Iterative Vertex Contraction (IVC) algorithm. The IVC algorithm is a method for simplifying a mesh by iteratively collapsing the edge with the lowest cost until a desired level of simplification is achieved.

#### set_mesh
This function is responsible for updating the mesh data in an application. The mesh data is specified by the input matrices OV and OF, which represent the vertex positions and face indices of the mesh, respectively, and the input matrix vertexNormals, which represents the normal vector for each vertex in the mesh.

### ass1/tutorial/Solution/BasicScene.h
Adding the variables: 

- igl::min_heap< std::tuple<double,int,int> > Q  =>  priority queue that holds the list of the edges to remove by cost.
- int num_collapsed  =>  counter for number of collapses in each iteration to reach 10%. 
- PriorityQueue Qt  =>  priority queue for each edge.
- PriorityQueue::iterator inf => value of deleted edges.
- int curr_collaps_index = 0;
- int max. => max number of collapsed needed to be done.
- std::vector <bool> done_collaps  =>  flag for the collapse action.
  
  

