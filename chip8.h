#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef struct Chip8{

    uint16_t PC;
    uint8_t Memory[4096];
    uint8_t V[16];
    uint16_t I;

    uint8_t timeDelay, timeSound;
    uint8_t screen[64*32];//по хорошему потом превратить в массив битов или качество повысить
    uint8_t keys[16];//тоже

    uint16_t Stack[16];
    uint8_t SP;

} Chip8;

#endif