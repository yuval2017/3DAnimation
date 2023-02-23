#include "Stopper.h"

Stopper::Stopper() : countdown_running(false) {}

void Stopper::start(int seconds) {
    if (countdown_running) {
        return;
    }

    countdown_running = true;
    start_time = std::chrono::system_clock::now();
    end_time = start_time + std::chrono::seconds(seconds);
}

bool Stopper::is_countdown_running() const {
    return countdown_running;
}

void Stopper::reset() {
    countdown_running = false;
}

void Stopper::update_countdown() {
    auto current_time = std::chrono::system_clock::now();
    if (current_time >= end_time) {
        countdown_running = false;
    }
}

