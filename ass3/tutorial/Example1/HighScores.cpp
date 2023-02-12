
#include <sys/stat.h>
#include "HighScores.h"

HighScores::HighScores(char* fileName) {
    this->fileName = fileName;
    this->maxScoresInTable = 3;
    this->numOfScoresInTable = 0;
    this->grabCurrentHighScores();
}

std::vector <std::string> HighScores::getTopHighScoreNames() {
    std::vector <std::string> names;
    numOfScoresInTable = highscoreRows.size();
    names.resize(numOfScoresInTable);
    for (int i = 0; i < numOfScoresInTable; i++)
        names[i] = highscoreRows[i].name;
    return names;
}
std::vector <int> HighScores::getTopHighScores() {
    std::vector <int> scores;
    numOfScoresInTable = highscoreRows.size();
    scores.resize(numOfScoresInTable);
    for (int i = 0; i < numOfScoresInTable; i++)
        scores[i] = highscoreRows[i].score;
    return scores;
}

int HighScores::getHighestScore() {
    return highscoreRows[0].score;
}

inline bool fileExists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

void HighScores::grabCurrentHighScores() {
    highscoreRows.clear();
    if (fileExists(fileName))
    {
        std::ifstream fileReader(fileName);
        fileReader >> j;
        ///Now j holds the json info
        numOfScoresInTable = j["numOfScores"];

        for (int i = 0; i < numOfScoresInTable; i++)
        {

            std::string indexStr = std::to_string(i);
            std::string currentName = j[indexStr]["name"];
            int         currentScore = j[indexStr]["score"];

            Score* row = new Score();
            row->name = currentName;
            row->score = currentScore;
            highscoreRows.push_back(*row);
        }
    }
}

int sortScores(Score a, Score b) {
    return (a.score - b.score < 0);
}


void HighScores::saveToHighScores(std::string playerName, int score) {
    if (fileExists(fileName)) {
        this->grabCurrentHighScores();
        /*High Scores File Exists*/

        bool hasPassedLimit = true;
        if (highscoreRows.size() < maxScoresInTable)
            //We can add another entry since table is still not full
            hasPassedLimit = false;

        std::ofstream fileWriter(fileName);
        if (!hasPassedLimit) {
            //We have space starting from numOfScoresInTable
            Score* newRow = new Score();
            newRow->score = score;
            newRow->name = playerName;

            highscoreRows.push_back(*newRow);
            std::sort(highscoreRows.begin(), highscoreRows.end(), sortScores);

            j["numOfScores"] = highscoreRows.size();
            for (int i = 0; i < highscoreRows.size(); i++) {
                std::string indexStr = std::to_string(i);
                j[indexStr]["name"] = highscoreRows[i].name;
                j[indexStr]["score"] = highscoreRows[i].score;
            }
            fileWriter << std::setw(4) << j << std::endl;
            fileWriter.flush();
            fileWriter.close();
        }
        else {
            //We don't have space
            Score* newRow = new Score();
            newRow->score = score;
            newRow->name = playerName;

            std::vector<Score> rowsTemp;
            for (int i = 0; i < highscoreRows.size(); i++)
                rowsTemp.push_back(highscoreRows[i]);

            rowsTemp.push_back(*newRow);
            std::sort(rowsTemp.begin(), rowsTemp.end(), sortScores);

            j["numOfScores"] = highscoreRows.size();
            for (int i = 0; i < highscoreRows.size(); i++) {
                std::string indexStr = std::to_string(i);
                j[indexStr]["name"] = rowsTemp[i + 1].name;
                j[indexStr]["score"] = rowsTemp[i + 1].score;
            }
            fileWriter << std::setw(4) << j << std::endl;
            fileWriter.flush();
            fileWriter.close();
        }
    }
    else
    {
        std::ofstream fileWriter(fileName);
        j["numOfScores"] = 1;
        j[std::to_string(0)]["score"] = score;
        j[std::to_string(0)]["name"] = playerName;
        fileWriter << std::setw(4) << j << std::endl;
        fileWriter.flush();
        fileWriter.close();
    }
}
