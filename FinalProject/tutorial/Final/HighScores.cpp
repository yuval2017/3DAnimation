
#include "HighScores.h"
#include <sstream>
#include <sys/stat.h>


HighScores::HighScores() {
    this->numOfScoresInTable = 0;
    this->loadHighScores();
}

std::vector <Score> HighScores::getHighScores() {
    return scores;
}
std::vector<int> HighScores::extractScores() {
    std::vector<int> result;
    result.reserve(scores.size());

    for (const auto& s : scores) {
        result.push_back(s.score);
    }

    return result;
}

std::vector<std::string> HighScores::extractPlayerNames() {
    std::vector<std::string> result;
    result.reserve(scores.size());

    for (const auto& s : scores) {
        result.push_back(s.name);
    }

    return result;
}

int HighScores::getHighestScore() {
    return scores[0].score;
}

inline bool fileExists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

void HighScores::loadHighScores() {
    scores.clear();
    std::filesystem::path filePath(fileName);
    if (!std::filesystem::exists(filePath)) {
        // Create scores.json if it doesn't exist.
        std::ofstream file(filePath);
        file.close();
    }
    std::ifstream();
    try {
        std::ifstream fileReader(fileName);
        //avoid parse problems.
        fileReader >> j;
        fileReader.close();
    }
    catch(const std::exception& e) {
        std::ofstream fileReader(fileName);
        std::stringstream ss;
        j =  nlohmann::json::object();
        fileReader << std::setw(4) << j;
        fileReader.close();
    }
    std::ifstream fileReader(fileName);
    ///Now j holds the json info
    fileReader >> j;

    // Check if "numOfScores" field is present in JSON object
    if (!j.contains("numOfScores")) {
        // Add "name" field with default value if it doesn't exist
        j.emplace("numOfScores", 0);
        fileReader.close();
        std::ofstream fileWriter(fileName);
        fileWriter << std::setw(4) << j << std::endl;
        fileWriter.flush();
        fileWriter.close();
        std::ifstream fileReader(fileName);
    }
    numOfScoresInTable = j["numOfScores"];
    for (int i = 0; i < numOfScoresInTable; i++) {

        std::string indexStr = std::to_string(i);
        std::string currentName = j[indexStr]["name"];
        int currentScore = j[indexStr]["score"];
        Score *row = new Score();
        row->name = currentName;
        row->score = currentScore;
        scores.push_back(*row);
    }

    fileReader.close();

}



int sortScores(Score a, Score b) {
    return (a.score - b.score >0);
}

int HighScores::nextLeaderPos()
{
    if (maxScoresInTable > scores.size()){
       return scores.size();
    }
    else{
        return -1;
    }
}

void HighScores::saveToHighScores(Score* score,int index) {
    // check that the file exists.

    std::filesystem::path filePath(fileName);
    if (!std::filesystem::exists(filePath)) {
        // Create scores.json if it doesn't exist.
        std::ofstream file(filePath);
        file.close();
    }
    std::ofstream fileWriter(fileName);
    //We have space starting from numOfScoresInTable
    if(numOfScoresInTable> index){
        replaceScoreAtIndex(index, scores, *score);
    }
    else{
        scores.push_back(*score);
        numOfScoresInTable++;
        std::sort(scores.begin(), scores.end(), sortScores);
    }
    //empty the file.
    j.clear();
    if(numOfScoresInTable > maxScoresInTable){
        numOfScoresInTable = numOfScoresInTable%maxScoresInTable;
    }
    j["numOfScores"] = numOfScoresInTable;
    for (int i = 0; i < numOfScoresInTable; i++) {
        std::string indexStr = std::to_string(i);
        j[indexStr]["name"] = scores[i].name;
        j[indexStr]["score"] = scores[i].score;
    }
    fileWriter << std::setw(4) << j << std::endl;
    fileWriter.flush();
    fileWriter.close();
}

void HighScores::replaceScoreAtIndex(int index, std::vector<Score>& scores, const Score& newScore) {
    scores[index] = newScore;
    std::sort(scores.begin(), scores.end(), sortScores);
}