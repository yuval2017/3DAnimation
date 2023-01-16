#pragma once
#include "igl/opengl/glfw/Viewer.h"
#include "igl/opengl/ViewerData.h"
#include <time.h>

class backgroundHandler {
public:
	backgroundHandler( igl::opengl::glfw::Viewer* viewer);
	void initBackground();

private:
	igl::opengl::glfw::Viewer* viewer;
	int ground;
};