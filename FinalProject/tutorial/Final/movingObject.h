#pragma once
#include "igl/opengl/glfw/Viewer.h"
#include "igl/opengl/ViewerData.h" 
#include <time.h> 
#include <random>
#include "Game.h"

class movingObject {
public:
	movingObject(float xCoordinate, float yCoordinate, float zCoordinate, int meshIdx, float speed, Game* viewer);

	int   getMeshIdx();
	float getSpeed();

	void  generateBeizerCurve();
	void  moveAccordingToBeizerCurve();
	void  drawTheBeizerCurve();
	void  move();

	Eigen::Matrix4f               M;
	Eigen::Matrix <float, 4, 3 > MG_Result;
	Eigen::Matrix <float, 4, 3 > curvePoints;

	Eigen::RowVector4f            T;
	Eigen::Vector3f               currentPosition;
	float                         t;
	std::vector<Eigen::Vector3d>  spawnLocations;

	float xCoordinate;
	float yCoordinate;
	float zCoordinate;

	int   meshIdx;
	float speed;

	bool moveBackwords;
	Game* viewer;

	virtual void  onSnakeCollide() = 0;
	virtual void  update() = 0;
};