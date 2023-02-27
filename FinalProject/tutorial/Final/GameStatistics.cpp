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

GameStatistics::GameStatistics() {

    level = 1;
    speed = 1.0;
    score = 0;
    mousesNum = 0;
    frogsNum = 0;
    double_score = false;
    levelUp = false;
    data = Data::getInstance();
    soundManager = SoundManager::getInstance();
}

void GameStatistics::reset_game() {

    level = 1;
    speed = 1.0;
    score = 0;
    mousesNum = 0;
    frogsNum = 0;
    double_score = false;
    levelUp = false;
    selfCollisionStopper.reset();
    objectCollisionStopper.reset();
}

void GameStatistics::inc_Score(int mouse, int frog, int coin) {


    frogsNum+=frog;
    mousesNum+=mouse;
    int scor = 0;
    scor += (mouse*MOUSE_PRICE)+ (frog*FROG_PRICE) + (coin*COIN_PRICE);
    if(double_score){
        scor*=2;
    }
    score+=scor;
    if(data->checkScore(mousesNum,frogsNum,level)) {
        if (level != num_of_levels) {
            levelUp = true;
            menu_flags[LevelMenu_OP] = true;
            soundManager->play_sound(std::to_string(PROGRESS_SOUND));

        } else {
            won = true;
            soundManager->play_sound(std::to_string(SUCCESS_SOUND));
        }
    }
}
