// #include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

struct chip8SDL
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
};

int _chip8_new_sdl(struct chip8SDL *backend)
{

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) < 0)
    {
        fprintf(stderr, "Could not initialize SDL!");
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    if ((backend->window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * 5, 32 * 5, SDL_WINDOW_RESIZABLE)) == NULL) {
        return 1;
    }

    if ((backend->renderer = SDL_CreateRenderer(backend->window, -1, 0)) == NULL) {
        return 1;
    }

    SDL_RenderSetVSync(backend->renderer, 1);

    backend->texture = SDL_CreateTexture(backend->renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, 64, 32);

    SDL_StartTextInput();

    printf("Initialized SDL2!\n");

    return 0;
}

uint8_t _chip8_key_layout(SDL_Keycode code) {
    switch (code) {
        case SDLK_1: return 0;
        case SDLK_2: return 1;
        case SDLK_3: return 2;
        case SDLK_4: return 3;
        case SDLK_q: return 4;
        case SDLK_w: return 5;
        case SDLK_e: return 6;
        case SDLK_r: return 7;
        case SDLK_a: return 8;
        case SDLK_s: return 9;
        case SDLK_d: return 10;
        case SDLK_f: return 11;
        case SDLK_z: return 12;
        case SDLK_x: return 13;
        case SDLK_c: return 14;
        case SDLK_v: return 15;
        default: return UINT8_MAX;
    }
}

// split into function to update texture and one to render texture (or not since clear isnt called a lot)
void _chip8_draw_sdl(struct chip8SDL *backend, SDL_bool *gfx)
{

    int w, h;

    SDL_GetWindowSize(backend->window, &w, &h);

    SDL_RenderSetScale(backend->renderer, w / 64.0, h / 32.0);

    SDL_SetRenderDrawColor(backend->renderer, 0, 0, 0, 50);

    SDL_RenderClear(backend->renderer);

    SDL_SetRenderDrawColor(backend->renderer, 255, 255, 255, 255);

    // uint32_t pixels[64 *32] = {0};

    // for (int i = 0; i < 64 * 32; i++)
    // {
    //     pixels[i] = gfx[i] == SDL_TRUE ? 0xFFFFFFFF : 0;
    // }

    for (int i = 0; i < 64; i++) {    
        for (int j = 0; j < 32; j++) {
            if(gfx[i + j * 64] == SDL_TRUE) {
                SDL_RenderDrawPoint(backend->renderer, i, j);
            }
        }
    }

    // SDL_UpdateTexture(backend->texture, NULL, &pixels, 4);

    // SDL_RenderCopy(backend->renderer, backend->texture, NULL, NULL);

    printf("Rendered!\n");

    SDL_RenderPresent(backend->renderer);
}

void _chip8_quit_sdl(struct chip8SDL *backend)
{
    SDL_DestroyRenderer(backend->renderer);
    SDL_DestroyWindow(backend->window);
    SDL_Quit();
}