//
// Created by יובל היטר on 18/02/2023.
//

#include "Stopper.h"
void Stopper::start() {
    if (!isRunning) {
        isRunning = true;
        startTime = system_clock::now();
    }
}

void Stopper::stop() {
    if (isRunning) {
        isRunning = false;
        elapsedTime += duration<double>(system_clock::now() - startTime).count();
    }
}

void Stopper::clear() {
    isRunning = false;
    elapsedTime = 0;
}

double Stopper::getElapsedTime() {
    if (isRunning) {
        return elapsedTime + duration<double>(system_clock::now() - startTime).count();
    } else {
        return elapsedTime;
    }
}