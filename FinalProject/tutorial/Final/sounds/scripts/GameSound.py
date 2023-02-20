import pygame
from threading import Thread
from time import sleep
import sys

stop_music = True
terminate = False
play_sound = False
is_sound_mute = False
sound_volume_changes = True
game_play_volume_changed = True
sound_number: float = 0.5
game_music_number: float = 0.5
audio_file = "../tutorial/Final/sounds/scripts/music/Game.mp3"
audio_file1 = "../tutorial/Final/sounds/scripts/music/fail.mp3"
audio_file2 = "../tutorial/Final/sounds/scripts/music/Health.mp3"
audio_file3 = "../tutorial/Final/sounds/scripts/music/hit.mp3"
audio_file4 = "../tutorial/Final/sounds/scripts/music/progress.mp3"
audio_file5 = "../tutorial/Final/sounds/scripts/music/success.mp3"
pygame.init()
sound1 = pygame.mixer.Sound(audio_file)
sound2 = pygame.mixer.Sound(audio_file1)
sound3 = pygame.mixer.Sound(audio_file2)
sound4 = pygame.mixer.Sound(audio_file3)
sound5 = pygame.mixer.Sound(audio_file4)
sound6 = pygame.mixer.Sound(audio_file5)
curr_sound = sound2
curr_background = sound1
sounds = {sound2, sound3, sound4, sound5, sound6}
game_musics = {sound1}


# Initialize Pygame

def main():
    global stop_music
    global terminate
    global curr_sound
    global curr_background
    global play_sound
    global is_sound_mute
    global sound1
    global sound2
    global sound3
    global sound4
    global sound5
    global sound6
    global sounds
    global game_musics
    global sound_number
    global game_music_number
    global sound_volume_changes
    global game_play_volume_changed

    curr_sound = sound2
    curr_background = sound1
    thread1 = Thread(target=game_music)
    thread1.start()
    thread2 = Thread(target=game_sound)
    thread2.start()
    thread3 = Thread(target=game_play_music_volume)
    thread3.start()
    thread4 = Thread(target=game_sounds_volume)
    thread4.start()
    while True:
        # this is to play backround
        pip_input = read_pipe_input_line()
        # here is to break the loop
        if pip_input == 'd':
            terminate = True
            pygame.quit()
            thread1
            break
        elif pip_input == 'c':
            stop_music = False
        elif pip_input == 's':
            stop_music = True
        # this is to mute sounds
        elif pip_input == 'v':
            is_sound_mute = False
        elif pip_input == 'b':
            is_sound_mute = True
            # this is all sounds
        elif pip_input == '1':
            curr_sound = sound2
            play_sound = True
        elif pip_input == '2':
            curr_sound = sound3
            play_sound = True
        elif pip_input == '3':
            curr_sound = sound4
            play_sound = True
        elif pip_input == '4':
            curr_sound = sound5
            play_sound = True
        elif pip_input == '5':
            curr_sound = sound6
            play_sound = True
        # here is to change the game sound
        elif pip_input == 'q':
            play_sound = sound1
        elif pip_input[0:1] == 'n':
            sound_number = float(pip_input[1:6])
            sound_volume_changes = True
        elif pip_input[0:1] == 'm':
            game_music_number = float(pip_input[1:6])
            game_play_volume_changed = True



def game_sound():
    global curr_sound
    global play_sound
    global is_sound_mute
    global terminate
    while not terminate:
        if play_sound and not is_sound_mute:
            play_sound = False
            curr_sound.play()


def game_music():
    # Set the audio file name
    global stop_music
    global terminate
    one_time_stop_music = True
    one_time_play = True
    while not terminate:
        if stop_music:
            if one_time_stop_music:
                curr_background.stop()
                one_time_play = True
                one_time_stop_music = False
        else:
            if one_time_play:
                curr_background.play(loops=-1)
                one_time_play = False
                one_time_stop_music = True
    # Quit Pygame


def read_pipe_input():
    global terminate
    while not terminate:
        char = sys.stdin.read(1)
        if char:
            break
    return char
    # Do something with the input here


def read_pipe_input_line():
    global terminate
    while not terminate:
        line = sys.stdin.readline().strip()
        if line:
            break
    return line


def game_sounds_volume():
    global terminate
    global sounds
    global sound_volume_changes
    while not terminate:
        if sound_volume_changes:
            sound_volume_changes = False
            for s in sounds:
                s.set_volume(sound_number)


def game_play_music_volume():
    global terminate
    global game_musics
    global game_play_volume_changed
    while not terminate:
        if game_play_volume_changed:
            game_play_volume_changed = False
            for g in game_musics:
                g.set_volume(game_music_number)


if __name__ == "__main__":
    main()
