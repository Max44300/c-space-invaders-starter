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

void handle_input(bool *running, const Uint8 *keys, Entity *player, Entity *bullet)
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

    if (keys[SDL_SCANCODE_SPACE] && !player->bullet_activ)
    {
        player->bullet_activ = true;
        bullet->x = player->x + player->w / 2 - BULLET_WIDTH / 2;
        bullet->y = player->y;
        bullet->w = BULLET_WIDTH;
        bullet->h = BULLET_HEIGHT;
        bullet->vy = -BULLET_SPEED;
    }
}

void update(Entity *player, Entity *bullet, float dt, Army *army, Entity *heart, int niveau)
{
    player->x += player->vx * dt;

    if (army->ennemies[0].y >= 200 && army->ennemies[0].y<400){
        for (int i = 0; i< army->nb; i++){
            army->ennemies[i].vx = ENNEMY_SPEED_INIT * 1.3;
        }
    }
    if (army->ennemies[0].y >= 400){
        for (int i = 0; i< army->nb; i++){
            army->ennemies[i].vx = ENNEMY_SPEED_INIT * 1.3*1.3;
        }
    }
    if (player->x < 0)
        player->x = 0;
    if (player->x + player->w > SCREEN_WIDTH)
        player->x = SCREEN_WIDTH - player->w;

    if (player->bullet_activ)
    {
        bullet->y += bullet->vy * dt;
        if (bullet->y + bullet->h < 0)
            player->bullet_activ = false;
        
        for (int i=0; i<army->nb; i++){
            if (army->ennemies[i].alive == true && bullet->y >= army->ennemies[i].y-ENNEMY_HEIGHT && bullet->y <= army->ennemies[i].y && (bullet->x>=army->ennemies[i].x-BULLET_WIDTH) && bullet->x<=army->ennemies[i].x+ENNEMY_WIDTH){
                army->ennemies[i].alive = false;
                player->bullet_activ = false;
            } 
        }
    }
    for (int i=0; i<army->nb; i++)
    {
        if (army->direction){
            army->ennemies[i].x += army->ennemies[i].vx * dt;
        }
        else{
            army->ennemies[i].x += -army->ennemies[i].vx * dt;
        }
        if (army->ennemies[army->nb -1].x > SCREEN_WIDTH -40){
            army->direction = false;
            army->ennemies[i].y += ENNEMY_HEIGHT;
        }
        if (army->ennemies[0].x < 0){
            army->direction = true;
            army->ennemies[i].y += ENNEMY_HEIGHT;
        }
        if (rand()<400000 && !army->ennemies[i].bullet_activ && army->ennemies[i].alive){
            army->ennemies[i].bullet_activ = true;
            army->bullets[i].x = army->ennemies[i].x + army->ennemies[i].w / 2 - BULLET_WIDTH / 4;
            army->bullets[i].y = army->ennemies[i].y;
            army->bullets[i].w = BULLET_WIDTH/2;
            army->bullets[i].h = BULLET_HEIGHT/2;
            army->bullets[i].vy = BULLET_SPEED;
        }
        
        if (army->ennemies[i].bullet_activ)
        {
            army->bullets[i].y += army->bullets[i].vy * dt;
            if (army->bullets[i].y + bullet->h > SCREEN_HEIGHT)
                army->ennemies[i].bullet_activ = false;
            if (army->bullets[i].y >= player->y-PLAYER_HEIGHT && army->bullets[i].y <= player->y && (army->bullets[i].x>=player->x-BULLET_WIDTH/2) && army->bullets[i].x<=player->x+PLAYER_WIDTH)
            {
                army->ennemies[i].bullet_activ=false;
                player->pv += -1;
            }
        }

    }

    if (!heart->alive && rand()<200000*niveau ){
        heart->alive = true;
        heart->x = 50 + rand () % (SCREEN_WIDTH-100);
        heart->y = 50 + rand () % (SCREEN_HEIGHT-400);
    }
    if (heart->alive){
        heart->y += heart->vy * dt;
        if ( heart->y >= player->y-PLAYER_HEIGHT && heart->y <= player->y && (heart->x >= player->x - PLAYER_WIDTH) && heart->x<=player->x + PLAYER_WIDTH){
            if (player->pv <3){
                player->pv += 1;
            }
            heart->alive = false;
        }
        if (heart->y > SCREEN_HEIGHT){
            heart->alive = false;
        }
    }


}

void new_ennemy( Army *army)
{
    for (int i=0; i<10; i++){
        Entity ennemy = {
        .x = 20+60*i,
        .y = 60 ,
        .w = ENNEMY_WIDTH,
        .h = ENNEMY_HEIGHT,
        .vx = ENNEMY_SPEED_INIT,
        .vy = 0,
        .alive = true};
        army->ennemies[i]=ennemy;
    }
    for (int i=10; i<army->nb; i++){
        Entity ennemy = {
        .x = 20+60*(i-10),
        .y = 100 ,
        .w = ENNEMY_WIDTH,
        .h = ENNEMY_HEIGHT,
        .vx = ENNEMY_SPEED_INIT,
        .vy = 0,
        .alive = true};
        army->ennemies[i]=ennemy;
    }
}



void render(SDL_Renderer *renderer, Entity *player, Entity *bullet, Army *army, Entity *heart)
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


    if (player->bullet_activ)
    {
        SDL_Rect bullet_rect = {
            (int)bullet->x, (int)bullet->y,
            bullet->w, bullet->h};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &bullet_rect);
    }

    SDL_Rect tab_bullets[army->nb];
    for (int i=0; i<army->nb; i++){
        if (army->ennemies[i].bullet_activ){
            tab_bullets[i].x = (int)army->bullets[i].x, 
            tab_bullets[i].y = (int)army->bullets[i].y,
            tab_bullets[i].w = army->bullets[i].w, 
            tab_bullets[i].h = army->bullets[i].h;
        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
        SDL_RenderFillRect(renderer, &tab_bullets[i]);

        }
    }
    SDL_Rect vie;
    vie.y =0;
    SDL_Surface* image;
    if (player->pv <= 1){
        image = IMG_Load("1coeur.png");
        vie.w = 50;
        vie.x = 750;
        vie.h = 50;
            if(!image)
            {
                printf("Erreur de chargement de l'image : %s \n",SDL_GetError());
                return -1;
            }
        SDL_Texture* monimage = SDL_CreateTextureFromSurface(renderer,image);
        SDL_RenderCopy(renderer, monimage, NULL, &vie);
    }
    if (player->pv == 2){
        image = IMG_Load("2coeurs.png");
        vie.w = 101;
        vie.x = 699;
        vie.h = 50;
            if(!image)
            {
                printf("Erreur de chargement de l'image : %s \n",SDL_GetError());
                return -1;
            }
        SDL_Texture* monimage = SDL_CreateTextureFromSurface(renderer,image);
        SDL_RenderCopy(renderer, monimage, NULL, &vie);
    }
    if (player->pv == 3){
        image = IMG_Load("3coeurs.png");
        vie.w = 152;
        vie.x = 648;
        vie.h = 50;
            if(!image)
            {
                printf("Erreur de chargement de l'image : %s \n",SDL_GetError());
                return -1;
            }
        SDL_Texture* monimage = SDL_CreateTextureFromSurface(renderer,image);
        SDL_RenderCopy(renderer, monimage, NULL, &vie);
    }
    SDL_Rect coeur;
    SDL_Surface* image2;
    if (heart->alive){
        image2 = IMG_Load("1coeur.png");
        coeur.w = 50;
        coeur.x = heart->x;
        coeur.y = heart->y;
        coeur.h = 50;
        SDL_Texture* monimage2 = SDL_CreateTextureFromSurface(renderer,image2);
        SDL_RenderCopy(renderer, monimage2, NULL, &coeur);
    }
    
    SDL_RenderPresent(renderer);
}

void end (Entity *player, Army *army, Endgame *endgame)
{
    endgame->running = false;
    endgame->victory = true;
    for (int i = 0; i<army->nb; i++)
    {
        if (army->ennemies[i].alive){
            endgame->running = true;
        }
    }
    if (army->ennemies[0].y >= SCREEN_HEIGHT-100){
        endgame->running = false;
        endgame->victory = false;
    }
    if (player->pv <= 0){
        endgame->victory = false;
        endgame->running = false;

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
