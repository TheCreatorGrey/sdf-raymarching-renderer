import pygame
import sys
import math
import numpy as np

pygame.init()

width, height = 800, 600
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption("Pixel Drawer")


def euler_to_vector(euler):
    roll, pitch, yaw = euler
    x = math.cos(yaw) * math.cos(pitch)
    y = math.sin(yaw) * math.cos(pitch)
    z = math.sin(pitch)

    return x, y, z


voxels = np.ones((4, 4, 4))


def ray(rotation, origin, sliceLength, slices):
    vector = euler_to_vector(rotation)
    current = origin
    for s in range(slices):
        for v in voxels:
            voxpos = [
                round(current[0]),
                round(current[1]),
                round(current[2])
            ]

            if voxpos[0] in voxels:
                if voxpos[1] in voxels[voxpos[0]]:
                    if voxpos[2] in voxels[voxpos[0]][voxpos[1]]:
                        return s #voxels[voxpos[0]][voxpos[1]][voxpos[2]]

        current[0] += vector[0]*sliceLength
        current[1] += vector[1]*sliceLength
        current[2] += vector[2]*sliceLength

print(ray([0, 1, 0], [0, 0, 0], .1, 4))

running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    
    screen.fill((0, 0, 0))

    for y in range(200):
        for x in range(200):
            cast = ray([0, 1, 2], [x/width, y/height, 0], .1, 10)
            if cast:
                pygame.draw.rect(screen, (200-(cast*10), 50, 50), (x, y, 1, 1))
    
    pygame.display.flip()

# Quit Pygame
pygame.quit()
sys.exit()