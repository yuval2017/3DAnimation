#include "Obstacle.h"
#include "Game.h"
#include "SoundManager.h"

  
Obstacle::Obstacle(float xCoordinate, float yCoordinate, float zCoordinate, int meshIdx, float speed, Game* viewer)
	: movingObject(xCoordinate,yCoordinate,zCoordinate,meshIdx,speed,viewer) {
}

void Obstacle::update() {
	if (viewer->getMeshIdCollision() == meshIdx) {
		onSnakeCollide();
	}
	if (t <= 1 && !moveBackwords) {
		this->t += speed * 5;
		moveAccordingToBeizerCurve();
	}
	else {
		if (!moveBackwords)
			moveBackwords = true; 
		if (moveBackwords) {
			this->t -= speed * 5;
			moveAccordingToBeizerCurve();
			if (t <= 0)
				moveBackwords = false;
		}
	}
} 
void Obstacle::onSnakeCollide() { 
	viewer->decreaseHealth();
	viewer->resetMeshIdCollision();
	viewer->soundManager->playGameHit();

}

