//
// Created by יובל היטר on 10/02/2023.
//

#include "Data.h"
#include "iostream"


Data* Data::instance = 0;

Data* Data::getInstance()
{
    if (instance == 0)
    {
        instance = new Data();
    }

    return instance;
}



Data::Data() {
    msg = "";
    level = 1;
    speed =1;
    score = 0;
    life = 0 ;
    sound = true;
}



void Data::dec_Level() {

    this->level--;
}

void Data::set_message(std::string other) {
    this->msg = std::move(other);
}

void Data::add_Score(int s) {
    this->score+=s;
}

void Data::inc_Level() {
    this->level++;
}

int Data::message_size() {
    return msg.size();
}

const char * Data::msg_c_str(){
    return this->msg.c_str();
}

int Data::get_level() {
    return this->level;
}

int Data::get_score() {
    return this->score;
}

void Data::dec_Score(int s) {
    this->score-=s;
}

int Data::get_life() {
    return this->life;
}

void Data::set_life(int l) {
    this->life = l;
}

float Data::get_progress() {
    return this->level/this->num_of_levels;
}

int Data::get_Num_Of_Levels() {
    return this->num_of_levels;
}

std::string Data::add_life() {
    if (this->score>= LIFE_COST){
        this->life ++;
        this->score -= LIFE_COST;

    } else{
        return "you have not enough score to add a life.";
    }
}
std::string Data::inc_speed(){
    if (this->score>= SPEED_COST){
        this->speed ++;
        this->score -= SPEED_COST;

    } else{
        return "you have not enough score to purchase speed.";
    }
}

int Data::get_speed() {
    return this->speed;
}

void Data::reset_Speed() {

    this->speed=1;
}
int Data::get_back_to(){
    return this->back_to_main;
}
void Data::set_back(int val){
    this->back_to_main= val;
}
void Data::restart_game() {

    this->score=0;
    this->level=1;
    this->speed=1;
    this->life=0;
    this->msg="";
}





