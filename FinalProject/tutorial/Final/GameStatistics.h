//
// Created by Bar damri on 19/02/2023.
//

#ifndef ENGINEREWORK_GAMESTATISTICS_H
#define ENGINEREWORK_GAMESTATISTICS_H
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
#define PlayMenu_OP                 8
#define LoadingMenu_OP              9
#define SPEED_COST                  50
#define LIFE_COST                   30
#define FROG_PRICE                  10
#define MOUSE_PRICE                 50
#define COIN_PRICE                  100

#include "Stopper.h"
#include "Data.h"
using namespace std;

class GameStatistics {

public:
    static GameStatistics* getInstance();
    char characterName[256] = "";
    bool menu_flags[11] = {false, false,false, false,false, false,false, false,false,false, false};
    bool back_to_main=false;
    //Stopper timer;
    const int num_of_levels =3;
    int level;
    int score ;
    int num_of_strikes;
    int strikes_used;
    bool double_score;
    float speed ;
    void reset_level_data();
    float get_progress();
    bool levelUp;
    bool restart;
    void reset_game();
    Stopper* selfCollisionStopper;
    Stopper* objectCollisionStopper;

    float loadingProgress();

    void inc_Score(int i);

private :
    Data* data ;
    static GameStatistics* instance;
    GameStatistics();
    GameStatistics(GameStatistics const&);              // Don't Implement.
    void operator=(GameStatistics const&); // Don't implement



};


#endif //ENGINEREWORK_GAMESTATISTICS_H
