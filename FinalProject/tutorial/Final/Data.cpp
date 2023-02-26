
#include "Data.h"
#include <fstream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "GameStatistics.h"
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

void Data::load_data()
{
    std::ifstream file("data.json");
    if (file.is_open()) {
        json data;
        file >> data;
        total_money = data["total_money"];
        life_bought = data["life_bought"];
        object_collision = data["object_collision"];
        self_collision = data["self_collision"];
        double_score = data["double_score"];
        scores[1] = data["scoreLevel1"];
        scores[2] = data["scoreLevel2"];
        scores[3] = data["scoreLevel3"];
        file.close();
    }
    else {
        // file does not exist or cannot be opened, set default values
        total_money = 0;
        life_bought = 0;
        object_collision = 0;
        self_collision = 0;
        double_score = 0;
    }
}

void Data::save_data()
{
    json data;
    data["total_money"] = total_money;
    data["life_bought"] = life_bought;
    data["object_collision"] = object_collision;
    data["self_collision"] = self_collision;
    data["double_score"] = double_score;
    data["scoreLevel1"] = scores[1];
    data["scoreLevel2"] = scores[2];
    data["scoreLevel3"] = scores[3];
    std::ofstream file("data.json");
    if (file.is_open()) {
        file << data;
        file.close();
    }
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

void Data::restart_game()
{
    this->msg = "";
}


const char* Data::msg_c_str() const
{
    return this->msg.c_str();
}

int Data::message_size()
{
    return this->msg.size();
}

bool Data::checkScore(int score, int level) {
    if(score >= scores[level]){
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

