//
// Created by יובל היטר on 18/02/2023.
//



#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;

class Stopper {
private:
    time_point<system_clock> startTime;
    bool isRunning;
    double elapsedTime;
public:
    Stopper() : isRunning(false), elapsedTime(0) {}
    void start();
    void stop();
    void clear();
    double getElapsedTime();
};


