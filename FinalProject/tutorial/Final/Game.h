#pragma once
#include "igl/opengl/glfw/Viewer.h"
#include "igl/opengl/ViewerData.h"
#include "igl/opengl/ViewerCore.h"
#include <time.h>
#include "snake.h"
#include "backgroundHandler.h"
#include "ImGuiMenu.h"
#include "BasicScene.h"

class GameMenu;
class ObjectManager;
class SoundManager;
enum class GameState { PLAY, PAUSED, MENU,LOST,NEXTLEVEL,STORE,WIN };
class Game : public BasicScene {
public:
	Game(igl::opengl::glfw::imgui::ImGuiMenu* menu, SoundManager* soundManager);
	~Game();
	void initMenu();
	void initGame();
	void gameLoop();
	GameState getGameState() { return gameState; }
	void setGameState(GameState state) { gameState = state; }
	void restartLevel();
	void restartGame();
	void increaseLevel() { level++; }
	void increaseMoney() { total_money+=curr_level_money; }
	Snake* getSnake() { return snake; }
	float getProgress() {return score / maxScore;}
	float getHealth() { return health / maxHealth; }
	int getLevelMoney() { return curr_level_money; }
	int getTotalMoney() { return total_money; }
	int getLevel() { return level; }
	float getMaxHealth() { return maxHealth; }
	double getSpeed() { return speed; }
	std::string increaseSpeed();
	std::string increaseMaxHealth();
	std::string refillHealth();
	int getMaxLevel() { return maxLevel; }
	void increaseProgress();
	void increaseHealth();
	void decreaseHealth();
	GameMenu* getGameMenu() { return gameMenu; }
	int getTick() { return tick; }
	int getMeshIdCollision() { return meshIdCollision; }
	void resetMeshIdCollision() { meshIdCollision = -1; }
	void Animate();
	bool shouldMoveSanke;
	SoundManager* soundManager;
	GameState lastState;


private:
	ObjectManager* objectManager;
	GameState gameState;
	GameMenu* gameMenu;
	backgroundHandler* back;
	int level;
	float score;
	int total_money;
	int curr_level_money;
	float health;
	float maxHealth;
	float maxScore;
	double speed;
	int tick;
	int meshIdCollision;
	int maxLevel;

};