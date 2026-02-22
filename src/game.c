#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"
#include <stdio.h>

void new_ennemy( Army *army, int vitesse)
{
    int lim = army->nb/10;
    for (int j=1; j<lim+1; j++){    
        for (int i=10*(j-1); i<10*j; i++){
            Entity ennemy = {
            .x = 20+60*(i%10),
            .y = 20 + 40*j ,
            .w = ENNEMY_WIDTH,
            .h = ENNEMY_HEIGHT,
            .vx = ENNEMY_SPEED_INIT * vitesse,
            .vy = 0,
            .alive = true,
            .pv = 1,
            .type = Normal};
            int a= rand() % (3);
            if (a==1){
                ennemy.type = Resistant;
                ennemy.pv = 2;
            }
            if (a==2){
                ennemy.type = Sniper;
            }
            army->ennemies[i]=ennemy;
        }
    }
}


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
    //on regarde les touches pour déplacer le joueur
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

void update(Entity *player, Entity *bullet, float dt, Army *army, Entity *heart, Entity *ammo, Entity *totem, int niveau, int vitesse)
{
    // pour la progressivité de la vitesse
    if (army->ennemies[0].y >= 200 && army->ennemies[0].y<400){
        for (int i = 0; i< army->nb; i++){
            army->ennemies[i].vx = ENNEMY_SPEED_INIT *vitesse * 1.3;
        }
    }
    if (army->ennemies[0].y >= 400){
        for (int i = 0; i< army->nb; i++){
            army->ennemies[i].vx = ENNEMY_SPEED_INIT *vitesse * 1.3*1.3;
        }
    }
    //on gère le temps d'activation du totem
    if (SDL_GetTicks()-player->tick>2000){
        player->type = Normal;
    }
    // déplacement du joueur
    player->x += player->vx * dt;
    if (player->x < 0)
        player->x = 0;
    if (player->x + player->w > SCREEN_WIDTH)
        player->x = SCREEN_WIDTH - player->w;

    // collision balle du joueur-ennemi
    if (player->bullet_activ)
    {
        bullet->y += bullet->vy * dt;
        if (bullet->y + bullet->h < 0)
            player->bullet_activ = false;
        
        for (int i=0; i<army->nb; i++){
            if (army->ennemies[i].alive == true && bullet->y >= army->ennemies[i].y-ENNEMY_HEIGHT && bullet->y <= army->ennemies[i].y && (bullet->x>=army->ennemies[i].x-BULLET_WIDTH) && bullet->x<=army->ennemies[i].x+ENNEMY_WIDTH){
                army->ennemies[i].pv = army->ennemies[i].pv - bullet->pv;
                if (bullet->pv == 2){
                    bullet->pv = 1;
                }
                player->bullet_activ = false;
                if (army->ennemies[i].pv <= 0 ){
                    army->ennemies[i].alive = false;
                }
            } 
        }
    }
    // déplacement de l'armée d'ennemis
    // d'abord on regarde s'il y a un changement de direction à faire
    if (army->ennemies[army->nb -1].x > SCREEN_WIDTH -40){
            army->direction = false;
            for (int i=0; i<army->nb; i++){
                army->ennemies[i].y += ENNEMY_HEIGHT;
            }
        }
    if (army->ennemies[0].x < 0){
            army->direction = true;
            for (int i=0; i<army->nb; i++){
                army->ennemies[i].y += ENNEMY_HEIGHT;
            }
        }
    //puis on bouge toute l'armée
    for (int i=0; i<army->nb; i++)
    {
        if (army->direction){
            army->ennemies[i].x += army->ennemies[i].vx * dt;
        }
        else{
            army->ennemies[i].x += -army->ennemies[i].vx * dt;
        }
        // on s'occupe des snipers
        if (army->ennemies[i].type == 2){
            if (rand()<600000 && !army->ennemies[i].bullet_activ && army->ennemies[i].alive){
                army->ennemies[i].bullet_activ = true;
                army->bullets[i].x = army->ennemies[i].x + army->ennemies[i].w / 2 - BULLET_WIDTH / 4;
                army->bullets[i].y = army->ennemies[i].y;
                army->bullets[i].w = BULLET_WIDTH/2;
                army->bullets[i].h = BULLET_HEIGHT/2;
                army->bullets[i].vy = BULLET_SPEED;
            }
        }
        // puis des autres ennemis
        if (army->ennemies[i].type == 0 || army->ennemies[i].type == 1 ){
            if (rand()<200000 && !army->ennemies[i].bullet_activ && army->ennemies[i].alive){
                army->ennemies[i].bullet_activ = true;
                army->bullets[i].x = army->ennemies[i].x + army->ennemies[i].w / 2 - BULLET_WIDTH / 4;
                army->bullets[i].y = army->ennemies[i].y;
                army->bullets[i].w = BULLET_WIDTH/2;
                army->bullets[i].h = BULLET_HEIGHT/2;
                army->bullets[i].vy = BULLET_SPEED;
            }
        }
        
        if (army->ennemies[i].bullet_activ)
        {
            army->bullets[i].y += army->bullets[i].vy * dt;
            if (army->bullets[i].y + bullet->h > SCREEN_HEIGHT)
                army->ennemies[i].bullet_activ = false;
            if (army->bullets[i].y >= player->y-PLAYER_HEIGHT && army->bullets[i].y <= player->y && (army->bullets[i].x>=player->x-BULLET_WIDTH/2) && army->bullets[i].x<=player->x+PLAYER_WIDTH)
            {
                army->ennemies[i].bullet_activ=false;
                if (!(player->type == Invicible)){
                    player->pv += -1;
                }
                
            }
        }

    }
    // la partie régénération de la vie et apparition des coeurs
    if (!heart->alive && rand()<400000*niveau ){
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
    //partie apparition d'un power-up : balle renforcée
    if (!ammo->alive && rand()<600000 ){
        ammo->alive = true;
        ammo->x = 50 + rand () % (SCREEN_WIDTH-100);
        ammo->y = 50 + rand () % (SCREEN_HEIGHT-400);
    }
    if (ammo->alive){
        ammo->y += ammo->vy * dt;
        if ( ammo->y >= player->y-PLAYER_HEIGHT && ammo->y <= player->y && (ammo->x >= player->x - PLAYER_WIDTH) && ammo->x<=player->x + PLAYER_WIDTH){
            bullet->pv += 1;
            ammo->alive = false;
        }
        if (ammo->y > SCREEN_HEIGHT){
            ammo->alive = false;
        }
    }
    //partie apparition d'un power-up : totem d'immortalité
    if (!totem->alive && rand()<400000 ){
        totem->alive = true;
        totem->x = 50 + rand () % (SCREEN_WIDTH-100);
        totem->y = 50 + rand () % (SCREEN_HEIGHT-400);
    }
    if (totem->alive){
        totem->y += totem->vy * dt;
        if ( totem->y >= player->y-PLAYER_HEIGHT && totem->y <= player->y && (totem->x >= player->x - PLAYER_WIDTH) && totem->x<=player->x + PLAYER_WIDTH){
            player->type = Invicible;
            player->tick = SDL_GetTicks();
            totem->alive = false;
        }
        if (totem->y > SCREEN_HEIGHT){
            totem->alive = false;
        }
    }


}





void render(SDL_Renderer *renderer, Entity *player, Entity *bullet, Army *army, Entity *heart, Entity *ammo, Entity *totem)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (player->type == Invicible){
        SDL_Rect Shield = {
        (int)player->x-5, (int)player->y-5,
        player->w+10, player->h+10};
    SDL_SetRenderDrawColor(renderer, 255, 204, 255, 20);
    SDL_RenderFillRect(renderer, &Shield);
    }
    SDL_Rect player_rect = {
        (int)player->x, (int)player->y,
        player->w, player->h};
    SDL_SetRenderDrawColor(renderer, 204, 0, 204, 255);
    SDL_RenderFillRect(renderer, &player_rect);
    
    
    
    SDL_Rect tab[army->nb];
    for (int i=0; i<army->nb; i++){
        if (army->ennemies[i].alive == true){
            tab[i].x = (int)army->ennemies[i].x, 
            tab[i].y = (int)army->ennemies[i].y,
            tab[i].w = army->ennemies[i].w, 
            tab[i].h = army->ennemies[i].h;
            if (army->ennemies[i].type == Normal){
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            }
            if (army->ennemies[i].type == Resistant){
                if (army->ennemies[i].pv == 2){
                    SDL_SetRenderDrawColor(renderer, 0, 102, 0, 255);
                }
                else {
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                }
            }
            if (army->ennemies[i].type == Sniper){
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            
            SDL_RenderFillRect(renderer, &tab[i]); 
        };
        
    }


    if (player->bullet_activ)
    {
        SDL_Rect bullet_rect = {
            (int)bullet->x, (int)bullet->y,
            bullet->w, bullet->h};
        if (bullet->pv == 2){
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
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
        SDL_DestroyTexture(monimage);
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
        SDL_DestroyTexture(monimage);
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
        SDL_DestroyTexture(monimage);
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
        SDL_DestroyTexture(monimage2);
    }
    // affichage des ammos
    SDL_Rect munition;
    SDL_Surface* image3;
    if (ammo->alive){
        image3 = IMG_Load("balle.png");
        if(!image3)
            {
                printf("Erreur de chargement de la munition : %s \n",SDL_GetError());
                return -1;
            }
        munition.w = 50;
        munition.x = ammo->x;
        munition.y = ammo->y;
        munition.h = 50;
        SDL_Texture* monimage3 = SDL_CreateTextureFromSurface(renderer,image3);
        SDL_RenderCopy(renderer, monimage3, NULL, &munition);
        SDL_DestroyTexture(monimage3);
    }
    // affichage des totems
    SDL_Rect immort;
    SDL_Surface* image4;
    if (totem->alive){
        image4 = IMG_Load("totem.png");
        if(!image4)
            {
                printf("Erreur de chargement du totem : %s \n",SDL_GetError());
                return -1;
            }
        immort.w = 50;
        immort.x = totem->x;
        immort.y = totem->y;
        immort.h = 50;
        SDL_Texture* monimage4 = SDL_CreateTextureFromSurface(renderer,image4);
        SDL_RenderCopy(renderer, monimage4, NULL, &immort);
        SDL_DestroyTexture(monimage4);
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
