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
    strikes_used = 0 ;
    double_score = false;
    levelUp = false;
    selfCollisionStopper->reset();
    objectCollisionStopper->reset();
}

void GameStatistics::inc_Score(int i) {

    if(double_score){
        i=i*2;
    }
    score+=i;
    if(data->checkScore(score,level)) {
        if (level != num_of_levels) {
            levelUp = true;
            menu_flags[LevelMenu_OP] = true;

        } else {
            restart = true;
        }
    }
}

void GameStatistics::inc_speed() {

    speed++;
    data->sub_total_money(SPEED_COST);
}
