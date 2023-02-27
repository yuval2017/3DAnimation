//
// Created by יובל היטר on 14/02/2023.
//

#include "Calculates.h"
#include <cmath>
#include <random>
#include <vector>
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <igl/per_vertex_normals.h>
#include "ModelsFactory.h"
using namespace Eigen;
using namespace std;


Calculates *Calculates::instancePtr = NULL;
vector<TexCoord> Calculates::calculateTextureCoordinates(vector<Vertex> vertices, vector<Face> faces, const std::string& filename ) {
    vector<TexCoord> texCoords;

    // Iterate over each face
    for (int i = 0; i < faces.size(); i++) {
        Face face = faces[i];

        // Calculate the vectors for two edges of the face
        Vertex v1 = vertices[face.v1];
        Vertex v2 = vertices[face.v2];
        Vertex v3 = vertices[face.v3];

        float edge1x = v2.x - v1.x;
        float edge1y = v2.y - v1.y;
        float edge1z = v2.z - v1.z;

        float edge2x = v3.x - v1.x;
        float edge2y = v3.y - v1.y;
        float edge2z = v3.z - v1.z;

        // Calculate the normal of the face
        float normalx = edge1y * edge2z - edge1z * edge2y;
        float normaly = edge1z * edge2x - edge1x * edge2z;
        float normalz = edge1x * edge2y - edge1y * edge2x;

        // Calculate the magnitude of the normal
        float length = sqrt(normalx * normalx + normaly * normaly + normalz * normalz);

        // Normalize the normal
        normalx /= length;
        normaly /= length;
        normalz /= length;

        // Calculate the texture coordinates for each vertex in the face
        for (int j = 0; j < 3; j++) {
            Vertex vertex = vertices[face.v1 + j];
            TexCoord texCoord;

            // Calculate the dot product between the vertex and the normal of the face
            float dotProduct = vertex.x * normalx + vertex.y * normaly + vertex.z * normalz;

            // Calculate the texture coordinates based on the dot product
            texCoord.u = dotProduct / length;
            texCoord.v = dotProduct / length;

            texCoords.push_back(texCoord);
        }
    }
    write_obj_file(vertices,faces,texCoords,filename);
    return texCoords;
}
void Calculates::write_obj_file(const std::vector<Vertex>& vertices,std::vector<Face> faces,std::vector<TexCoord> VT, const std::string& filename){
    std::ofstream out(filename);
    for (const auto& v : vertices) {
        out << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }
    for (const auto& vt : VT) {
        out << "vt " << vt.u << " " << vt.v << "\n";
    }
    for (const auto& f : faces) {
        out << "f " << f.v1 << " " << f.v2 << " " << f.v3 << "\n";
    }
    out.close();
}
std::vector<TexCoord> Calculates::getVertexTextureCoordinates(Eigen::MatrixXd vertices, Eigen::MatrixXi faces, std::string imagePath) {
    vector<Vertex> _vertices;
    vector<Face> _faces;
    for (int i = 0; i < vertices.rows(); i++) {
        _vertices.push_back({vertices.row(i)[0], vertices.row(i)[1], vertices.row(i)[2]});
    }
    for (int i = 0; i < faces.rows(); i++) {
        _faces.push_back({faces.row(i)[0], faces.row(i)[1]});
    }
    return getVertexTextureCoordinates(_vertices, _faces, imagePath);
}
#include <cmath>
#include <algorithm>
#include "stb_image.h"

std::vector<TexCoord> Calculates::getVertexTextureCoordinates(vector<Vertex> vertices, vector<Face> faces, std::string imagePath) {
    vector<TexCoord> textureCoordinates;
    ifstream file(imagePath.c_str(), ios::binary);
    int width, height, numChannels;
    unsigned char* imageData = stbi_load(imagePath.c_str(), &width, &height, &numChannels, 0);
    if (!imageData) {
        cerr << "Error loading image" << endl;
        return textureCoordinates;
    }
    for (int i = 0; i < vertices.size(); i++) {
        Vertex vertex = vertices[i];
        double u = (vertex.x + 1) * width / 2;
        double v = (1 - vertex.y) * height / 2;
        u = max(0.0, min(u, (double)width - 1));
        v = max(0.0, min(v, (double)height - 1));
        int pixelOffset = ((int)v * width + (int)u) * numChannels;
        double b = (unsigned char)imageData[pixelOffset] / 255.0;
        double g = (unsigned char)imageData[pixelOffset + 1] / 255.0;
        double r = (unsigned char)imageData[pixelOffset + 2] / 255.0;
        textureCoordinates.push_back({b, g});
    }
    stbi_image_free(imageData);
    write_obj_file(vertices,faces,textureCoordinates,"../tutorial/Final/myCube.obj");
    return textureCoordinates;
}





void Calculates::generateRandomBeizierCurve(Eigen::Vector3f vector, Eigen::Matrix <float, 4, 3 > &MG_Result){
    //Slide 15 Beizer_Curves_And_surfaces.pdf
    Eigen::Vector3f pRow1;
    Eigen::Vector3f pRow2;
    Eigen::Vector3f pRow3;
    Eigen::Vector3f pRow4;
    float xCoordinate = vector[0];
    float yCoordinate = vector[1];
    float zCoordinate = vector[2];

    std::random_device dev;
    std::mt19937       rng(dev());
    std::uniform_int_distribution<int> distributeX(xCoordinate, xCoordinate + 4);
    std::uniform_int_distribution<int> distributeY(yCoordinate - 1, yCoordinate + 5);
    std::uniform_int_distribution<int> distributeZ(zCoordinate - 5, zCoordinate + 5);

    pRow1 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));
    pRow2 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));
    pRow3 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));
    pRow4 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));
    Eigen::Matrix <float, 4, 3 > curvePoints;
    curvePoints.row(0) = pRow1;
    curvePoints.row(1) = pRow2;
    curvePoints.row(2) = pRow3;
    curvePoints.row(3) = pRow4;
    Eigen::Matrix4f M;
    M << -1, 3, -3, 1,
            3, -6, 3, 0,
            -3, 3, 0, 0,
            1, 0, 0, 0;

    MG_Result = M * curvePoints;
}


std::vector<double> Calculates::linspace(float start_in, float end_in, int num_in){

    std::vector<double> linspaced;

    double start = static_cast<double>(start_in);
    double end = static_cast<double>(end_in);
    double num = static_cast<double>(num_in);

    if (num == 0) { return linspaced; }
    if (num == 1)
    {
        linspaced.push_back(start);
        return linspaced;
    }

    double delta = (end - start) / (num - 1);

    for(int i=0; i < num-1; ++i)
    {
        linspaced.push_back(start + delta * i);
    }
    linspaced.push_back(end);
    return linspaced;
}



std::vector<TexCoord> Calculates::calculateTextureCoordinates(Eigen::MatrixXd vertices, Eigen::MatrixXi faces, const std::string& filename) {
    vector<Vertex> _vertices;
    for (int i = 0; i < vertices.rows(); i++) {
        float v1 = vertices.row(i)[0];
        float v2 = vertices.row(i)[1];
        float v3 = vertices.row(i)[2];
        _vertices.push_back({v1,v2,v3});
    }
    vector<Face> _faces;
    for (int i = 0; i < faces.rows(); i++) {
        int fv1 = faces.row(i)[0];
        int fv2 = faces.row(i)[1];
        int fv3 = faces.row(i)[2];
        _faces.push_back({fv1,fv2,fv3});
    }
    return calculateTextureCoordinates(_vertices,_faces,filename);
}
bool Calculates::isMeshCollision(const std::shared_ptr<cg3d::Movable>& mesh1, const std::shared_ptr<cg3d::Movable>&  mesh2, igl::AABB<Eigen::MatrixXd, 3>* treeA, igl::AABB<Eigen::MatrixXd, 3>* treeB){
    //base cases

    if (treeA == nullptr || treeB == nullptr){
        std::cout<< "not good here!!!!!" << " \n"<< std::endl;

        return false;
    }


    if (!isBoxesIntersect(treeA->m_box, treeB->m_box, mesh1, mesh2)) {
        return false;
    }
    if (treeA->is_leaf() && treeB->is_leaf()) {
        //if the smallest box intersect so draw the boxes on each mesh
        //setCube(mesh1 ,treeA->m_box,true,true,true);
        //setCube(mesh2 ,treeB->m_box,true,true,true);
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
bool Calculates::isBoxesIntersect(Eigen::AlignedBox<double, 3>& boxA, Eigen::AlignedBox<double, 3>& boxB, const std::shared_ptr<cg3d::Movable>& mesh1,const std::shared_ptr<cg3d::Movable>& mesh2){
    // matrix A
    Eigen::Matrix3d A = mesh1->GetRotation().cast<double>();
    Eigen::Vector3d A0 = A.col(0);
    Eigen::Vector3d A1 = A.col(1);
    Eigen::Vector3d A2 = A.col(2);

    // matrix B
    Eigen::Matrix3d B = mesh2->GetRotation().cast<double>();
    Eigen::Vector3d B0 = B.col(0);
    Eigen::Vector3d B1 = B.col(1);
    Eigen::Vector3d B2 = B.col(2);


    //C=A^T*B
    //calculate C so that: C_ij = A_iXB_j
    Eigen::Matrix3d C = A.transpose() * B;

    //length sides of bounding box
    Eigen::Vector3d a =  {boxA.sizes()[0]*mesh1->scale_factor[0],boxA.sizes()[1]*mesh1->scale_factor[1],boxA.sizes()[2]*mesh1->scale_factor[2]};
    Eigen::Vector3d b = {boxB.sizes()[0]*mesh2->scale_factor[0],boxB.sizes()[1]*mesh2->scale_factor[1],boxB.sizes()[2]*mesh2->scale_factor[2]};

    a = a / 2;
    b = b / 2;
    //create matrix D
    Eigen::Vector4d CA = Eigen::Vector4d(boxA.center()[0], boxA.center()[1], boxA.center()[2], 1);
    Eigen::Vector4d CB = Eigen::Vector4d(boxB.center()[0], boxB.center()[1], boxB.center()[2], 1);
    Eigen::Vector4d DOfVector4d = mesh2->GetAggregatedTransform().cast<double>() * CB - mesh1->GetAggregatedTransform().cast<double>() * CA;
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

void Calculates::setRandomCubeLocations(double domainX, double domainY, double domainZ,
                                                     int numCubes, double cubeSize, std::vector<Eigen::Vector3f> &coords) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disX(-domainX/2 + cubeSize/2, domainX/2 - cubeSize/2);
    std::uniform_real_distribution<> disY(-domainY/2 + cubeSize/2, domainY/2 - cubeSize/2);
    std::uniform_real_distribution<> disZ(-domainZ/2 + cubeSize/2, domainZ/2 - cubeSize/2);


    while (coords.size() < numCubes) {
        // Generate random cube center
        double x = disX(gen);
        double y = disY(gen);
        double z = disZ(gen);
        Eigen::Vector3d p = {x, y, z};

        // Check if cube is inside the domain and is at least 10 units from the origin
        double distFromOrigin = std::sqrt(x*x + y*y + z*z);
        if (std::abs(x) > domainX/2 || std::abs(y) > domainY/2 || std::abs(z) > domainZ/2 ||
            distFromOrigin < 10.0) {
            continue;
        }

        // Check if cube intersects with any other cubes
        bool intersects = false;
        for (const auto& c : coords) {
            if (doCubesIntersect(p, c.cast<double>(), cubeSize)) {
                intersects = true;
                break;
            }
        }
        if (intersects) {
            continue;
        }

        // Add cube center to vector
        Eigen::Vector3f object;
        object = p.cast<float>();
        coords.push_back(object);
    }
}

bool Calculates::doCubesIntersect(const Eigen::Vector3d& c1, const Eigen::Vector3d& c2, double cubeSize) {
    double d = std::sqrt((c1.x() - c2.x()) * (c1.x() - c2.x()) +
                         (c1.y() - c2.y()) * (c1.y() - c2.y()) +
                         (c1.z() - c2.z()) * (c1.z() - c2.z()));
    return d < 2 * cubeSize;
}

std::queue<Eigen::Vector3f> Calculates::generatePointsInSystem(float x_length, float y_length, float z_length, int n,float min_dist, Eigen::Vector3f point, std::vector<Eigen::Vector3f> points) {
//first try.

    //    // Define a random number generator
//    std::queue<Eigen::Vector3f> result_points;
//    float min_dist_sq = min_dist * min_dist;
//
//    // calculate grid size for n points
//    int grid_size = std::ceil(std::cbrt(n));
//
//    // calculate spacing between grid points
//    float x_spacing = x_length / grid_size;
//    float y_spacing = y_length / grid_size;
//    float z_spacing = z_length / grid_size;
//    // generate grid points
//    for (int i = 0; i < grid_size; ++i) {
//        for (int j = 0; j < grid_size; ++j) {
//            for (int k = 0; k < grid_size; ++k) {
//                float x = (i + 0.5) * x_spacing - x_length / 2.0;
//                float y = (j + 0.5) * y_spacing - y_length / 2.0;
//                float z = (k + 0.5) * z_spacing - z_length / 2.0;
//                Eigen::Vector3f grid_point(x, y, z);
//
//                // check distance to existing points
//                bool valid = true;
//                for (const auto& existing_point : points) {
//                    if ((grid_point - existing_point).squaredNorm() < min_dist_sq) {
//                        valid = false;
//                        break;
//                    }
//                }
//
//                // check distance to target point
//                if ((grid_point - point).squaredNorm() < min_dist_sq) {
//                    valid = false;
//                }
//
//                // add valid points to result
//                if (valid) {
//                    result_points.push(grid_point);
//                }
//
//                // stop if we have enough points
//                if (result_points.size() >= n) {
//                    return result_points;
//                }
//            }
//        }
//    }
//
//    return result_points;

//second try.

    // Define the minimum distance from the edges and the point
    float min_edge_dist = 5.0;
    float min_point_dist = 70.0;


    // Create a queue to store the points
    std::queue<Vector3f> pointQueue;

    // Generate new points until we have enough
    while (pointQueue.size() < n) {
        // Generate a random point within the system, subject to the constraints
        Vector3f newPoint;
        bool validPoint = false;
        while (!validPoint) {
            newPoint = Vector3f::Random();
            newPoint.x() *= x_length / 2.0 - min_edge_dist;
            newPoint.y() *= y_length / 2.0 - min_edge_dist;
            newPoint.z() *= z_length / 2.0 - min_edge_dist;
            newPoint += point;
            if ((newPoint - point).norm() >= min_point_dist) {
                bool tooClose = false;
                for (const auto& existingPoint : points) {
                    if ((newPoint - existingPoint).norm() < min_dist) {
                        tooClose = true;
                        break;
                    }
                }
                if (!tooClose) {
                    validPoint = true;
                }
            }
        }

        // Add the new point to the queue
        pointQueue.push(newPoint);
        std::cout << "point " << pointQueue.size() <<" added "<< endl;
    }

    return pointQueue;
}

MeshData Calculates::createDataFromBox(Eigen::AlignedBox<double, 3>& box){
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
std::shared_ptr<Model> Calculates::createBox(Eigen::AlignedBox<double, 3>& box){
    std::vector<std::shared_ptr<Mesh>> meshList;
    std::vector<MeshData> new_data;
    std::shared_ptr<Material> material = {std::make_shared<Material>("green_color", "shaders/phongShader")};
    material->program->name = "green";
    new_data.push_back(createDataFromBox(box));
    auto m = std::make_shared<Mesh>("cube",new_data);
    meshList.push_back(m);
    return Model::Create( "cube",meshList,material);
}

Eigen::Vector3f Calculates::generateRandomPoint(Eigen::Vector3f max, Eigen::Vector3f min, Eigen::Vector3f point, double min_d, double max_d) {
    Eigen::Vector3f random_point(3);
    float x, y, z;
    float r, theta, phi, d;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(-1.0, 1.0);
    uniform_real_distribution<double> dis_d(min_d, max_d);

    do {
        d = (float)dis_d(gen);
        // Generate a random point on the surface of a sphere with radius d
        // and center at the given point
        r = d;
        theta = (float)dis(gen) * M_PI;
        phi = (float)dis(gen) * 2.0 * M_PI;
        x = point[0] + r * sin(theta) * cos(phi);
        y = point[1] + r * sin(theta) * sin(phi);
        z = point[2] + r * cos(theta);
    } while (!(x < max[0] && x > min[0] && y < max[1] && y > min[1] && z < max[2] && z > min[2]));

    random_point[0] = x;
    random_point[1] = y;
    random_point[2] = z;

    return random_point;
}