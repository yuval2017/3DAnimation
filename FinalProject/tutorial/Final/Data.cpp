//
// Created by יובל היטר on 10/02/2023.
//

#include "Data.h"
#include "iostream"
#include <fstream>
#include <string>
#include <sstream>
#define TOTAL_MONEY "total_money"
#define LIFE_BOUGHT "life_bought"
#define OBJECT_COLLISION "object_collision"
#define SELF_COLLISION "self_collision"
#define DOUBLE_SCORE "double_score"

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
    sound = true;
    back_to_main = false;
    init_vals();
    //handler = new jsonHandler();

}
void Data::init_vals() {

    fstream newfile;
    newfile.open("data.txt",ios::in);
    if (newfile.is_open()){ //checking whether the file is open
        string tp;
        while(getline(newfile, tp)){ //read data from file object and put it into string.
            string arr[2];
            stringstream ssin(tp);
            int i=0;
            while ( getline( ssin, arr[i++], '=' ) ) {}
            put_val(arr[0],arr[1]);
        }
        newfile.close(); //close the file object.
    }
//    init_double_score();
//    init_life_bought();
//    init_object_collision();
//    init_self_collision();
//    init_total_money();
}

void Data::put_val(std::string s, std::string val) {
    if( s.compare(TOTAL_MONEY) == 0){
        total_money = std::stoi(val);
    }
    else  if( s.compare(LIFE_BOUGHT) == 0){
        life_bought = std::stoi(val);
    }
    else  if( s.compare(OBJECT_COLLISION) == 0){
        object_collision = std::stoi(val);
    }
    else  if( s.compare(SELF_COLLISION) == 0){
        self_collision = std::stoi(val);
    }
    else  if( s.compare(DOUBLE_SCORE) == 0){
        double_score= std::stoi(val);
    }

}

int Data::get_back_to(){
    return this->back_to_main;
}
std::string Data::inc_life_bought(){

    //handler->saveToData("life_bought", life_bought+1) ;
    life_bought ++;
    return "Life added successfully!";
}

void Data::init_double_score(){
    //double_score = handler->get_double_score();
}
void Data::init_object_collision(){
   // object_collision = handler->get_object_collision();
}
void Data::init_self_collision(){
   // self_collision = handler->get_self_collision();
}
void Data::init_total_money(){
    //total_money = handler->get_total_money();
}
void Data::init_life_bought(){
   // life_bought = handler->get_life_bought();
}


void Data::set_message(std::string other) {
    this->msg = std::move(other);
}

void Data::restart_game() {

    this->msg="";
}

std::string Data::get_message(){
    return this->msg;
}

const char * Data::msg_c_str(){
    return this->msg.c_str();
}
void Data::set_back(int val){
    this->back_to_main= val;
}

int Data::message_size(){
    return this->msg.size();
}