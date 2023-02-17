//
// Created by יובל היטר on 17/02/2023.
//
#pragma once
#include <iostream>
#include <vector>
#include "iostream"
#include "Model.h"
#include "Material.h"
#include "Camera.h"
#include "igl/opengl/glfw/Viewer.h"
#include "igl/opengl/ViewerData.h"

using namespace cg3d;
class ModelsFactory {
    private:
    static ModelsFactory* instancePtr;
    ModelsFactory(){}
public:
    ModelsFactory(const ModelsFactory& obj) = delete;
    static ModelsFactory *getInstance(){
        if (instancePtr == NULL){
            instancePtr = new ModelsFactory();
        }
        return instancePtr;
    }
};


