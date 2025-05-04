import pygame
import sys

def play_tone(frequency=440):
    pygame.mixer.init()
    sample_rate = 44100
    n_samples = sample_rate  # 1-second buffer
    buf = pygame.sndarray.make_sound(
        (32767 * pygame.sndarray.array(
            [pygame.sndarray.array([pygame.math.sin(2.0 * pygame.math.pi * frequency * t / sample_rate) for t in range(n_samples)])],
            dtype='int16'
        )).astype('int16')
    )
    buf.play(-1)  # Play indefinitely

if __name__ == "__main__":
    frequency = int(sys.argv[1]) if len(sys.argv) > 1 else 440
    play_tone(frequency)
