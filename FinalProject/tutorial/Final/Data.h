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

    void set_message(std::string other);
    std::string get_message();
    int message_size();
    bool back_to_main ;
    static Data* getInstance();
    void restart_game();
    void set_back(int val);
    const char * msg_c_str();
    bool sound;
    std::string inc_life_bought();
    int get_back_to();
    int total_money;
    void init_double_score();
    void init_object_collision();
    void init_self_collision();
    void init_total_money();
    void init_life_bought();
    void put_val(string s , string val);
    void init_vals();
private :
    //singleton methods.
    static Data* instance;
    Data(Data const&);              // Don't Implement.
    void operator=(Data const&); // Don't implement

    //methods

    Data();
    //properties
    std::string msg;
    int life_bought;
    int object_collision;
    int self_collision;
    int double_score;

};



