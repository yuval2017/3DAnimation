//
// Created by יובל היטר on 18/02/2023.
//



#include <iostream>
#include <chrono>
#include <thread>
#include <utility>
using namespace std;
using namespace std::chrono;

class Stopper {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time_;
    std::chrono::nanoseconds elapsed_time_{0};
    bool is_running_{false};
public:
    Stopper();
    void start();
    void stop();
    void reset();
    void resume();
    std::pair<int, int> ElapsedSecondsAndMilliseconds() const;
    void CountDownFrom(int seconds);
};


