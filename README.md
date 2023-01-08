# 3DAnimation

## Run instructions 
In order to run one of those assignments, do as explained: 
- Clone the the project into your computer.
- Download the CMake gui program.
- Choose the generator for the project (unix makefiles for linux and mac, and visual studio for windows). 
- Run the cmake gui on the specific assignment folder(not on the folder that contains all of the assignment!), and set the output to be in the 'build' folder inside the assignment folder ( if there is no such folder, create one).
- Open the specific assignment folder with your IDE.
- Run the bin files in the project. 

  ## Assignment 1 
  Implementing mesh reduction for a given shape represented by vertices and faces. For each reduction, 10% of the mesh number will be reduced. 
The changes were in the classes:
  

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

  - Q : Priority queue that holds the list of the edges to remove by cost.
  - num_collapsed : Counter for number of collapses in each iteration to reach 10%. 
  - Qt : Priority queue for each edge.
  - inf : Value of deleted edges.
  - curr_collaps_index : Counter for the num of collapses made will now.
  - max : Max number of collapsed needed to be done.
  - done_collaps : Flag for the collapse action.
  
  
  ## Assignment 2
  Implementing a collision detection mechanism using AABB and Eigen boxes. 
  The chnges were in the files below : 
  
  ### Solution.cpp
  The main scene file. Implementing the mehods below: 
  
  
  #### isMeshCollision
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
  - cyl, sphere1 ,cube1,cube2, sphere2 : The two objects, the boxis that contains them, and the boxes who collided.
  - treeA1 : AABB tree of the first object. 
  - treeB1 : AABB tree of the second object 
  - movement_speed : Setting the the speed the two object will move towards each other.
  
  The rest are callback variables, and init data. 
  
  - material;
  - scaleMesh1
  - scaleMesh2
  - is_intersec
  - d_key 
  - a_key 
  - w_key
  - s_key 
  - left_key 
  - right_key 
  - down_key  
  - up_key
  - only_move
  
  
   ## Assignment 3
  Implementing Kinematic chain and inverse Kinematics.
   The chnges were in the files below : 

  ### BasicScene.cpp
  The main scene file. Implementing the mehods below: 
  
  
  #### Init
  Initializing the class variables, init the links in the scene, setting their position and init the connection of parent-child between each two connected links. 
  
  
  #### IKCoordinateDecent
  This function implementation of the inverse kinematics algorithm known as Cyclic Coordinate Descent (CCD).

The algorithm starts by setting the target position of the end effector (the last link of the chain) to the position of a sphere object. Then it checks if the distance between the first link of the chain and the target position is greater than the sum of the lengths of all the links in the chain. If it is, then it returns and stops the animation. Otherwise, it proceeds to iteratively rotate each link in the chain in order to move the end effector closer to the target position.

For each link, the function calculates the position of the start and end of the link, as well as the position of the target. It then calculates the normal vector of the plane formed by the link and the target position, as well as the angle between them. The function then rotates the link by this angle around the normal vector in order to move the end effector closer to the target position. The function repeats this process for each link, starting from the last link and working its way backwards towards the first link.

Finally, the function checks if the distance between the end effector and the target position is within a certain tolerance, and if it is, then it stops the animation.


  #### IKFabric Algorithm
  This implement the "FABRIK" (Forward And Backward Reaching Inverse Kinematics) algorithm for solving an inverse kinematics problem for a kinematic chain with a given target position. 
  
  The kinematic chain is made up of a series of "links" (cylinders in this case) that are connected by joints. The FABRIK algorithm starts by assuming that the end effector (the last link in the chain) is at the target position and then iteratively works its way back through the chain, adjusting the positions of each link based on its constraints until the starting link (the "base" link) is at its desired position. The algorithm then works its way forward through the chain again, adjusting the positions of each link until the end effector reaches the target position within a certain tolerance. This process is repeated until the end effector is close enough to the target position.
  
  The function first checks if it should animate the FABRIK algorithm and, if not, returns. It then initializes some variables, including the target position and the position of the first link in the chain. It then iteratively works its way back through the chain, adjusting the positions of each link until the base link is at its desired position. It then works its way forward through the chain again, adjusting the positions of each link until the end effector is close enough to the target position. If the end effector is not close enough to the target position, the process is repeated until it is. If the end effector is close enough to the target position, the function returns.

 #### ikRotateHelper
  This function seems to be a helper function used to perform inverse kinematics (IK) on a chain of cylinders and used in the Fabrik algorithms.
  The function takes an integer 'id' and a target position 't' as input, and rotates the cylinder with the given id such that its end effector (the position of the cylinder's tip) is moved closer to the target position 't'.
  
  The function first calculates the position of the base of the cylinder with the given 'id' ('r'), the position of the end effector of the cylinder with the id ('e'), and the displacement vector between the target position 't' and the base of the cylinder ('rd'). It then calculates the cross product of the normalized versions of the displacement vector between the end effector and the base (re) and rd, and stores it in the variable normal.

Next, the function calculates the dot product of the normalized versions of rd and re and stores it in the variable dot. If dot is outside the 
  range [-1, 1], it is clamped to the range [-1, 1]. The function then calculates the angle between rd and re using the inverse cosine function 
  and divides the result by 100.

The function then calculates a rotation matrix rot using the calculated angle and normal as the axis of rotation. It then calculates the Euler angles corresponding to rot and uses them to rotate the cylinder with id in the X, Y, and Z axes. Finally, the function rotates the cylinder with id by the calculated angle about the axis rotationVec.
  
  
  #### ikGetPosition
  This is a function to calculate the position of a point on the cylinder with the given id. The length parameter determines whether the position is at the top or bottom of the cylinder. If length is positive, it returns the position of the top of the cylinder, and if it is negative, it returns the position of the bottom of the cylinder.

To calculate the position, the function first gets the center of the cylinder using cyls[id]->GetAggregatedTransform().col(3).head(3). Then it creates a displacement vector mov that moves the center of the cylinder along the z-axis by length. Finally, it adds the displacement vector to the center of the cylinder, taking into account the cylinder's rotation using cyls[id]->GetRotation().
  
  
  #### ScrollCallback
  In this function, the ScrollCallback changed to support the translate of the picked object away and to the
camera (perpendicular to camera plane). When no object is picked translate the whole scene. As asked in the assignment.
  
  
  #### KeyCallback
  Adding the callbacks : 
  
  - 'space' : starts and stops IK solver animation.
  - 'left' : rotates picked link around the previous link Y axis with the 'angle' variable .
  - 'right' : rotates picked link around the previous link Y axis with the -'angle' variable .
  - 'up' : rotates picked link around the current X axis with the 'angle' variable .
  - 'down' : rotates picked link around the current X axis with the -'angle' variable .
  - 'p' : prints rotation matrices of the picked link.
  - 't' : prints arms tip positions.
  - 'd' : prints destination position.
  - 'n' : pick the next link, or the first one in case the last link is picked.
  
  #### GetSpherePos
  This function is calculating the position of a sphere based on the position and orientation of one of the links in the robot. The position of the link is determined by the cyls[tipIndex] object and the position of the sphere is based on the orientation of this link with an offset of l along the x-axis. The result is then returned as a 3D vector.
  
  
  #### print_positions
  This function prints the tip position of a given cylinder.
  
  
  #### create_new_Rotation_q
  This function returns the new rotation matrix of Model object over the axis in the argument 'xyz' with the radian value in 'angle'.
  this function uses the quaterinion that exists in the Eigen labrary named 'Quaternionf' and make use in eule rangles given by the 'eulerAngles' method of a given matrix (that is the former rotation matrix of the object).
  
  
  #### print_rotation
  Prints the rotation matrix of the picked cylinder, or the rotation matrix of the system if there is no cylinder chosen. 
  
  
  #### get_euler_rotate_matrix
  Returns the rotation matrix of a given object using the three euler angles phi,theta,psi. This function uses the Eigen method 'eulerAngles' in order to get the angles from the previous rotation matrix of the object.
 
 
 #### reset
 A method for resetting the system as asked in the assignment. It resets the position of the whole scene objects as set in the initialization.
 
 
  
  ### BasicScene.h
  Adding the variables: 
  
  - link_len;
  - shouldAnimateCCD
  - lastLinkIndex : Index of the last cylinder ( number of cylinders -1).
  - firstLinkIndex : Index of the first cylinder/link (usually 0).
  - shouldAnimateFabrik : Flag for starting the Fabrik algorithm.
  - num_of_cyls : Number of cylinders.
  - parents : Vector representing the parent cylinder of a selected cylinder by its id number.
  - hildren : Vector representing the child cylinder of a selected cylinder by its id number.
  - decent = : Flag for starting the Decent algorithm.
  - angle : The rotation angle of the links.
  - addCyl : Adding cylinder flag.
  - scaleFactor : The scale factor of the shaped made during the init.
