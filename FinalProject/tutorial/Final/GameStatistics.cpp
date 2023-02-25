//
// Created by Bar damri on 19/02/2023.
//

#include "GameStatistics.h"
GameStatistics* GameStatistics::instance = 0;

GameStatistics* GameStatistics::getInstance()
{
    if (instance == 0)
    {
        instance = new GameStatistics();
    }

    return instance;
}


float GameStatistics::get_progress(){
    return score/data->scores[level];
}

GameStatistics::GameStatistics() {

    level = 1;
    speed = 1.0;
    score = 0;
    num_of_strikes = 0 ;
    strikes_used = 0 ;
    double_score = false;
    levelUp = false;
    selfCollisionStopper = new Stopper();
    objectCollisionStopper = new Stopper();
    data = Data::getInstance();
}

void GameStatistics::reset_game() {

    level = 1;
    speed = 1.0;
    score = 0;
    num_of_strikes = 0 ;
    strikes_used = 0 ;
    double_score = false;
    levelUp = false;
}

void GameStatistics::inc_Score(int i) {

    score+=i;
    if(data->checkScore(score,level)){
        levelUp= true;
        menu_flags[LevelMenu_OP] = true;
    }
}
