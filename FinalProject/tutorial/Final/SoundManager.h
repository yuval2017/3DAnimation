#include <string>
#include "thread"
#include <assert.h>
#include <chrono>
#include <future>


class SoundManager {
public:

    static SoundManager* getInstance();
	void playGameSound();
	void playGameNextLevel();
	void playGameHit();
	void playGameLose();
	void playGameProgress();
	void playGameHealth();
    void drop();
    void pauseSound();
    void continueSound();
    std::thread python_thread;
    std::string id ;

private:
    bool* run;
    SoundManager();
    static SoundManager* instance;
    SoundManager(SoundManager const&);              // Don't Implement.
    void operator=(SoundManager const&); // Don't implement
};