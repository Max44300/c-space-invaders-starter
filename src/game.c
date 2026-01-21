#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"
#include <stdio.h>

bool init(SDL_Window **window, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Erreur SDL_Init: %s", SDL_GetError());
        return false;
    }

    *window = SDL_CreateWindow("Space Invaders (SDL)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!*window)
    {
        SDL_Log("Erreur SDL_CreateWindow: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer)
    {
        SDL_Log("Erreur SDL_CreateRenderer: %s", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return false;
    }

    return true;
}

void handle_input(bool *running, const Uint8 *keys, Entity *player, Entity *bullet, bool *bullet_active)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            *running = false;
    }

    player->vx = 0.0f;
    if (keys[SDL_SCANCODE_LEFT])
        player->vx = -PLAYER_SPEED;
    if (keys[SDL_SCANCODE_RIGHT])
        player->vx = PLAYER_SPEED;

    if (keys[SDL_SCANCODE_SPACE] && !*bullet_active)
    {
        *bullet_active = true;
        bullet->x = player->x + player->w / 2 - BULLET_WIDTH / 2;
        bullet->y = player->y;
        bullet->w = BULLET_WIDTH;
        bullet->h = BULLET_HEIGHT;
        bullet->vy = -BULLET_SPEED;
    }
}

void update(Entity *player, Entity *bullet, bool *bullet_active, float dt, Army *army)
{
    player->x += player->vx * dt;

    if (player->x < 0)
        player->x = 0;
    if (player->x + player->w > SCREEN_WIDTH)
        player->x = SCREEN_WIDTH - player->w;

    if (*bullet_active)
    {
        bullet->y += bullet->vy * dt;
        if (bullet->y + bullet->h < 0)
            *bullet_active = false;
        
        for (int i=0; i<army->nb; i++){
            if (army->ennemies[i].alive == true && bullet->y >= army->ennemies[i].y-ENNEMY_HEIGHT && bullet->y <= army->ennemies[i].y && (bullet->x>=army->ennemies[i].x-BULLET_WIDTH) && bullet->x<=army->ennemies[i].x+ENNEMY_WIDTH){
                army->ennemies[i].alive = false;
                *bullet_active = false;
            } 
        }
    }
    for (int i=0; i<army->nb; i++)
    {
        army->ennemies[i].y += ENNEMY_SPEED * dt;
    }

}

void new_ennemy( Army *army)
{
    for (int i=0; i<10; i++){
        Entity ennemy = {
        .x = 20+80*i,
        .y = 60 ,
        .w = ENNEMY_WIDTH,
        .h = ENNEMY_HEIGHT,
        .vx = 0,
        .vy = 0,
        .alive = true};
        army->ennemies[i]=ennemy;
    }
    for (int i=10; i<army->nb; i++){
        Entity ennemy = {
        .x = 20+80*(i-10),
        .y = 100 ,
        .w = ENNEMY_WIDTH,
        .h = ENNEMY_HEIGHT,
        .vx = 0,
        .vy = 0,
        .alive = true};
        army->ennemies[i]=ennemy;
    }
}



void render(SDL_Renderer *renderer, Entity *player, Entity *bullet, bool bullet_active, Army *army)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect player_rect = {
        (int)player->x, (int)player->y,
        player->w, player->h};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &player_rect);
    
    
    SDL_Rect tab[army->nb];
    for (int i=0; i<army->nb; i++){
        if (army->ennemies[i].alive == true){
            tab[i].x = (int)army->ennemies[i].x, 
            tab[i].y = (int)army->ennemies[i].y,
            tab[i].w = army->ennemies[i].w, 
            tab[i].h = army->ennemies[i].h;
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &tab[i]); 
        };
        
    }

    if (bullet_active)
    {
        SDL_Rect bullet_rect = {
            (int)bullet->x, (int)bullet->y,
            bullet->w, bullet->h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &bullet_rect);
    }

    SDL_RenderPresent(renderer);
}

void victory (Army *army, bool running)
{
    running = false;
    for (int i = 0; i<army->nb; i++)
    {
        if (army->ennemies[i].alive){
            running = true;
        }
    }
}

void cleanup(SDL_Window *window, SDL_Renderer *renderer)
{
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}
