import pygame
from threading import Thread
from time import sleep
import sys
stop_music = True
terminate = False
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
    audio_file = "../tutorial/Final/sounds/scripts/music/Game.mp3"

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
    # Quit Pygame


def read_pipe_input():
    global terminate
    while not terminate:
        char = sys.stdin.read(1)
        if char:
            if char == 'd':
                terminate = True
            break
        # Do something with the input here


if __name__ == "__main__":
    main()



