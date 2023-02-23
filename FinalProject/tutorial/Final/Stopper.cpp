//
// Created by יובל היטר on 18/02/2023.
//

#include "Stopper.h"
Stopper::Stopper() {}

void Stopper::start() {
    start_time_ = std::chrono::high_resolution_clock::now();
    is_running_ = true;
}

void Stopper::stop() {
    end_time_ = std::chrono::high_resolution_clock::now();
    is_running_ = false;
}

void Stopper::reset() {
    elapsed_time_ = std::chrono::nanoseconds::zero();
    start_time_ = std::chrono::time_point<std::chrono::high_resolution_clock>();
    end_time_ = std::chrono::time_point<std::chrono::high_resolution_clock>();
    is_running_ = false;
}

void Stopper::resume() {
    if (is_running_) {
        return;
    }

    start_time_ = std::chrono::high_resolution_clock::now() - elapsed_time_;
    is_running_ = true;
}
//second and miilisecond
std::pair<int, int> Stopper::ElapsedSecondsAndMilliseconds() const {
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    if (is_running_) {
        end_time = std::chrono::high_resolution_clock::now();
    } else {
        end_time = end_time_;
    }

    auto elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_).count();
    auto seconds = static_cast<int>(elapsed_milliseconds / 1000);
    auto milliseconds = static_cast<int>(elapsed_milliseconds % 1000);

    return std::make_pair(seconds, milliseconds);
}

void Stopper::CountDownFrom(int seconds) {
    for (int i = seconds; i >= 0; i--) {
        std::cout << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}