#include "movingObject.h"
#include "Game.h"



movingObject::movingObject(float xCoordinate, float yCoordinate, float zCoordinate, int meshIdx, float speed, Game* viewer) {
	this->xCoordinate = xCoordinate;
	this->yCoordinate = yCoordinate;
	this->zCoordinate = zCoordinate;

	this->meshIdx = meshIdx;
	this->speed = speed;
	this->viewer = viewer;
	this->t = 0;
	this->spawnLocations = spawnLocations;
	this->moveBackwords = false;
    this->generateBeizerCurve();
}
int  movingObject::getMeshIdx() {
	return this->meshIdx;
}

float movingObject::getSpeed() {
	return this->speed;
}

void movingObject::generateBeizerCurve() {
	//Slide 15 Beizer_Curves_And_surfaces.pdf 

	Eigen::Vector3f pRow1;
	Eigen::Vector3f pRow2;
	Eigen::Vector3f pRow3;
	Eigen::Vector3f pRow4;

	std::random_device dev;
	std::mt19937       rng(dev());
	std::uniform_int_distribution<int> distributeX(xCoordinate, xCoordinate + 4);
	std::uniform_int_distribution<int> distributeY(yCoordinate - 1, yCoordinate + 5);
	std::uniform_int_distribution<int> distributeZ(zCoordinate - 5, zCoordinate + 5);

	pRow1 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));
	pRow2 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));
	pRow3 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));
	pRow4 = Eigen::Vector3f(distributeX(rng), distributeY(rng), distributeZ(rng));

	curvePoints.row(0) = pRow1;
	curvePoints.row(1) = pRow2;
	curvePoints.row(2) = pRow3;
	curvePoints.row(3) = pRow4;

	M << -1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 3, 0, 0,
		1, 0, 0, 0;

	MG_Result = M * curvePoints;
	//drawTheBeizerCurve(); 
}

void movingObject::drawTheBeizerCurve() {
	Eigen::Vector3d drawingColor = Eigen::RowVector3d(0, 0, 1);

	for (int i = 0; i < 3; i++)
		viewer->data_list[meshIdx].add_edges(curvePoints.row(i).cast<double>(), curvePoints.row(i + 1).cast<double>(), drawingColor);

	viewer->data_list[meshIdx].line_width = 3;
	viewer->data_list[meshIdx].show_lines = true;
	viewer->data_list[meshIdx].show_overlay_depth = true;
}

void movingObject::move()
{
	viewer->data_list[meshIdx].resetTranslation();
	std::random_device dev;
	std::mt19937       rng(dev());
	std::uniform_int_distribution<int> distributeZ(1, 5);
	std::uniform_int_distribution<int> distribute(1 , 7);
	viewer->data_list[meshIdx].MyTranslate(Eigen::Vector3d(distribute(rng), distribute(rng), distributeZ(rng)), true);
	
}

void movingObject::moveAccordingToBeizerCurve() {
	T[0] = powf(t, 3);
	T[1] = powf(t, 2);
	T[2] = t;
	T[3] = 1;

	currentPosition = (T * MG_Result);
	viewer->data_list[meshIdx].SetTranslation(currentPosition.cast<double>());
}


