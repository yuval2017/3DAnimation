#ifndef STOPPER_H
#define STOPPER_H

#include <chrono>

class Stopper {
public:
    Stopper();
    void start(int seconds);
    bool is_countdown_running() ;
    void reset();
    void stop();
    void startAfterStop();

private:

    std::chrono::time_point<std::chrono::system_clock> start_time;
    int seconds;
    int second_remain_after_stop = 0;
    bool in_stop;
};

#endif // STOPPER_H
