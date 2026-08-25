// Flappy Bird Demo - High-Level Script for DByte CHIP-8 SDK
CLEAR
SET V0 = 0x0A
SET V1 = 0x08
ADDR I = 0x0250
DRAW V0, V1, 3
ADD V1 = 0x01
DRAW V0, V1, 3
JUMP 0x0206
