
#include "Data.h"
#include <fstream>
#include <sstream>
#include "nlohmann/json.hpp"

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
    back_to_main = false;
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

    std::ofstream file("data.json");
    if (file.is_open()) {
        file << data;
        file.close();
    }
}

void Data::set_total_money(int val)
{
    total_money = val;
    save_data();
}

void Data::add_total_money(int val)
{
    total_money += val;
    save_data();
}

int Data::get_total_money() const
{
    return total_money;
}

void Data::set_life_bought(int val)
{
    life_bought = val;
    save_data();
}

void Data::inc_life_bought()
{
    life_bought++;
    save_data();
}

int Data::get_life_bought() const
{
    return life_bought;
}

void Data::set_object_collision(int val)
{
    object_collision = val;
    save_data();
}

void Data::inc_object_collision()
{
    object_collision++;
    save_data();
}

int Data::get_object_collision() const
{
    return object_collision;
}

void Data::set_self_collision(int val)
{
    self_collision = val;
    save_data();
}

void Data::inc_self_collision()
{
    self_collision++;
    save_data();
}

int Data::get_self_collision() const
{
    return self_collision;
}

void Data::set_double_score(int val)
{
    double_score = val;
    save_data();
}

void Data::inc_double_score()
{
    double_score++;
    save_data();
}

int Data::get_double_score() const
{
    return double_score;
}

void Data::set_message(std::string other)
{
    this->msg = std::move(other);
}

void Data::restart_game()
{
    this->msg = "";
}

std::string Data::get_message() const
{
    return this->msg;
}

const char* Data::msg_c_str() const
{
    return this->msg.c_str();
}

void Data::set_back(int val)
{
    this->back_to_main = val;
}

int Data::get_back_to_main()
{
    return this->back_to_main;
}

int Data::message_size()
{
    return this->msg.size();
}

