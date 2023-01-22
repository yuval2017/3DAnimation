#pragma once
#include "igl/opengl/glfw/Viewer.h"
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include "Game.h"
#include "HighScores.h"

#include <iostream>
#include <set>
class GameMenu {
public:
	GameMenu(Game* game, igl::opengl::glfw::imgui::ImGuiMenu* menu);
	~GameMenu();
	void init();
	void startGameMenu(std::vector<igl::opengl::ViewerCore>& core);
	void startGameMenuPaused(std::vector<igl::opengl::ViewerCore>& core);
	void startGameMenuPlay(std::vector<igl::opengl::ViewerCore>& core);
	void startGameMenuNextLevel(std::vector<igl::opengl::ViewerCore>& core);
	void startGameMenuWin(std::vector<igl::opengl::ViewerCore>& core);
	void startGameMenuLose(std::vector<igl::opengl::ViewerCore>& core);
	void startGameStore(std::vector<igl::opengl::ViewerCore>& core);
private:
	char characterName[256] = "";
	Game* game;
	igl::opengl::glfw::imgui::ImGuiMenu* menu;
	std::string msg = "";
	HighScores* highScores;

};

