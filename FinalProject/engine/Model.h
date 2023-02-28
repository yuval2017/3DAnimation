#pragma once

#include <iostream>
#include <utility>
#include <igl/AABB.h>
#include "Mesh.h"
#include "Material.h"
#include "Movable.h"
#include "ViewerData.h"
#include "../tutorial/Final/Stopper.h"
namespace cg3d
{

class Model : virtual public Movable
{
    friend class DrawVisitor;

protected:
    Model(const std::string& file, std::shared_ptr<Material> material);
    Model(std::string name, const std::string& file, std::shared_ptr<Material> material);
    Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
    Model(std::string name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
    Model(std::string name, std::vector<std::shared_ptr<Mesh>> meshList, std::shared_ptr<Material> material);
    Model(const Model& other) = default; // important: doesn't add itself to the parent's children (object isn't constructed yet)
    Model(Model&&) = default; // important: doesn't add itself to the parent's children (object isn't constructed yet)
    Model& operator=(const Model& other) = default;



public:
    void setMaterial(std::shared_ptr<Material> _material){material = std::move(_material);};
    template<typename... Args>
    static std::shared_ptr<Model> Create(Args&&... args) {
        return std::shared_ptr<Model>{new Model{std::forward<Args>(args)...}}; // NOLINT(modernize-make-shared)
    }
    //add to eigen
    void SetTreeAndCube(std::shared_ptr<Model> cube, igl::AABB<Eigen::MatrixXd, 3> tree);
    igl::AABB<Eigen::MatrixXd, 3> *GetTree();
    ~Model() override = default;

    void Accept(Visitor* visitor) override { visitor->Visit(this); };

    std::shared_ptr<Material> material;
    bool isHidden = false;
    bool showFaces = true;
    bool showTextures = true;
    bool showWireframe = false;
    Eigen::Vector4f wireframeColor{0, 0, 0, 0};
    int meshIndex = 0;
    int mode = 0;
    //add to engine for gameplay
    Eigen::Matrix <float, 4, 3 > MG_Result;
    bool moveBackwards = false;
    float t = 0;
    Eigen::Vector3f GetPosition();
    std::shared_ptr <Model> bezier;
    float bezier_speed = 1.0f;
    Stopper stopper;
    //add to eigen
    std::shared_ptr<Model> cube;
    igl::AABB<Eigen::MatrixXd, 3> treeA1;
    bool is_tree_inited = false;

    inline std::shared_ptr<Mesh> GetMesh(int index = 0) const { return meshList[index]; }
    inline std::vector<std::shared_ptr<Mesh>> GetMeshList() const { return meshList; }
    void SetMeshList(std::vector<std::shared_ptr<Mesh>> _meshList);
    void UpdateDataAndDrawMeshes(const Program& program, bool _showFaces, bool bindTextures); // helper function
    void AddOverlay(const OverlayData& data, bool drawPoints);
    void setMeshData(const std::string& name,  Eigen::MatrixXd _V,  Eigen::MatrixXi _F,Eigen::MatrixXd _VN, Eigen::MatrixXd textureCoords);
private:
    static void UpdateDataAndBindMesh(igl::opengl::ViewerData& viewerData, const Program& program); // helper function

    static std::vector<igl::opengl::ViewerData> CreateViewerData(const std::shared_ptr<Mesh>& mesh);
    std::vector<std::shared_ptr<Mesh>> meshList;
    std::vector<std::vector<igl::opengl::ViewerData>> viewerDataListPerMesh;

    // TODO: TAL: handle the colors...
    Eigen::RowVector4f ambient = Eigen::RowVector4f(1.0, 1.0, 1.0, 1.0);
    Eigen::RowVector4f diffuse = Eigen::RowVector4f(1.0, 1.0, 1.0, 1.0);
    Eigen::RowVector4f specular = Eigen::RowVector4f(1.0, 1.0, 1.0, 1.0);


};

} // namespace cg3d
