#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "json.hpp"
#include <string> 


typedef struct {
	std::string name;
	int         score;
} rowHighScores;

class HighScores {
public:
	HighScores(char* fileName);
	
	void saveToHighScores (std::string playerName, int score);
	void grabCurrentHighScores();

	std::vector<rowHighScores>& getHighScoreRows() {
		return highscoreRows;
	}
	int getHighestScore();

	std::vector <std::string> getTopHighScoreNames();
	std::vector <int>         getTopHighScores();
private :
	char* fileName; 
	nlohmann::json j;
    int maxScoresInTable;
	int numOfScoresInTable;
	std::vector <rowHighScores> highscoreRows;
};