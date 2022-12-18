#include <io.h>

#include "chip8.h"

int main(int argc, char* argv[])
{

    struct chip8 chip;
    
    int err = chip8_new(&chip);
    
    if (err != 0)
        return err;
    
    printf("Opening ROM...");

    FILE* file = fopen("rom.ch8", "rb");

    if(file == NULL) {
        fprintf(stderr, "Unable to open ROM! \n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    rewind(file);

    uint8_t buffer[4096 - 511] = {0};

    fread(&buffer, len, 1, file);
    fclose(file);

    if (!chip8_load(&chip, buffer, len)) {
        fprintf(stderr, "Could not load ROM! \n");
        return 1;
    }

    // Run the message loop.

    SDL_bool running = SDL_TRUE;

    SDL_Event event;

    while(running) {

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    running = SDL_FALSE;
                break;
                case SDL_KEYDOWN:
                    chip.key[_chip8_key_layout(event.key.keysym.sym)] = Pressed;
                break;
                case SDL_KEYUP:
                    chip.key[_chip8_key_layout(event.key.keysym.sym)] = Up;
                break;
            }
        }

        chip8_cycle(&chip);

        if(chip.drawFlag) {
            chip8_draw(&chip);
        }

        SDL_Delay(16);
    }

    chip8_quit(&chip);

    return 0;
}