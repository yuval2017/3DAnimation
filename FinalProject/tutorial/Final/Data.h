
#pragma once

#include <string>
#include "nlohmann/json.hpp"
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <filesystem>

class Data {
public:
    static Data* getInstance();

    //getters and setters.

    const char* msg_c_str() const;
    void set_message(std::string other);
    void set_total_money(int money);

    //adders and increases.
    void add_total_money(int val);
    void inc_life_bought();
    void inc_object_collision();
    void inc_self_collision();
    void inc_double_score();
    int message_size();
    void restart_game();
    void dec_life_bought();
    int total_money;
    //game variables.
    bool gameMusic ;
    bool gameSound;
    float musicVolume ;
    float soundVolume ;
    std::vector<int> back_to_main;
    int mouse_Scores[4];
    int frog_Scores[4];
    bool checkScore(int mouseScore, int frogScore, int level);
    void sub_total_money(int val);
    void dec_object_collision();
    void dec_self_collision();
    void dec_double_score();
    int life_bought;
    int object_collision;
    int self_collision;
    int double_score;



private:
    void load_data();
    void save_data();
    void checkFileds(nlohmann::json json);
    static Data* instance;
    std::string msg;
    const char* fileName = "../tutorial/Final/data.json";
    nlohmann::json json_data;
    Data();
    ~Data();


};
