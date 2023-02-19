//
// Created by יובל היטר on 10/02/2023.
//
#pragma once
#include <iostream>
#define MainMenu_OP                 0
#define StoreMenu_OP                1
#define PauseMenu_OP                2
#define SettingsMenu_OP             3
#define LeadersMenu_OP              4
#define LevelMenu_OP                5
#define GameOverMenu_OP             6
#define WinMenu_OP                  7
#define SPEED_COST                  50
#define LIFE_COST                   30


using namespace std;

class Data {
public:



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
    bool menu_flags[10] = {false, false,false, false,false, false,false, false,false, false};
    bool back_to_main=false;
    int get_life();
    void set_life(int l);
    float get_progress();
    void set_Num_of_levels(int n);
    int get_Num_Of_Levels();
    std::string inc_speed();
    std::string add_life();
    int get_back_to();
    void set_back(int val);
    void restart_game();
    void init_sound_engine();
    static Data* getInstance();
    bool sound;
private :
    static Data* instance;
    Data();
    std::string msg;
    int level;
    const int num_of_levels =3;
    int score ;
    int life ;
    int speed;
    Data(Data const&);              // Don't Implement.
    void operator=(Data const&); // Don't implement
};



