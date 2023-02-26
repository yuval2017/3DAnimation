import pygame



# Initialize Pygame
pygame.init()

# Set the audio file name
audio_file = "scripts/music/Game.mp3"

# Load the audio file
pygame.mixer.music.load(audio_file)

# Play the audio file
pygame.mixer.music.play()

# Keep the program running until the audio finishes playing
while pygame.mixer.music.get_busy():
    continue

# Quit Pygame
pygame.quit()