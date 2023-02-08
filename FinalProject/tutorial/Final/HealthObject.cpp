#include "HealthObject.h"
#include "Game.h"
#include "SoundManager.h"

HealthObject::HealthObject(float xCoordinate, float yCoordinate, float zCoordinate, int meshIdx, float speed, Game* viewer)
  : movingObject(xCoordinate,yCoordinate,zCoordinate, meshIdx, speed,viewer){
}

void HealthObject::update() {
	if (viewer->getMeshIdCollision() == meshIdx) 
		onSnakeCollide();
	else if (viewer->getLevel() == 1 && viewer->getTick() % 800 == 0)
		move();
	else if (viewer->getLevel() == 2 && viewer->getTick() % 600 == 0)
		move();
	else if (viewer->getLevel() == 3 && viewer->getTick() % 450 == 0)
		move();
	else if (viewer->getLevel() == 4 && viewer->getTick() % 350 == 0)
		move();
	else if (viewer->getLevel() == 5 && viewer->getTick() % 250 == 0)
		move();
}

void  HealthObject::onSnakeCollide() {
	std::cout << " Snake Collide Health " << std::endl;;
	viewer->increaseHealth();
	viewer->resetMeshIdCollision();
	viewer->soundManager->playGameHealth();
	move();
}


