#include <stdio.h>
#include <time.h>

struct chip8 {
    /// @brief Program counter
    unsigned short pc;
    unsigned short opcode;
    /// @brief Index register
    unsigned short I;
    unsigned short sp;

    /// @brief V0 to VF registers.
    unsigned char V[16];
    unsigned short stack[16];
    unsigned char memory[4096];

    unsigned char delay_timer;
    unsigned char sound_timer;
    
    unsigned char gfx[64 * 32];
    unsigned char key[16];

    SDL_bool drawFlag;
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

struct chip8 new_chip8() {

    struct chip8 chip = { 0, .pc = 0x200 };

    memcpy(&chip.memory, &chip8_fontset, 80);


    srand(time(NULL));

    return chip;
}

SDL_bool chip8_load(struct chip8* chip, const char* buf, const size_t size) {
    if (size > 4096 - 512) {
        return SDL_TRUE;
    }
    memcpy(&chip->memory[512], buf, size);
    return SDL_TRUE;
}

void chip8_cycle(struct chip8 *chip) {
    chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];

    switch(chip->opcode & 0xF000) {
        case 0x0:
            switch(chip->opcode & 0x000F) {
                case 0x0000:
                    for(int i = 0; i < 2048; i++) {
                        chip->gfx[i] = 0;
                        chip->drawFlag = SDL_TRUE;
                        chip->pc += 2;
                    }
                break;
                case 0x000E:
                    chip->sp--;
                    chip->pc = chip->stack[chip->sp];
                    chip->pc += 2;
                break;
                default:
                    fprintf(stderr, "Error decoding opcode: 0x%X\n", chip->opcode);
            }
        break;
        case 0x1:
            chip->pc = chip->opcode & 0x0FFF;
        default:
            fprintf(stderr, "Error decoding opcode: 0x%X\n", chip->opcode);
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

}

void chip8_draw(struct chip8 *chip) {

}

void chip8_input(struct chip8 *chip) {

}