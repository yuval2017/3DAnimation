#pragma once
#include <string>
#include "thread"
#include <assert.h>
#include <chrono>
#include <future>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#define FAIL_SOUND 1
#define HEALTH_SOUND 2
#define HIT_SOUND 3
#define PROGRESS_SOUND 4
#define GAME_MUSIC q
#define START_BACKGROUND_MUSIC "c"
#define STOP_BACKGROUND_MUSIC "s"
#define STOP_SOUNDS "b"
#define RESTART_SOUNDS "v"
#define SOUNDS_VOLUME "n"
#define GAME_PLAY_MUSIC_VOLUME "m"
class SoundManager {
public:

    static SoundManager* getInstance();
    void drop();
    std::thread python_thread;
    std::string id ;
    ~SoundManager();
    void play_sound(const std::string& sound);
    void change_game_music(const std::string& new_music);
    void stop_game_music();
    void play_game_music();
    void stop_all_game_sounds();
    void restart_game_sounds();
    void switch_game_music(std::string new_game_music);
    void set_sounds_volume(std::string new_vulume);
    void set_game_play_music(std::string new_vulume);

private:
    void send_to_pipe(const std::string& to_send);
    bool* run;
    SoundManager();
    static SoundManager* instance;
    SoundManager(SoundManager const&);              // Don't Implement.
    void operator=(SoundManager const&); // Don't implement
    FILE* pipe;
    bool sound_on = true;
    bool game_music_on = false;

};