#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "entity.h"
#include "game.h"

int main(void)
{
    //initialisation de la librairie SDL_ttf
    if (TTF_Init() < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", TTF_GetError());
        return EXIT_FAILURE;
    }
    TTF_Font* font = TTF_OpenFont("ARIALI.TTF", 18); // Crée un police avec la police "ariali.ttf" et de taille 18 pixels
    if (font == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", TTF_GetError());

    }
    //
    
    
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!init(&window, &renderer))
    {
        return 1;
    }

    bool running = true;
    Uint32 last_ticks = SDL_GetTicks();

    Entity player = {
        .x = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2,
        .y = SCREEN_HEIGHT - 60,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT,
        .vx = 0,
        .vy = 0,
        .alive = true };

    Entity bullet = {0};
    bool bullet_active = false;
    Army army;
    army.nb = 20;
    new_ennemy(&army);

    while (running)
    {
        Uint32 ticks = SDL_GetTicks();
        float dt = (ticks - last_ticks) / 1000.0f;
        if (dt > 0.05f)
            dt = 0.05f;
        last_ticks = ticks;

        SDL_PumpEvents();
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        handle_input(&running, keys, &player, &bullet, &bullet_active);
        update(&player, &bullet, &bullet_active, dt, &army);
        render(renderer, &player, &bullet, bullet_active, &army);
        victory(&army, running);

    }
    SDL_Surface* text = TTF_RenderText_Blended(font, "Victoire", SDL_Color{ 0, 255, 0, 255 });

    cleanup(window, renderer);
    void SDLCALL TTF_CloseFont(TTF_Font *font);
    TTF_Quit();
    return 0;
}
