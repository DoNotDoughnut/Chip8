#include "SDL.h"
#include "chip8.h"
#include "SDL_opengl.h"
#include "gl/GL.h"

SDL_Window* window;
SDL_GLContext glContext;

int main(int argc, char* argv[])
{

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "Could not initialize SDL!");
        return 1;
    }

    window = SDL_CreateWindow("Demo Game",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        64,
        32,
        SDL_WINDOW_OPENGL);

    if (window == NULL) {
        SDL_Log("Could not create a window: %s", SDL_GetError());
        return -1;
    }

    glContext = SDL_GL_CreateContext(window);
            
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

    if (SDL_GL_SetSwapInterval(1) < 0) {
        fprintf(stderr, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }



    // Create the window.

    struct chip8 myChip8 = new_chip8();

    // Run the message loop.

    SDL_bool running = SDL_TRUE;

    SDL_Event e;

    while(running) {

        while( SDL_PollEvent( &e ) != 0 )
        {
            if( e.type == SDL_QUIT )
            {
                running = SDL_FALSE;
            }
        }

        chip8_cycle(&myChip8);

        if(myChip8.drawFlag) {
            glClear(GL_COLOR_BUFFER_BIT);
            chip8_draw(&myChip8);
            SDL_GL_SwapWindow(window);
        }

        SDL_Delay(16);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}