#include "Game.h"
#include "igl/opengl/glfw/Viewer.h"
#include "igl/opengl/ViewerData.h"
#include <time.h>
#include "sandBox.h"
#include "snake.h"
#include "GameMenu.h"
#include "ObjectManager.h"
#include "backgroundHandler.h"
#include "SoundManager.h"
#include "Game.h"


Game::Game(std::string name, Display *display, igl::opengl::glfw::imgui::ImGuiMenu *menu,
           SoundManager *soundManager) : BasicScene(name, display), level(0), score(0.0), maxScore(100.0),
                                         health(100.0), maxHealth(100.0), tick(0), meshIdCollision(-1),
                                         shouldMoveSanke(false), total_money(0), curr_level_money(0), maxLevel(5),
                                         speed(0.04) {
	 snake         = new Snake             (this, 16);
	 this->soundManager = soundManager;
	 gameMenu      = new GameMenu(this,menu);
	 objectManager = new ObjectManager     (this);
	 this->soundManager->playGameSound();

	 gameState     = GameState::MENU; 
	 direction     = Eigen::Vector3d(0, 0, 0.04);
	 Translate(Eigen::Vector3d(0, 0, -10), true);
	 initGame();

}
void Game::initGame() {
	std::cout << "Current Level : "<< level << std::endl;
	if (level == 0) { 
		std::cout << "Initated Snake & Background (Level 0)" << std::endl;
		snake->initSnake("configuration.txt");

	}
	else if (level == 1) {
		health = 100;
		score = 0;
		curr_level_money = 0;
		std::cout << "Level1 Initated.. " << std::endl;
		objectManager->addHealthObjects(3);
		objectManager->addScoreObjects(5);
		objectManager->addMovingObstacles(2);
	
	}
	else if (level == 2) {
		restartLevel();
		std::cout << "Level2 Initated.. " << std::endl;
		objectManager->addMovingObstacles(2);
	}
	else if (level == 3) {
		restartLevel();
		std::cout << "Level3 Initated.. " << std::endl;
		objectManager->addMovingObstacles(2);
		for (int i = 0; i < data_list.size(); i++) {
			std::cout << data_list[i].id << std::endl;
		}
	}
	else if (level == 4) {
		restartLevel();
		std::cout << "Level4 Initated.. " << std::endl;
		objectManager->addMovingObstacles(2);
		for (int i = 0; i < data_list.size(); i++) {
			std::cout << data_list[i].id << std::endl;
		}
	}
	else if (level == 5) {
		restartLevel();
		std::cout << "Level5 Initated.. " << std::endl;
		objectManager->addMovingObstacles(2);
		for (int i = 0; i < data_list.size(); i++) {
			std::cout << data_list[i].id << std::endl;
		}
	}
}
void Game::restartLevel(){
	tick = 0;
	health = maxHealth;
	score = 0;
	shouldMoveSanke = false;
	curr_level_money = 0;
	snake->restartSnake();
}

void Game::restartGame()
{
	tick = 0;
	maxHealth = 100;
	speed = 0.04;
	total_money = 0;
	shouldMoveSanke = false;
	curr_level_money = 0;
	health = 100;
	score = 0;
	level = 1;
	objectManager->reset();
	selected_data_index = data_list.size() - 1;
	  while(erase_mesh(selected_data_index)){
		  selected_data_index = data_list.size() - 1;
		  data(selected_data_index).clear();
	  }
	 data().clear();

	 snake->initSnake("configuration.txt");


}

Game::~Game(){
	delete gameMenu;
	delete snake;
	delete objectManager;

}

void Game::initMenu(){
	gameMenu->init();
	
}

void Game::gameLoop() {
	tick++;
	objectManager->updateObjects();
	if (shouldMoveSanke) {
        snake->skinnig(direction);
    }
	
	int size = data_list.size();
	for (int i = 0; i < 16; i++) {
		for (int j = 17; j < size; j++) {
			if (thereIsCollisionSingleTree(snake->snakeJointBoxes[i], &trees[j], i+1, j)) {
				meshIdCollision = j;
				std::cout << "Collision!!!!!  " <<"joint "<<i<<" with "<< meshIdCollision << std::endl;
				break;
			}

	}
}
}

void Game::Animate() {
	if(gameState==GameState::PLAY)
     gameLoop();
}
std::string Game::increaseSpeed()
{
	if (total_money >= 50) {
		if (speed < 0.055) {
			speed += 0.005;
			total_money -= 50;
			soundManager->playGameProgress();
			return "Successful";
		}
		else {
			soundManager->playGameLose();
			return "Sorry your at the maximum speed";;
		}
	}
	soundManager->playGameLose();
	return "Sorry you dont have enough money :(";
}
std::string Game::increaseMaxHealth(){
	if (total_money >= 100) {
		if (maxHealth < 300) {
			maxHealth = maxHealth + 50;
			health = maxHealth;
			total_money -= 100;
			soundManager->playGameHealth();
			return "Successful";
		}
		else {
			soundManager->playGameLose();
			return "Sorry your at the maximum health";;
		}
	}
	soundManager->playGameLose();
	return "Sorry you dont have enough money :(";
}
std::string Game::refillHealth(){
	if (total_money >= 30) {
		health=maxHealth;
		total_money -= 30;
		soundManager->playGameHealth();
		return "Successful";
	}
	soundManager->playGameLose();
	return "Sorry you dont have enough money :(";
}
void Game::increaseProgress() {
	std::cout << "increaseProgress() called, Decreasing Score Remaining!" << std::endl;
	score += 10.0;
	curr_level_money += 5;
	if (score == 100) {
		soundManager->playGameNextLevel();
		if(level==maxLevel)
			gameState = GameState::WIN;
		else
		gameState = GameState::Nextlevel;
	}
}
void Game::increaseHealth()
{
	std::cout << "increaseHealth() Called" << std::endl; 
	curr_level_money += 5;
	if (health <100)
	health += 10.0; 
}
void Game::decreaseHealth()
{
	std::cout << "decreaseHealth() Called" << std::endl;
	if (health > 0)
		health -= 1.0;
	if (health <= 0) {
		soundManager->playGameLose();
		gameState = GameState::LOST;
		
	}
}
