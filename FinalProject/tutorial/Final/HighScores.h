#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "json.hpp"
#include <string> 
#include <filesystem>

typedef struct {
	std::string name;
	int score;
} Score;

class HighScores {
public:
	HighScores();
	int nextLeaderPos();
	void saveToHighScores ( Score* score, int index);
	void loadHighScores();
	int getHighestScore();
	std::vector <Score> getHighScores();
    std::vector<std::string> extractPlayerNames();
    std::vector<int> extractScores();


private :

	const char* fileName ="../tutorial/Final/scores.json";
	nlohmann::json j;
    int maxScoresInTable = 5;
	int numOfScoresInTable;
	std::vector <Score> scores;
    void replaceScoreAtIndex(int index, std::vector<Score> &scores, const Score &newScore);


};