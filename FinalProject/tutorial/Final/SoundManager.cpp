#include "SoundManager.h"
#include <string>


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
}

void SoundManager::drop(){
    system("pkill -f ../tutorial/Final/sounds/scripts/GameSound.py");
}
void SoundManager::pauseSound(){

    system("killall python");

}
void SoundManager::continueSound(){
    playGameSound();
}

void SoundManager::playGameSound() {


// Calling a script to play the music
    const auto& PlayMusic = [&]() {
        std::string command = "python ../tutorial/Final/sounds/scripts/GameSound.py";
        system(command.c_str());
    };
    python_thread = std::thread (PlayMusic);
    python_thread.detach();

}
void SoundManager::playGameNextLevel () {
	//engine->play2D("./tutorial/music/success.mp3");
}
void SoundManager::playGameHit() {
	//engine->play2D("./tutorial/music/hit.mp3");
}
void SoundManager::playGameLose() {
	//engine->play2D("./tutorial/music/fail.mp3");
}
void SoundManager::playGameHealth() {
	//engine->play2D("./tutorial/music/Health.mp3");
}
void SoundManager::playGameProgress() {
	//engine->play2D("./tutorial/music/progress.mp3");
}