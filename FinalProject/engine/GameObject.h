//
// Created by יובל היטר on 14/02/2023.
//
#pragma once
#include "Model.h"
#include <iostream>
namespace cg3d{
    class GameObject : virtual public Model{
public:
    void Accept(Visitor *visitor) override { visitor->Visit(this); };
        template<typename... Args>
        static std::shared_ptr<GameObject> Create(Args&&... args) {
            return std::shared_ptr<GameObject>{new GameObject{std::forward<Args>(args)...}}; // NOLINT(modernize-make-shared)
        }

protected:

    explicit GameObject(const std::string& file, std::shared_ptr<Material> material);
    GameObject(std::string name, const std::string& file, std::shared_ptr<Material> material);
    GameObject(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
    GameObject(std::string name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
    GameObject(std::string name, std::vector<std::shared_ptr<Mesh>> meshList, std::shared_ptr<Material> material);
    GameObject(const GameObject& other) = default; // important: doesn't add itself to the parent's children (object isn't constructed yet)
    GameObject(GameObject&&) = default; // important: doesn't add itself to the parent's children (object isn't constructed yet)
    GameObject& operator=(const GameObject& other) = default;


};

}





