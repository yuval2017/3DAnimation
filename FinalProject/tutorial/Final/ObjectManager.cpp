#include "ObjectManager.h"
#include "Game.h"


ObjectManager::ObjectManager(Game* viewer) {

	this->numSnakeLinks = 16;

	/*
	* Since snake is 1 and has 16 links, first object after that is 17 and we must account for that.
	*/

	this->currentMeshId = this->numSnakeLinks + 1;
	this->viewer = viewer;
	viewer->next_data_id = 17;
	this->initSpawnLocations();
}

void ObjectManager::initSpawnLocations() {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<int> distributeZ(1, 5);
	std::uniform_int_distribution<int> distribute(1, 7);
	for (int i = 0; i < 30; i++)
		spawnLocations.push_back(Eigen::Vector3d(distribute(rng), distribute(rng), distributeZ(rng)));
}

void ObjectManager::addMovingObstacles(int numberOfObstacles) {
	for (int i = 0; i < numberOfObstacles; i++)
		addMovingObstacle();
}


int ObjectManager::addSnakeHead() {
	viewer->load_mesh_from_file("C:\\Users\\sabin\\Downloads\\EngineForAnimationCourse\\tutorial\\data\\snake2.obj", "C:\\Users\\sabin\\Downloads\\EngineForAnimationCourse\\tutorial\\textures\\snake.png");
	viewer->data_list.back().show_overlay_depth = true;
	viewer->data_list.back().show_texture = 3;
	viewer->data_list.back().line_width = 2;
	viewer->data_list.back().set_visible(false, 1);
	viewer->data_list.back().set_visible(true, 2);

	viewer->data_list.back().show_faces = 3;
	viewer->data_list.back().SetCenterOfRotation(((Game*)viewer)->getSnake()->getJointPos(15));
	viewer->data_list.back().MyTranslate(((Game*)viewer)->getSnake()->getJointPos(15), false);
	((Game*)viewer)->initTree(currentMeshId);
	viewer->parents.push_back(-1);
	int id = currentMeshId;
	currentMeshId++;
	return id;

}
int ObjectManager::addObject(std::string pathOfObject, std::string  pathOfTexture) {
	std::random_device dev;
	std::mt19937       rng(dev());
	std::uniform_int_distribution<int> distribute(1, spawnLocations.size()-1);

	int  randSpawnIndex = distribute(rng); 
	bool a= viewer->load_mesh_from_file(pathOfObject, pathOfTexture);
	//std::cout << viewer->data_list.size() << std::endl;
	viewer->data_list.back().show_overlay_depth = true;
	viewer->data_list.back().show_texture = 3;
	viewer->data_list.back().line_width = 2;
	viewer->data_list.back().set_visible(false, 1); 
	viewer->data_list.back().set_visible(true, 2);
	viewer->data_list.back().id = currentMeshId;
	viewer->data_list.back().show_faces = 3;
	viewer->data_list.back().MyTranslate(spawnLocations[randSpawnIndex], false);
	((Game*)viewer)->initTree(currentMeshId);
	viewer->parents.push_back(-1);
	return randSpawnIndex;
}

void ObjectManager::addMovingObstacle() {
	int spawnIndex = addObject("C:\\Users\\sabin\\Downloads\\EngineForAnimationCourse\\tutorial\\data\\cube.obj", "C:\\Users\\sabin\\Downloads\\EngineForAnimationCourse\\tutorial\\textures\\bricks.png");
	Obstacle* obstacle = new Obstacle(spawnLocations[spawnIndex][0], spawnLocations[spawnIndex][1], spawnLocations[spawnIndex][2], currentMeshId, 0.0005, viewer);
	movingObstacles.push_back(obstacle);
	currentMeshId++;
}

void ObjectManager::reset()
{
	currentMeshId = 17;
	movingObstacles.clear();
	healthObjects.clear();
	scoreObjects.clear();

}

void ObjectManager::addHealthObjects(int numOfHealthObjects) {
	for (int i = 0; i < numOfHealthObjects; i++)
		addHealthObject();
}

void ObjectManager::addHealthObject() {

	int spawnIndex = addObject("C:\\Users\\sabin\\Downloads\\EngineForAnimationCourse\\tutorial\\data\\sphere.obj", "C:\\Users\\sabin\\Downloads\\EngineForAnimationCourse\\tutorial\\textures\\grass.png");
	HealthObject* healthObject = new HealthObject(spawnLocations[spawnIndex][0], spawnLocations[spawnIndex][1], spawnLocations[spawnIndex][2], currentMeshId, 0.0005, viewer);
	healthObjects.push_back(healthObject);
	currentMeshId++;
}
void ObjectManager::addScoreObjects(int numOfScoreObjects) {
	for (int i = 0; i < numOfScoreObjects; i++)
		addScoreObject();
}
void ObjectManager::addScoreObject() {
	int spawnIndex = addObject("C:\\Users\\sabin\\Downloads\\EngineForAnimationCourse\\tutorial\\data\\sphere.obj", "C:\\Users\\sabin\\Downloads\\EngineForAnimationCourse\\tutorial\\textures\\red.png");
	ScoreObject* scoreObject = new ScoreObject(spawnLocations[spawnIndex][0], spawnLocations[spawnIndex][1], spawnLocations[spawnIndex][2], currentMeshId, 0.0005, viewer);
	scoreObjects.push_back(scoreObject);
	currentMeshId++;
}

void ObjectManager::updateObjects() {
	for (int i = 0; i < movingObstacles.size(); i++)
		movingObstacles[i]->update();
	for (int i = 0; i < healthObjects.size(); i++)
		healthObjects  [i]->update();
	for (int i = 0; i < scoreObjects.size(); i++)
		scoreObjects   [i]->update();
}