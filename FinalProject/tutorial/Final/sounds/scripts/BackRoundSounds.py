import pygame
from threading import Thread
from time import sleep
import sys
stop_music = True
terminate = False
audio_file1 = "../tutorial/Final/sounds/scripts/music/fail.mp3"
audio_file2 = "../tutorial/Final/sounds/scripts/music/Health.mp3"
audio_file3 = "../tutorial/Final/sounds/scripts/music/hit.mp3"
audio_file4 = "../tutorial/Final/sounds/scripts/music/progress.mp3"
audio_file5 = "../tutorial/Final/sounds/scripts/music/success.mp3"



# Initialize Pygame

def main():
    global stop_music
    global terminate
    pygame.init()

    thread = Thread(target=game_music)
    thread.start()
    while not terminate:
        read_pipe_input()
        stop_music = False

        read_pipe_input()
        stop_music = True
    pygame.quit()

def game_music():
    # Set the audio file name
    global audio_file

    # Load the audio file
    pygame.mixer.music.load(audio_file)

    # Play the audio file
    pygame.mixer.music.play()

    # Keep the program running until the audio finishes playing
    one_time_play = True
    while not terminate:
        if stop_music:
            pygame.mixer.music.stop()
            one_time_play = True
        else:
            if one_time_play:
                pygame.mixer.music.play()
                one_time_play = False
            else:
                if not pygame.mixer.music.get_busy():
                    pygame.mixer.music.play()
    # Quit Pygame


def read_pipe_input():
    global terminate
    while not terminate:
        char = sys.stdin.read(1)
        if char:
            if char == 'd':
                terminate = True
            break
    return terminate
        # Do something with the input here

def read_pipe_input_line():
    while True:
        line = sys.stdin.readline().strip()
        if line:
            break
    return line

if __name__ == "__main__":
    main()



