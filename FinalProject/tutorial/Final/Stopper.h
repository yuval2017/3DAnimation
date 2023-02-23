#ifndef STOPPER_H
#define STOPPER_H

#include <chrono>

class Stopper {
public:
    Stopper();
    void start(int seconds);
    bool is_countdown_running() ;
    void reset();

private:

    std::chrono::time_point<std::chrono::system_clock> start_time;
    int seconds;
};

#endif // STOPPER_H
