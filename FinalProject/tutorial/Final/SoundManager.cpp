#include "SoundManager.h"



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
    fputs("d", pipe);
    fflush(pipe);
    pclose(pipe);
}
void SoundManager::drop(){
    fputs("d", pipe);
    fflush(pipe);
    pclose(pipe);
}
void SoundManager::pauseSound(){
    fputs("c", pipe);
    fflush(pipe);

}
void SoundManager::continueSound(){
    fputs("c", pipe);
    fflush(pipe);
}

void SoundManager::playGameSound() {


// Calling a script to play the music
    const auto& PlayMusic = [&]() {
        std::string command = "python ../tutorial/Final/sounds/scripts/GameSound.py";
        system(command.c_str());
    };
    python_thread = std::thread (PlayMusic);

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