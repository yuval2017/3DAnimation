//
// Created by יובל היטר on 10/02/2023.
//
#pragma once
#include <iostream>
#define MainMenu                    0
#define StoreMenu                   1
#define PauseMenu                   2
#define SettingsMenu                3
#define LeadersMenu                 4
#define LevelMenu                   5
#define LooseMenu                   6
#define GameOverMenu                7
#define SPEED_COST                  50
#define LIFE_COST                   30


using namespace std;

class Data {
public:


    Data();
    void inc_Level();
    void dec_Level();
    int get_score();
    void set_message(std::string other);
    void add_Score(int s);
    void dec_Score(int s);
    int get_speed();
    void reset_Speed();
    int message_size();
    const char * msg_c_str();
    int get_level();
    char characterName[256] = "";
    bool menu_flags[10] = {false};
    int get_life();
    void set_life(int l);
    float get_progress();
    void set_Num_of_levels(int n);
    int get_Num_Of_Levels();
    std::string inc_speed();
    std::string add_life();
    void restart_game();


private :
    std::string msg;
    int level;
    const int num_of_levels =3;
    int score ;
    int life ;
    int speed;
};


