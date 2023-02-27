
#include "Data.h"
#include <fstream>
#include "nlohmann/json.hpp"
#include "GameStatistics.h"
#define LIFE_BOUGHT_NAME            "life_bought"
#define TOTAL_MONEY_NAME            "total_money"
#define OBJECT_COLLISION_NAME       "object_collision"
#define SELF_COLLISION_NAME         "self_collision"
#define DOUBLE_SCORE_NAME           "double_score"
#define SCORE_LEVEL_1_NAME          "scoreLevel1"
#define SCORE_LEVEL_2_NAME          "scoreLevel2"
#define SCORE_LEVEL_3_NAME          "scoreLevel3"
#define MOUSE_NAME                  "0"
#define FROG_NAME                   "1"

using json = nlohmann::json;

Data* Data::instance = 0;

Data* Data::getInstance()
{
    if (instance == 0)
    {
        instance = new Data();
    }

    return instance;
}

Data::Data()
{
    msg = "";
    gameMusic = true;
    gameSound = true;
    musicVolume = 0.5f;
    soundVolume = 0.5f;
    back_to_main.resize(0);
    load_data();
}

void Data::load_data() {


    std::__fs::filesystem::path filePath(fileName);
    //error handling, file not exists or not json file.
    if (!std::__fs::filesystem::exists(filePath)) {
        // Create scores.json if it doesn't exist.
        std::ofstream file(filePath);
        file.close();
    }
    try {
        std::ifstream fileReader(fileName);
        //Avoid parse problems.
        fileReader >> json_data;
        fileReader.close();
    }
    catch (const std::exception &e) {
        std::ofstream fileReader(fileName);
        std::stringstream ss;
        json_data = nlohmann::json::object();
        fileReader << std::setw(4) << json_data;
        fileReader.close();
    }

    checkFileds(json_data);

    std::ifstream file(fileName);
    if (file.is_open()) {
        file >> json_data;

        //validate fields existence.
        total_money = json_data[TOTAL_MONEY_NAME];
        life_bought = json_data[LIFE_BOUGHT_NAME];
        object_collision = json_data[OBJECT_COLLISION_NAME];
        self_collision = json_data[SELF_COLLISION_NAME];
        double_score = json_data[DOUBLE_SCORE_NAME];
        mouse_Scores[1] = json_data[SCORE_LEVEL_1_NAME][MOUSE_NAME];
        mouse_Scores[2] = json_data[SCORE_LEVEL_2_NAME][MOUSE_NAME];
        mouse_Scores[3] = json_data[SCORE_LEVEL_3_NAME][MOUSE_NAME];
        frog_Scores[1] = json_data[SCORE_LEVEL_1_NAME][FROG_NAME];
        frog_Scores[2] = json_data[SCORE_LEVEL_2_NAME][FROG_NAME];
        frog_Scores[3] = json_data[SCORE_LEVEL_3_NAME][FROG_NAME];
        file.close();
    } else {
        // file does not exist or cannot be opened, set default values
        total_money = 0;
        life_bought = 0;
        object_collision = 0;
        self_collision = 0;
        double_score = 0;
        mouse_Scores[1] = 10;
        mouse_Scores[2] = 15;
        mouse_Scores[3] = 20;
        frog_Scores[1] = 10;
        frog_Scores[2] = 15;
        frog_Scores[3] =20;
    }
}


void Data::checkFileds(nlohmann::json json ) {

    bool tm = true;
    bool lb = true;
    bool oc = true;
    bool sc = true;
    bool ds = true;
    bool scoreL1 = true;
    std::ifstream file(fileName);
    if (file.is_open()) {
        if (!json.contains(TOTAL_MONEY_NAME)) {
            // Add "name" field with default value if it doesn't exist
            tm = false;
        }
        if (!json.contains(LIFE_BOUGHT_NAME)) {
            // Add "name" field with default value if it doesn't exist
            lb = false;
        }
        if (!json.contains(OBJECT_COLLISION_NAME)) {
            // Add "name" field with default value if it doesn't exist
            oc = false;
        }
        if (!json.contains(SELF_COLLISION_NAME)) {
            // Add "name" field with default value if it doesn't exist
            sc = false;
        }
        if (!json.contains(DOUBLE_SCORE_NAME)) {
            // Add "name" field with default value if it doesn't exist
            ds = false;
        }
        if (!json.contains(SCORE_LEVEL_1_NAME)) {
            // Add "name" field with default value if it doesn't exist
            scoreL1 = false;
        }
        if (!tm) {
            json.emplace(TOTAL_MONEY_NAME, 0);
        }
        if (!lb) {
            json.emplace(LIFE_BOUGHT_NAME, 0);
        }
        if (!oc) {
            json.emplace(OBJECT_COLLISION_NAME, 0);
        }
        if (!sc) {
            json.emplace(SELF_COLLISION_NAME, 0);
        }
        if (!ds) {
            json.emplace(DOUBLE_SCORE_NAME, 0);
        }
        if (!scoreL1) {
            json.emplace(SCORE_LEVEL_1_NAME, json::object({{"0", 10},{"1", 10}}));
            json.emplace(SCORE_LEVEL_2_NAME, json::object({{"0", 15},{"1", 15}}));
            json.emplace(SCORE_LEVEL_3_NAME, json::object({{"0", 20},{"1", 20}}));
        }
        file.close();
        std::ofstream fileWriter(fileName);
        fileWriter << std::setw(4) << json << std::endl;
        fileWriter.flush();
        fileWriter.close();
    }
}



void Data::save_data()
{
    nlohmann::json data;
    data[TOTAL_MONEY_NAME] = total_money;
    data[LIFE_BOUGHT_NAME] = life_bought;
    data[OBJECT_COLLISION_NAME] = object_collision;
    data[SELF_COLLISION_NAME] = self_collision;
    data[DOUBLE_SCORE_NAME] = double_score;
    data[SCORE_LEVEL_1_NAME][MOUSE_NAME] = mouse_Scores[1];
    data[SCORE_LEVEL_2_NAME][MOUSE_NAME] = mouse_Scores[2];
    data[SCORE_LEVEL_3_NAME][MOUSE_NAME] = mouse_Scores[3];
    data[SCORE_LEVEL_1_NAME][FROG_NAME] = frog_Scores[1];
    data[SCORE_LEVEL_2_NAME][FROG_NAME] = frog_Scores[2];
    data[SCORE_LEVEL_3_NAME][FROG_NAME] = frog_Scores[3];
    std::ofstream file(fileName);
    file << std::setw(4) << data << std::endl;
    file.flush();
    file.close();
}

void Data::dec_life_bought()
{
    life_bought--;
    save_data();
}


void Data::add_total_money(int val)
{
    total_money += val;
    save_data();
}
void Data::sub_total_money(int val)
{
    total_money -= val;
    save_data();
}

void Data::inc_life_bought()
{
    life_bought++;
    sub_total_money(LIFE_COST);
}



void Data::inc_object_collision()
{
    object_collision++;
    sub_total_money(OBJECT_COLLIDE_COST);
}

void Data::dec_object_collision()
{
    object_collision--;
    save_data();
}

void Data::inc_self_collision()
{
    self_collision++;
    sub_total_money(SELF_COLLIDE_COST);
}

void Data::dec_self_collision()
{
    self_collision--;
    save_data();
}

void Data::inc_double_score()
{
    double_score++;
    sub_total_money(DOUBLE_SCORE_COST);
}


void Data::set_message(std::string other)
{
    this->msg = std::move(other);
}



bool Data::checkScore(int mouseScore, int frogScore, int level) {
    if(mouseScore >= mouse_Scores[level] & frogScore >=frog_Scores[level]){
        return true;
    }
    return false;
}

void Data::dec_double_score() {

    if(double_score<0){
        double_score--;
        save_data();
    }
}

