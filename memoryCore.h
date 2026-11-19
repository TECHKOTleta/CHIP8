#ifndef MEMORY_CORE_H
#define MEMORY_CORE_H

#include "chip8.h"

void Initialize(Chip8* chip8);
int LoadROM(Chip8* chip8, const char* fileName);


#endif