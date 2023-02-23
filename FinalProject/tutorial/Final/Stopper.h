#ifndef STOPPER_H
#define STOPPER_H

#include <chrono>

class Stopper {
public:
    Stopper();
    Stopper(const Stopper&) = delete;
    Stopper& operator=(const Stopper&) = delete;

    void start(int seconds);
    bool is_countdown_running() const;
    void reset();

private:
    bool countdown_running;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> end_time;

    void update_countdown();
};

#endif // STOPPER_H
