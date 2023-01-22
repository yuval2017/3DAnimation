#pragma once
#include "movingObject.h"
#include "Obstacle.h"
#include "HealthObject.h"
#include "ScoreObject.h"
#include "Game.h"


class ObjectManager {
public:
	ObjectManager(Game* viewer);

	void addMovingObstacles(int numOfObstacles);
	void addMovingObstacle();
	void reset();

	int  addObject(std::string pathToObject, std::string pathOfTexture);
	void addHealthObjects(int numOfHealths);
	void addHealthObject();

	void addScoreObjects(int numOfScoreObjects);
	void addScoreObject();
	void initSpawnLocations();
	int addSnakeHead();
	void updateObjects();

	Game* viewer;

	std::vector <Obstacle* >  movingObstacles;
	std::vector <HealthObject* >  healthObjects;
	std::vector <ScoreObject*  >  scoreObjects;

	std::vector <Eigen::Vector3d> spawnLocations;

	int currentMeshId;
	int numSnakeLinks;
};