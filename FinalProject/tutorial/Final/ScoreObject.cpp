#include "ScoreObject.h"
#include "Game.h"
#include "SoundManager.h"

ScoreObject::ScoreObject(float xCoordinate, float yCoordinate, float zCoordinate, int meshIdx, float speed, Game* viewer)
	: movingObject(xCoordinate, yCoordinate, zCoordinate, meshIdx, speed, viewer) {

}

void ScoreObject::update() {
	if (viewer->getMeshIdCollision() == meshIdx) 
		onSnakeCollide();
	else if (viewer->getLevel() == 1 && viewer->getTick() % 900 == 0)
		 move();
	else if (viewer->getLevel() == 2 && viewer->getTick() % 700 == 0)
		move();
	else if (viewer->getLevel()==3&&viewer->getTick() % 500 == 0)
        move();
	else if (viewer->getLevel() == 4 && viewer->getTick() % 350 == 0)
		move();
	else if (viewer->getLevel() == 5 && viewer->getTick() % 200 == 0)
		move();
}

void  ScoreObject::onSnakeCollide() {
	std::cout << " Snake Collide Score " << std::endl;;
	viewer->increaseProgress();
	viewer->resetMeshIdCollision();
	viewer->soundManager->playGameProgress();
	move();
}


