#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "backend.h"

#define OP_SIZE 2
#define PC_INC chip->pc += OP_SIZE
#define MEM_SIZE 4096
#define bool SDL_bool
#define KEY_COUNT 16
#define GFX_WIDTH 64
#define GFX_HEIGHT 32
#define true SDL_TRUE
#define false SDL_FALSE

typedef enum {
    Up,
    Pressed,
    Down,
} KeyState;

struct chip8 {
    /// @brief Program counter
    uint16_t pc;
    uint16_t opcode;
    /// @brief Index register
    uint16_t I;
    /// @brief Stack pointer
    uint16_t sp;

    /// @brief V0 to VF registers.
    uint8_t V[16];
    uint16_t stack[16];
    /// @brief 4096 bytes of memory
    uint8_t memory[MEM_SIZE];

    unsigned char delay_timer;
    unsigned char sound_timer;
    
    bool gfx[GFX_WIDTH * GFX_HEIGHT];
    
    KeyState key[KEY_COUNT];

    bool drawFlag;

    struct chip8SDL backend;
};

const unsigned char chip8_fontset[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

int chip8_new(struct chip8* chip) {

    struct chip8 new = { 0, .pc = 0x200 };

    *chip = new;

    // chip->memory = calloc(0, 4096 * sizeof(uint8_t));

    memcpy_s(&chip->memory, MEM_SIZE, &chip8_fontset, 80);

    srand(time(0));

    struct chip8SDL backend = {0};
    int err = _chip8_new_sdl(&backend);

    if (err != 0) {
        return err;
    }

    chip->backend = backend;

    return 0;
}

SDL_bool chip8_load(struct chip8* chip, const char* buf, const size_t size) {
    if (size > MEM_SIZE - 0x200) {
        return false;
    }
    memcpy_s(chip->memory + 0x200, MEM_SIZE - 0x200, buf, size);
    return true;
}

void chip8_cycle(struct chip8 *chip) {

    printf("Program counter: 0x%X\n", chip->pc);

    chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];

    unsigned short code = chip->opcode & 0xF000;
    uint8_t x = ((uint16_t) chip->opcode & 0x0F00) >> 8;
    uint8_t y = ((uint16_t) chip->opcode & 0x00F0) >> 4;
    uint8_t n = chip->opcode & 0x000F;
    uint8_t nn = chip->opcode & 0x00FF;
    uint16_t nnn = chip->opcode & 0x0FFF;

    printf("opcode: 0x%X\n", chip->opcode);

    switch(code) {
        case 0x0000:
            switch(chip->opcode & 0x000F) {
                case 0x0000:
                    memset(&chip->gfx, 0, 64 * 32);
                    chip->drawFlag = true;
                    PC_INC;
                break;
                case 0x000E:
                    chip->pc = chip->stack[(--chip->sp) & 0xF] + 2;
                break;
                default:
                    fprintf(stderr, "Error decoding 0x0 opcode: 0x%X\n", chip->opcode);
                    PC_INC;
            }
        break;
        case 0x1000:
            chip->pc = nnn;
        break;
        case 0x2000:
            chip->stack[(chip->sp++) & 0xF] = chip->pc;
            chip->pc = nnn;
        break;
        case 0x3000:
            if(chip->V[x] == nn) {
                PC_INC;
            }
            PC_INC;
        break;
        case 0x4000:
            if(chip->V[x] != nn) {
                PC_INC;
            }
            PC_INC;
        break;
        case 0x5000:
            if(chip->V[x] == chip->V[y]) {
                PC_INC;
            }
            PC_INC;
        break;
        case 0x6000:
            chip->V[x] = nn;
            PC_INC;
        break;
        case 0x7000:
            chip->V[x] += nn;
            PC_INC;
        break;
        case 0x8000:
            switch (n) {
                case 0x0:
                    chip->V[x] = chip->V[y];
                break;
                case 0x1:
                    chip->V[x] = chip->V[x] | chip->V[y];
                break;
                case 0x2:
                    chip->V[x] = chip->V[x] & chip->V[y];
                break;
                case 0x3:
                    chip->V[x] = chip->V[x] ^ chip->V[y];
                break;
                case 0x4:
                    chip->V[x] += chip->V[y];
                break;
                case 0x5:
                    chip->V[x] -= chip->V[y];
                break;
                case 0x6:
                    uint8_t flag6 = chip->V[x] = chip->V[y];
                    chip->V[x] >>= 1;
                    chip->V[0xF] = flag6;
                break;
                case 0x7:
                    chip->V[x] = chip->V[y] - chip->V[x];
                break;
                case 0xE:
                    uint8_t flage = chip->V[x] = chip->V[y];
                    chip->V[x] <<= 1;
                    chip->V[0xF] = flage;
                break;
            }
            PC_INC;
        break;
        case 0x9000:
            if(chip->V[x] != chip->V[y]) {
                PC_INC;
            }
            PC_INC;
        case 0xA000:
            chip->I = nnn;
            PC_INC;
        break;
        case 0xB000:
            // ambiguous
            chip->pc = chip->V[0] + nnn;
        case 0xC000:
            chip->V[x] = rand() & nn;
            PC_INC;
        break;
        case 0xD000:
            uint8_t sx = chip->V[x] & 63;
            uint8_t sy = chip->V[y] & 31;
            chip->V[0xF] &= 0;

            for (uint8_t yy = 0; yy < n; yy++) {
                SDL_bool pixel = chip->memory[chip->I + yy];
                for (uint8_t xx = 0; xx < 8; xx++) {
                    if (pixel & (0x80 >> xx)) {
                        if (chip->gfx[(sx+xx) + (sy+yy) * 64]) {
                            chip->V[0xF] = 1;
                        }
                        chip->gfx[(sx+xx) + (sy+yy) * 64] ^= 1;
                    }
                }
            }

            chip->drawFlag = true;
            PC_INC;
        break;
        case 0xE000:
            switch (nn) {
                case 0x9E:
                    if(chip->key[chip->V[x]] != Up) {
                        PC_INC;
                    }
                break;
                case 0xA1:
                    if(chip->key[chip->V[x]] == Up) {
                        PC_INC;
                    }
                break;
                default:
                    fprintf(stderr, "Unknown 0xE opcode: 0x%X\n", chip->opcode);
            }
            PC_INC;
        break;
        case 0xF000:
            switch(nn) {
                case 0x07:
                    chip->V[x] = chip->delay_timer;
                    PC_INC;
                break;
                case 0x0A:
                    for (uint8_t i = 0; i < KEY_COUNT; i++) {
                        if (chip->key[i] == Pressed) {
                            PC_INC;
                        }
                    }
                break;
                case 0x15:
                    chip->delay_timer = chip->V[x];
                    PC_INC;
                break;
                case 0x18:
                    chip->sound_timer = chip->V[x];
                    PC_INC;
                break;
                case 0x1E:
                    chip->I += chip->V[x];
                    if (chip->I > 0x0FFF) {
                        chip->I &= 0x0FFF;
                        chip->V[0xF] = 1;
                    }
                    PC_INC;
                break;
                case 0x29:
                    chip->I = chip->V[x];
                    PC_INC;
                break;
                case 0x33:
                    uint8_t integer = chip->V[x] >> 8;
                    uint8_t* loc = &chip->memory[chip->I]; 
                    *loc = integer / 100;
                    *(loc + 1) = (integer / 10) % 10;
                    *(loc + 2) = integer % 10;
                    PC_INC;
                break;
                case 0x55:
                    uint8_t currentStore = 0;
                    while (currentStore <= x) {
                        chip->memory[chip->I + currentStore] = chip->V[currentStore];
                        currentStore++;
                    }
                    PC_INC;
                break;
                case 0x65:
                    uint8_t currentLoad = 0;
                    while(currentLoad <= x) {
                        chip->V[currentLoad] = chip->memory[chip->I + currentLoad];
                        currentLoad++;
                    }
                    PC_INC;
                break;
                default:
                    fprintf(stderr, "Unknown 0xF opcode: 0x%X\n", chip->opcode);
            }
        break;
        default:
            fprintf(stderr, "Error decoding opcode: 0x%X\n", chip->opcode);
            PC_INC;
    }

    if (chip->delay_timer > 0) {
        chip->delay_timer--;
    }

    if(chip->sound_timer >0) {
        chip->sound_timer--;
        if(chip->sound_timer == 0) {
            printf("Sound Timer went off!");
        }
    }

    for (int i = 0; i < KEY_COUNT; i++) {
        if (chip->key[i] == Pressed)
            chip->key[i] = Down;
    }

}

void chip8_draw(struct chip8 *chip) {
    _chip8_draw_sdl(&chip->backend, &chip->gfx);
    chip->drawFlag = false;
}

void chip8_quit(struct chip8* chip) {
    free(chip->memory);
    _chip8_quit_sdl(&chip->backend);
}