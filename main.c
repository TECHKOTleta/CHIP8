#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "chip8.h"
#include "memoryCore.h"

void fetch(Chip8* chip8, uint8_t* result){
    result[0] = chip8->Memory[chip8->PC++];
    result[1] = chip8->Memory[chip8->PC++];
}

void goToSubroutine(Chip8* chip8, int16_t address){
    chip8->Stack[chip8->SP++] = chip8->PC;
}


void returnFromSubroutine(Chip8* chip8){
    chip8->PC = chip8->Stack[--chip8->SP]+2;
}

#include <stdio.h>
#include <stdint.h>

uint8_t adder(uint8_t a, uint8_t b, uint8_t* carry){
    uint8_t a_bit = 0, b_bit = 0, c_bit = 0;
    uint8_t res = 0, res_bit = 0, count = 0;

    while(a != 0 || b != 0){
        a_bit = a & 0b1;
        a >>= 1;
        b_bit = b & 0b1;
        b >>= 1;

        res_bit = a_bit ^ b_bit ^ c_bit;
        c_bit = (a_bit&b_bit)|(a_bit&c_bit)|(b_bit&c_bit);

        res_bit<<=(count++);
        res |= res_bit;
    }

    if(count == 8){
        *carry = c_bit;
    }
    else{
        *carry = 0;
        res |= (1 << (count)) * c_bit;
    }
    
    return res;
}

uint8_t substractor(uint8_t a, uint8_t b, uint8_t* borrow){
    uint8_t a_bit = 0, b_bit = 0, c_bit = 0;
    uint8_t res = 0, res_bit = 0, count = 0;
    *borrow = 1;

    while(a != 0 || b != 0 || c_bit != 0){
        a_bit = a & 0b1;
        a >>= 1;
        b_bit = b & 0b1;
        b >>= 1;

        res_bit = a_bit ^ b_bit ^ c_bit;
        c_bit = (b_bit&(!a_bit))|(c_bit&(!a_bit))|(b_bit&c_bit);

        res_bit<<=(count++);
        res |= res_bit;

        if(count == 8 && c_bit == 1){
            *borrow = 0;
            c_bit = 0;
        }
    }
    
    return res;
}

int main() {
    uint8_t a, b, c;
    a = 50;
    b = 7;
    c = 0;
    uint8_t res = adder(a, b, &c);
    printf("Num: %d, carry: %d", res, c);
    return 0;
}

uint8_t execute(Chip8* chip8, uint8_t* instruction){
    uint8_t base = instruction[0] >> 4;
    uint16_t rest = (uint16_t)(instruction[0] << 4) << 4 | instruction[1];
    uint8_t X = instruction[0] & 0x00FF;
    uint8_t Y = instruction[1] >> 4;
    uint8_t value;
    printf("Executing %X%x...\n", instruction[0], instruction[1]);

    switch (base)
    {
    case 0:
        if(rest == 0x0E0){
            printf("Clscr\n");
            memset(chip8->screen, 0, sizeof(chip8->screen));
        }
        else if(rest == 0x0EE){
            returnFromSubroutine(chip8);
            printf("Return to %x\n", chip8->PC);
        }
        else{
            printf("Unknown operation with base 0\n");
            return 0;
        }
        break;

    case 1:
        chip8->PC = rest;
        printf("Jump to %x\n", chip8->PC);
        break;

    case 2:
        goToSubroutine(chip8, rest);
        printf("Jump to %x, subroutine style\n", chip8->PC);
        break;

    case 3:
        value = (uint8_t)(rest);
        if(chip8->V[X] == value){
            chip8->PC+=2;
            printf("Skipping, do not executing next instruction, skip to %x, because V%x (%x) = %x, but should not be\n", chip8->PC, X, chip8->V[X], value);
        }
        else{
            printf("Proceeding normally to %x, because V%x (%x) != %x, and should not be\n", chip8->PC, X, chip8->V[X], value);
        }
        break;

    case 4:
        value = (uint8_t)(rest);
        if(chip8->V[X] != value){
            chip8->PC+=2;
            printf("Skipping, do not executing next instruction, skip to %x, because V%x (%x) != %x, but should be\n", chip8->PC, X, chip8->V[X], value);
        }
        else{
            printf("Proceeding normally to %x, because V%x (%x) != %x, and should be\n", chip8->PC, X, chip8->V[X], value);
        }
        break;
    case 5:
        if(rest & 0xFF != 0){
            printf("Unknown operation with base 5\n");
            return 0;
        }
        if(chip8->V[X] == chip8->V[Y]){
            chip8->PC+=2;
            printf("Skipping, do not executing next instruction, skip to %x, because V%x (%x) == V%x (%x), but should not be\n", chip8->PC, X, chip8->V[X], Y, chip8->V[Y]);
        }
        else{
           printf("Proceeding normally to %x, because V%x (%x) != V%x (%x), and should not be\n", chip8->PC, X, chip8->V[X], Y, chip8->V[Y]);
        }
        break;
    case 6:
        value = (uint8_t)(rest);
        chip8->V[X] = value;
        printf("%x was written into V%x", value, X);
        break;
    case 7:
        value = (uint8_t)(rest);
        chip8->V[X] += value;
        printf("%x was written into V%x", value, X);
        break;
    case 8:
        value = (uint8_t)(rest) & 0x0F;
        switch (value)
        {
        case 0:
            chip8->V[X] = chip8->V[Y];
            printf("Made V%x = V%x (%x)", X, Y, chip8->V[X]);
            break;
        case 1:
            chip8->V[X] |= chip8->V[Y];
            printf("Made V%x |= V%x, result: %x", X, Y, chip8->V[X]);
            break;
        case 2:
            chip8->V[X] &= chip8->V[Y];
            printf("Made V%x &= V%x, result: %x", X, Y, chip8->V[X]);
            break;
        case 3:
            chip8->V[X] ^= chip8->V[Y];
            printf("Made V%x ^= V%x, result: %x", X, Y, chip8->V[X]);
            break;
        case 4:
            chip8->V[X] = adder(chip8->V[X], chip8->V[Y], &chip8->V[0xF]);
            printf("Made V%x += V%x, result: %x with carry %b", X, Y, chip8->V[X], chip8->V[0xF]);
            break;

        case 5:
            chip8->V[X] = substractor(chip8->V[X], chip8->V[Y], &chip8->V[0xF]);
            printf("Made V%x -= V%x, result: %x with borrow %b", X, Y, chip8->V[X], chip8->V[0xF]);
            break;

        case 6:
            chip8->V[0xF] = chip8->V[X] & 0b1;
            chip8->V[X] >>= chip8->V[Y];
            printf("Made V%x >>= V%x(%d bits), result: %x with %b in VF", X, Y, chip8->V[Y], chip8->V[X], chip8->V[0xF]);
            break;

        case 7:
            chip8->V[X] = substractor(chip8->V[Y], chip8->V[X], &chip8->V[0xF]);
            printf("Made V%x = V%x - V%x, result: %x with borrow %b", X, Y, X, chip8->V[X], chip8->V[0xF]);
            break;

        case 0xE:
            chip8->V[0xF] = (chip8->V[X] & 0b10000000) >> 7;
            chip8->V[X] <<= chip8->V[Y];
            printf("Made V%x <<= V%x(%d bits), result: %x with %b in VF", X, Y, chip8->V[Y], chip8->V[X], chip8->V[0xF]);
            break;
        
        default:
            printf("Unknown operation with base 8\n");
            return 0;
        }
        break;
    
    case 9:
        value = (uint8_t)(rest) & 0x0F;
        if(value != 0){
            printf("Unknown operation with base 9\n");
            return 0;
        }
        if(chip8->V[X] != chip8->V[Y]){
            chip8->PC+=2;
            printf("Skipping, do not executing next instruction, skip to %x, because V%x (%x) != V%x (%x), but should be\n", chip8->PC, X, chip8->V[X], Y, chip8->V[Y]);
        }
        break;

    case 0xA:
        chip8->I = rest;
        printf("Wrote %x in I\n", rest);
        break;

    case 0xB:
        chip8->PC = rest+chip8->V[0];
        printf("Jump %x (%x given and %x in V0)", chip8->PC, rest, chip8->V[0]);
        break;

    case 0xC:
        chip8->V[X] = rand() % 256;
        chip8->V[X] &= (uint8_t)rest;
        printf("Random value (%d) generated and put in V%x (with %x &)", chip8->V[X], (uint8_t)rest);
        break;

    default:
        return 0;
    }

    return 1;
}

int main(){
    printf("Starting...\n");

    srand(time(NULL));
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

    uint8_t instruction[] = {0, 0};

    for(int i = 0; i < 10; i++){
        fetch(&chip8, instruction);
        printf("Instruction: %x%x, next part: %x \n", instruction[0], instruction[1], chip8.PC);
    }
    

    return 0;
}