#include "Stopper.h"
#include "iostream"

// Get the minimum duration for the system clock
static const auto min_duration = std::chrono::system_clock::duration::min();

// Create a time point with the minimum value
static const auto min_time = std::chrono::time_point<std::chrono::system_clock>(min_duration);

void Stopper::start(int sec) {
    if (!is_countdown_running()) {
        start_time = std::chrono::system_clock::now();
        seconds = sec;
    }

}

bool Stopper::is_countdown_running()  {
    if(std::chrono::duration_cast<std::chrono::seconds>(start_time- min_time).count() ==0)
        return false;
    int val = seconds - std::chrono::duration_cast<std::chrono::seconds> (std::chrono::system_clock::now() - start_time).count() ;
    if(val <= 0){
        reset();
        return false;
    }
    return true;
}

void Stopper::reset() {
    seconds = 0 ;
    start_time = min_time;
}

Stopper::Stopper() {

    seconds = 0;
    start_time = min_time;
}

