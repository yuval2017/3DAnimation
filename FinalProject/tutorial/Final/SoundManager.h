#include <string>
#include "../../irrKlang-64bit-1.5.0/include/ik_ISoundEngine.h"


using namespace irrklang;

class SoundManager {
public:
	SoundManager();
	
	void playGameSound();
	void playGameNextLevel();
	void playGameHit();
	void playGameLose();
	void playGameProgress();
	void playGameHealth();
	ISoundEngine * engine;


};