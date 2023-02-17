//
// Created by יובל היטר on 14/02/2023.
//

#include "Calculates.h"
#include <cmath>
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
bool Calculates::isMeshCollision(std::shared_ptr<cg3d::Model> mesh1, Model* mesh2, igl::AABB<Eigen::MatrixXd, 3>* treeA, igl::AABB<Eigen::MatrixXd, 3>* treeB){
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
bool Calculates::isBoxesIntersect(Eigen::AlignedBox<double, 3>& boxA, Eigen::AlignedBox<double, 3>& boxB, const std::shared_ptr<cg3d::Model>& mesh1,Model* mesh2){
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
