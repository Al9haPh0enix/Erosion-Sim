## Initialization ##
import random
import math
import glob
from PIL import Image
import os

import pygame
pygame.init()

## Settings ##
movement_const = 5
width = 600
height = 600
background = (50, 127, 50)
radius = 4
alignment = 0
gravity = 2
num_of_sediment = 2000

## Dist function ##
def dist(x1, y1, x2, y2):
    return math.sqrt((x1-x2)**2 + (y1-y2)**2)

## Particle class ##
class Particle:
    def __init__(self, x, y):
        self.x = x
        self.y = y

## Sediment particle class ##
class sediment(Particle):
    def __init__(self, x, y, radius):
        super().__init__(x, y)
        self.radius = radius
    def move(self, angle, movement_const, vx, vy):
        self.x += (math.cos(angle * (math.pi/180)) / self.radius * movement_const) * vx/5
        self.y -= (math.sin(angle * (math.pi/180)) / self.radius * movement_const) * vy/5

## Water particle class ##
class water(Particle):
    def __init__(self, x, y, angle):
        super().__init__(x, y)
        self.angle = angle
        self.velx = 0
        self.vely = 0

    def check(self, sedimentL):
        for ind in range(len(sedimentL)):
            i = sedimentL[ind]
            if dist(self.x + math.cos(self.angle * (math.pi/180)), self.y - math.sin(self.angle * (math.pi/180)), i.x, i.y) < i.radius:
                sedimentL[ind].move(self.angle + random.randint(-2, 2), movement_const, self.velx, self.vely)
                self.angle += 5 if random.randint(0, 1) == 1 else -5
                self.vely *= .75
                self.velx *= .9 if random.randint(0, 1) == 1 else -.9

    def move(self, sedL, watL):

        self.velx += random.randint(-1, 1)
        self.vely += random.randint(-1, 1)

        while self.angle > 360:
            self.angle -= 360
        while self.angle < 0:
            self.angle += 360

        self.vely += gravity

        for i in watL:
            if dist(self.x, self.y, i.x, i.y) < 10:
                if i.angle < self.angle:
                    self.angle -= alignment
                if i.angle > self.angle:
                    self.angle += alignment

        self.velx += math.cos(self.angle * (math.pi/180))
        self.vely -= math.sin(self.angle * (math.pi/180))
        self.check(sedL)

        self.x += self.velx / 7.5
        self.y += self.vely / 7.5

        self.velx *= .99
        self.vely *= .99

## Variables ##
x = width/2
y = height/2
angle = 45
clock = pygame.time.Clock()
wats = [water(x, y, random.randint(0, 360)) for i in range(10)]
seds = [sediment( random.randint(width / 4, width - (width / 4)), random.randint(height/2, height), radius ) for i in range(num_of_sediment)]
loopIndex = 0

## Window initialization ##
w = pygame.display.set_mode([width, height])
w.fill(background)

## Game Loop ##
for i in seds:
    pygame.draw.circle(w, (35, 23, 9), (i.x, i.y), i.radius)
pygame.image.save(w, "sediment_pics/Sediment_" + str(loopIndex) + ".png")

running = True
while running:
    highest = height

    ## Event loop and clock ##
    clock.tick(120)
    for event in pygame.event.get():
        ## Stop when the X is clicked ##
        if event.type == pygame.QUIT:
            running = False

    ## Move water ##
    for i in wats:
        i.move(seds, wats)
        if i.y < highest:
            highest = i.y

    ## Rendering ##
    w.fill(background)

    for i in wats:
        pygame.draw.circle(w, (35, 23, 242), (i.x, i.y), radius)

    for i in seds:
        pygame.draw.circle(w, (35, 23, 9), (i.x, i.y), i.radius)
    
    pygame.display.flip()

    if highest == height:

        # ## Rendering ##
        # w.fill(background)

        # for i in seds:
        #     pygame.draw.circle(w, (35, 23, 9), (i.x, i.y), i.radius)
        
        pygame.display.flip()

        wats = [water(x, y, random.randint(250, 290)) for i in range(10)]
        loopIndex += 1
        pygame.image.save(w, "sediment_pics/Sediment_" + str(loopIndex) + ".png")

fp_in = "sediment_pics/Sediment_*.png"
fp_out = "Sed.gif"

img, *imgs = [Image.open(f) for f in sorted(glob.glob(fp_in))]
img.save(fp=fp_out, format='GIF', append_images=imgs,
         save_all=True, duration=200, loop=0)
for f in glob.glob(fp_in):
    os.remove(f)

## Close the window ##
pygame.quit()
    