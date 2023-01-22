#include "GameMenu.h"
#include "Game.h"
#include <igl/opengl/ViewerCore.h>
#include <charconv>
#include <iostream>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include "HighScores.h"


GameMenu::GameMenu(Game* game, igl::opengl::glfw::imgui::ImGuiMenu* menu) {
	this->game = game;
	this->menu = menu;
	highScores = new HighScores("Highscores.ini");
	

}
GameMenu::~GameMenu()
{
	delete highScores;
}
void GameMenu::init(){
}
void GameMenu::startGameMenu(std::vector<igl::opengl::ViewerCore>& core) { 
		int imageWidth = 0;
		int imageHeight = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(300, 800), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSizeConstraints(ImVec2(300, -1.0f), ImVec2(325, -1.0f));

		ImGui::Begin("Snake Game - Animation Class", nullptr,  ImGuiWindowFlags_NoMove);
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		std::vector <std::string> names = highScores->getTopHighScoreNames();
		std::vector <int>         scores = highScores->getTopHighScores();

		if (names.size() == 0) {
			ImGui::BulletText("No Saved Scores!");
		}
		else {
			for (int i = names.size()-1; i >=0; i--) {
				std::string text = names[i] + " | " + std::to_string(scores[i]) + "\n";
				ImGui::BulletText(text.c_str());
			}
		}
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::InputText("Name", characterName, IM_ARRAYSIZE(characterName));
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		if (game->getLevel() > 0) {
			if (ImGui::Button("Continue game", ImVec2(-1, 0)))
			{
				game->setGameState(game->lastState);
			}
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
			if (ImGui::Button("New Game", ImVec2(-1, 0))) {
				game->restartGame();
				game->initGame();
				game->setGameState(GameState::PLAY);
			}
		
		}
		else if (ImGui::Button("New Game", ImVec2(-1, 0))) {
			std::cout << "here" << std::endl;
				game->increaseLevel();
				game->initGame();
				game->setGameState(GameState::PLAY);
			}
		
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		if (ImGui::Button("Tutorial", ImVec2(-1, 0)))
		{
			if (msg.size() == 0)
				msg = "press the up,down,right,left,w(out)\ns(in) keys to move around.\nGreen balls are health and red are\nscore each of them will add 5 coins\nto your money.\nDont collide with the moving boxes.";
			else
				msg = "";
		}
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text(msg.c_str());
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::BulletText("Snake Game By Sabina & Ron Rachev\nAnimation Class\n");
		ImGui::End();
	
}

void GameMenu::startGameMenuPaused(std::vector<igl::opengl::ViewerCore>& core)
{
	int imageWidth = 0;
	int imageHeight = 0;
	
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 800), ImGuiSetCond_FirstUseEver);
	
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, -1.0f), ImVec2(325, -1.0f));

	ImGui::Begin("Snake Game - Animation Class", nullptr,  ImGuiWindowFlags_NoMove);
	ImGui::Text(characterName);
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::ProgressBar(game->getProgress(), ImVec2(0.0f, 0.0f));
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Progress Bar");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::ProgressBar(game->getHealth(), ImVec2(0.0f, 0.0f));
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Health Bar");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Current level money ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	std:: string tmp = std::to_string(game->getLevelMoney());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Text("Total money ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	tmp = std::to_string(game->getTotalMoney());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Text("Snake speed ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	tmp = std::to_string(game->getSpeed());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Text("max health ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	tmp = std::to_string(game->getMaxHealth());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Continue", ImVec2(-1, 0)))
	{
		game->setGameState(GameState::PLAY);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Store", ImVec2(-1, 0)))
	{
		msg = "";
		game->setGameState(GameState::STORE);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Back to menu", ImVec2(-1, 0)))
	{
		game->lastState = GameState::PAUSED;
		msg = "";
		game->setGameState(GameState::MENU);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::BulletText("Snake Game By Sabina & Ron Rachev\nAnimation Class\n");
	ImGui::End();

}

void GameMenu::startGameMenuPlay(std::vector<igl::opengl::ViewerCore>& core){ 
	int imageWidth = 0;
	int imageHeight = 0;
	
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 800), ImGuiSetCond_FirstUseEver);
	
	ImGui::SetNextWindowSizeConstraints(ImVec2(300, -1.0f), ImVec2(325, -1.0f));

	ImGui::Begin("Snake Game - Animation Class", nullptr, ImGuiWindowFlags_NoMove);
	ImGui::Text(characterName);
	ImGui::Spacing();
	ImGui::Text("current level ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	std::string tmp = std::to_string(game->getLevel());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::ProgressBar(game->getProgress(), ImVec2(0.0f, 0.0f));
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Progress Bar");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::ProgressBar(game->getHealth(), ImVec2(0.0f, 0.0f));
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Health Bar");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Current level money ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	 tmp = std::to_string(game->getLevelMoney());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Text("Total money ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    tmp = std::to_string(game->getTotalMoney()); 
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Text("Snake speed ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	tmp = std::to_string(game->getSpeed());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Text("max health ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	tmp = std::to_string(game->getMaxHealth());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Pause", ImVec2(-1, 0)))
	{
		game->setGameState(GameState::PAUSED);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Store", ImVec2(-1, 0)))
	{
		msg = "";
		game->setGameState(GameState::STORE);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Restart level", ImVec2(-1, 0)))
	{
		game->restartLevel();
		game->setGameState(GameState::PLAY);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Back to menu", ImVec2(-1, 0)))
	{
		game->lastState = GameState::PLAY;
		game->setGameState(GameState::MENU);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	auto make_checkbox = [&](const char* label, unsigned int& option)
	{
		return ImGui::Checkbox(label,
			[&]() { return core[1].is_set(option); },
			[&](bool value) { return core[1].set(option, value); }
		);
	};
	if (ImGui::CollapsingHeader("Overlays", ImGuiTreeNodeFlags_DefaultOpen))
	{
		make_checkbox("Wireframe", game->data().show_lines);
		make_checkbox("Fill", game->data().show_faces);
		make_checkbox("Show texture", game->data().show_texture);
		make_checkbox("Show overlay", game->data().show_overlay);
		make_checkbox("Show overlay depth", game->data().show_overlay_depth);
	}
	ImGui::BulletText("Snake Game By Sabina & Ron Rachev\nAnimation Class\n");
	ImGui::End();
}

void GameMenu::startGameMenuNextLevel(std::vector<igl::opengl::ViewerCore>& core) {
	int imageWidth = 0;
	int imageHeight = 0;

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 800), ImGuiSetCond_FirstUseEver);

	ImGui::SetNextWindowSizeConstraints(ImVec2(300, -1.0f), ImVec2(325, -1.0f));

	ImGui::Begin("Snake Game - Animation Class", nullptr, ImGuiWindowFlags_NoMove);
	ImGui::Text(characterName);
	ImGui::Spacing();
	ImGui::Text("Congratulations You passed level");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	std::string tmp = std::to_string(game->getLevel());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Text("In this level you gained");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	 tmp = std::to_string(game->getLevelMoney());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Text("So the total money is ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    tmp = std::to_string(game->getTotalMoney()+ game->getLevelMoney());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Next level", ImVec2(-1, 0)))
	{
		game->increaseLevel();
		game->increaseMoney();
		highScores->saveToHighScores(characterName, game->getTotalMoney());
		game->initGame();
		game->setGameState(GameState::PLAY);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Play again", ImVec2(-1, 0)))
	{
		highScores->saveToHighScores(characterName, game->getTotalMoney());
		game->restartLevel();
		game->setGameState(GameState::PLAY);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Back to menu", ImVec2(-1, 0)))
	{
		game->lastState = GameState::NEXTLEVEL;
		game->increaseMoney();
		highScores->saveToHighScores(characterName, game->getTotalMoney());
		game->setGameState(GameState::MENU);
	}
	ImGui::BulletText("Snake Game By Sabina & Ron Rachev\nAnimation Class\n");
	ImGui::End();
}
void GameMenu::startGameMenuWin(std::vector<igl::opengl::ViewerCore>& core) {
	int imageWidth = 0;
	int imageHeight = 0;

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 800), ImGuiSetCond_FirstUseEver);

	ImGui::SetNextWindowSizeConstraints(ImVec2(300, -1.0f), ImVec2(325, -1.0f));

	ImGui::Begin("Snake Game - Animation Class", nullptr, ImGuiWindowFlags_NoMove);
	ImGui::Text(characterName);
	ImGui::Spacing();
	ImGui::Text("Congratulations You ended the game!!");
	ImGui::Spacing();
	ImGui::Text("In this level you gained");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	std::string tmp = std::to_string(game->getLevelMoney());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Text("So the your total score is ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	tmp = std::to_string(game->getTotalMoney() + game->getLevelMoney());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Play again", ImVec2(-1, 0)))
	{
		highScores->saveToHighScores(characterName, game->getTotalMoney());
		game->restartLevel();
		game->setGameState(GameState::PLAY); 
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("new game", ImVec2(-1, 0)))
	{
		game->increaseMoney();
		highScores->saveToHighScores(characterName, game->getTotalMoney());
		game->restartGame();
		game->initGame();
		game->setGameState(GameState::PLAY);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Back to menu", ImVec2(-1, 0)))
	{
		game->lastState = GameState::WIN;
		game->increaseMoney();
		highScores->saveToHighScores(characterName, game->getTotalMoney());
		game->setGameState(GameState::MENU);
	}
	ImGui::BulletText("Snake Game By Sabina & Ron Rachev\nAnimation Class\n");
	ImGui::End();
}

void GameMenu::startGameMenuLose(std::vector<igl::opengl::ViewerCore>& core) {
	int imageWidth = 0;
	int imageHeight = 0;

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 800), ImGuiSetCond_FirstUseEver);

	ImGui::SetNextWindowSizeConstraints(ImVec2(300, -1.0f), ImVec2(325, -1.0f));

	ImGui::Begin("Snake Game - Animation Class", nullptr, ImGuiWindowFlags_NoMove);
	ImGui::Text(characterName);
	ImGui::Spacing();
	ImGui::Text("ohhh you died...");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Play again", ImVec2(-1, 0)))
	{
		game->restartLevel();
		game->setGameState(GameState::PLAY);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Back to menu", ImVec2(-1, 0)))
	{
		game->lastState = GameState::LOST;
		game->setGameState(GameState::MENU);
	}
	ImGui::BulletText("Snake Game By Sabina & Ron Rachev\nAnimation Class\n");
	ImGui::End();
}

void GameMenu::startGameStore(std::vector<igl::opengl::ViewerCore>& core) {
	int imageWidth = 0;
	int imageHeight = 0;

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(270, 800), ImGuiSetCond_FirstUseEver);

	ImGui::SetNextWindowSizeConstraints(ImVec2(300, -1.0f), ImVec2(325, -1.0f));

	ImGui::Begin("Snake Game - Animation Class", nullptr, ImGuiWindowFlags_NoMove);
	ImGui::Text(characterName);
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text(" welcome to the store");
	ImGui::Spacing();
	ImGui::Text("the total money you have ");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	std::string tmp = std::to_string(game->getTotalMoney());
	ImGui::Text(tmp.c_str());
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("extra speed - 50 coins", ImVec2(-1, 0))){
		msg = game->increaseSpeed();
		
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("refill health - 30 coins", ImVec2(-1, 0))){
		msg = game->refillHealth();
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("increase maximum health - 100 coins", ImVec2(-1, 0))) {
		msg = game->increaseMaxHealth();
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Back to game", ImVec2(-1, 0)))
	{
		game->lastState = GameState::STORE;
		game->setGameState(GameState::PLAY);
	}
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text(msg.c_str());
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::BulletText("Snake Game By Sabina & Ron Rachev\nAnimation Class\n");
	ImGui::End();
}
