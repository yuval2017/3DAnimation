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
//to change game music by given string
void SoundManager::switch_game_music(const std::string& new_game_music){
    send_to_pipe(new_game_music);
}
//to chane game music by given int
void SoundManager::change_game_music(const int num){
    send_to_pipe("@" + std::to_string(num));
}
void SoundManager::send_to_pipe(const std::string& to_send){
    if (!pipe) {
        std::cerr << "Error: Could not open pipe to Python process." << std::endl;
        return;
    }
    fputs((to_send + "\n").c_str() , pipe);
    fflush(pipe);
}
//stop game music
void SoundManager::stop_game_music(){
    if (game_music_on) {
        game_music_on = false;
        send_to_pipe(std::string(STOP_BACKGROUND_MUSIC));
    }
}
//play game music
void SoundManager::play_game_music(){
    if (!game_music_on) {
        game_music_on = true;
        send_to_pipe(std::string(START_BACKGROUND_MUSIC));
    }
}
//play sound
void SoundManager::play_sound(const std::string& sound){
    if(sound_on) {
        send_to_pipe(sound);
    }
}
//stop all game sounds
void SoundManager::stop_all_game_sounds() {
    if(sound_on) {
        sound_on = false;
    }
}
//restart all game sounds.. init them
void SoundManager::restart_game_sounds() {
    if(!sound_on) {
        sound_on = true;
    }
}
//change the volume
void SoundManager::set_sounds_volume(const std::string& new_vulume){
    send_to_pipe(std::string(SOUNDS_VOLUME) + new_vulume);
}
//change the volume
void SoundManager::set_game_play_music_volume(const std::string& new_vulume){
    send_to_pipe(std::string(GAME_PLAY_MUSIC_VOLUME) + new_vulume);
}