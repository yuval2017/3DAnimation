#include "movingObject.h"

class Obstacle : movingObject {
public:
	Obstacle(float xCoordinate, float yCoordinate, float zCoordinate, int meshIdx, float speed, Game* viewer);
	void update();
	void onSnakeCollide();
};