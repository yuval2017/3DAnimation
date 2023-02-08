#include "movingObject.h"

class HealthObject : movingObject {
public:
	HealthObject(float xCoordinate, float yCoordinate, float zCoordinate, int meshIdx, float speed, Game* viewer);

	void update();
	void onSnakeCollide();


};