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
    usleep(1000);
    fclose(pipe);
}
void SoundManager::send_to_pipe(const std::string& to_send){
    std::cout<< "sent to pipe : "<< to_send + "\n" << std::endl;
    fputs((to_send + "\n").c_str() , pipe);
    fflush(pipe);
}
void SoundManager::change_game_music(const int num){
    send_to_pipe("@"+std::to_string(num));
}
void SoundManager::stop_game_music(){
    if (game_music_on) {
        game_music_on = false;
        send_to_pipe(std::string(STOP_BACKGROUND_MUSIC));
    }
}
void SoundManager::play_game_music(){

        sound_on = true;
        game_music_on = true;
        send_to_pipe(std::string(START_BACKGROUND_MUSIC));
}
void SoundManager::play_sound(const std::string& sound){
    if(sound_on) {
        send_to_pipe(sound);
    }
}

void SoundManager::stop_all_game_sounds() {
    if(sound_on) {
        sound_on = false;
    }
}

void SoundManager::restart_game_sounds() {
    if(!sound_on) {
        sound_on = true;
    }
}
void SoundManager::set_sounds_volume(std::string new_vulume){
    send_to_pipe(std::string(SOUNDS_VOLUME) + new_vulume);
}
void SoundManager::set_game_play_music(std::string new_vulume){
    send_to_pipe(std::string(GAME_PLAY_MUSIC_VOLUME) + new_vulume);
}