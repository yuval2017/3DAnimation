#include "SoundManager.h"

#include <utility>



std::string python_exe = "python3.10.app";

SoundManager* SoundManager::instance = 0;

SoundManager* SoundManager::getInstance()
{
    if (instance == 0)
    {
        instance = new SoundManager();
    }

    return instance;
}
SoundManager::SoundManager() {
    python_exe = "python3.10.exe";
    run = new bool(true);
    // Build the command to execute the Python script with the given parameter
    std::string command = "python3.10 ../tutorial/Final/sounds/scripts/GameSound.py";

    // Open a pipe to the command process for input and output
    pipe = popen(command.c_str(), "r+");
    if (!pipe) {
        std::cerr << "Error: Could not open pipe to Python process." << std::endl;
        return;
    }

}
SoundManager::~SoundManager(){
    drop();
}
void SoundManager::drop(){
    send_to_pipe("d");
    pclose(pipe);
}
void SoundManager::send_to_pipe(std::string to_send){
    fputs(to_send.c_str(), pipe);
    fflush(pipe);
}
void SoundManager::change_game_music(std::string new_music){
    send_to_pipe(std::move(new_music));
}
void SoundManager::stop_game_music(){
    send_to_pipe(std::string(STOP_BACKGROUND_MUSIC));
}
void SoundManager::play_game_music(){
    send_to_pipe(std::string(START_BACKGROUND_MUSIC));
}
void SoundManager::play_sound(int sound){
    send_to_pipe(std::to_string(sound));
}

void SoundManager::stop_all_game_sounds() {
    send_to_pipe(std::string(STOP_SOUNDS));
}

void SoundManager::restart_game_sounds() {
    send_to_pipe(std::string(RESTART_SOUNDS));
}
