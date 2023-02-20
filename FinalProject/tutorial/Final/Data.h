
#pragma once

#include <string>
#include "nlohmann/json.hpp"

class Data {
public:
    static Data* getInstance();

    //getters and setters.

    std::string get_message() const;
    const char* msg_c_str() const;
    void set_message(std::string other);
    int get_total_money() const;
    void set_total_money(int money);
    int get_life_bought() const;
    void set_life_bought(int life);
    int get_object_collision() const;
    void set_object_collision(int collision);
    int get_self_collision() const;
    void set_self_collision(int collision);
    int get_double_score() const;
    void set_double_score(int score);

    //adders and increases.
    void add_total_money(int val);
    void inc_life_bought();
    void inc_object_collision();
    void inc_self_collision();
    void inc_double_score();
    void set_back(int val);
    int get_back_to_main();
    int message_size();
    void restart_game();


    //game variables.
    bool soundOn ;
    float volume ;
    bool back_to_main;

private:
    void load_data();
    void save_data();

    static Data* instance;
    std::string msg;
    int total_money;
    int life_bought;
    int object_collision;
    int self_collision;
    int double_score;
    nlohmann::json json_data;

    Data();
    ~Data();
};
