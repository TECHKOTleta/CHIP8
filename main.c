#include <stdlib.h>
#include <stdio.h>

#include "chip8.h"
#include "memoryCore.h"


int main(){
    printf("Starting...\n");

    Chip8 chip8;
    Initialize(&chip8);
    if(chip8.Memory[79] != 0x80){
        printf("ERROR: Sprites were not allocated correctly...\n");
    }
    printf("Sprites loaded...\n");

    char ROMSrc[200];
    int loaded = 1;
    while(loaded != 0){
        printf("Type instructions file name or addres: ");
        scanf("%199s", ROMSrc);
        printf("\n");

        loaded = LoadROM(&chip8, ROMSrc);
        switch(loaded){
            case 1:
                printf("ERROR: Could not open a file...\n");
                break;
            case 2:
                printf("ERROR: The instructions are bigger than 3584B accessable for programs...\n");
                break;
        }
    }
    printf("Instructions were copied, proceding to executions.\n");
    

    return 0;
}