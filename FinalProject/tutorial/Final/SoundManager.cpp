#include "SoundManager.h"
#include "../../irrKlang-64bit-1.5.0/include/irrKlang.h"

SoundManager::SoundManager() {
    engine = createIrrKlangDevice();

    if (!engine)
        return;
}

void SoundManager::playGameSound() {
    engine->play2D("./tutorial/music/Game.mp3", true);
}
void SoundManager::playGameNextLevel () {
    engine->play2D("./tutorial/music/success.mp3");
}
void SoundManager::playGameHit() {
    engine->play2D("./tutorial/music/hit.mp3");
}
void SoundManager::playGameLose() {
    engine->play2D("./tutorial/music/fail.mp3");
}
void SoundManager::playGameHealth() {
    engine->play2D("./tutorial/music/Health.mp3");
}
void SoundManager::playGameProgress() {
    engine->play2D("./tutorial/music/progress.mp3");
}