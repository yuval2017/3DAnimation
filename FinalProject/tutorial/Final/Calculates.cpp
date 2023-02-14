//
// Created by יובל היטר on 14/02/2023.
//

#include "Calculates.h"
#include <cmath>
using namespace std;
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
