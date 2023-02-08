#include "MovingObject.h"

class ScoreObject : movingObject {

public:
	ScoreObject(float xCoordinate, float yCoordinate, float zCoordinate, int meshIdx, float speed, Game* viewer);

	void update();
	void onSnakeCollide();



};