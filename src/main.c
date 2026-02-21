#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <time.h>
#include "entity.h"
#include "game.h"


int main(void)
{
   
    srand(time(NULL)); // pour avoir une génération aléatoire à chaque partie
    
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!init(&window, &renderer))
    {
        return 1;
    }

    Endgame endgame = {true, false};
    Uint32 last_ticks = SDL_GetTicks();

    
    bool selection_vie = true;
    int niveau = 3;
    bool selection_vitesse = true;
    int vitesse = 1;

    SDL_Surface* select;
    select = IMG_Load("Selection_difficulte.png");
    SDL_Texture* maselect = SDL_CreateTextureFromSurface(renderer,select);

    while (selection_vie){
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, maselect, NULL, NULL);
        SDL_RenderPresent(renderer);
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                selection_vie = false;
        }
        if (keys[SDL_SCANCODE_KP_1]){
            niveau = 1;
            selection_vie = false;
        }
        if (keys[SDL_SCANCODE_KP_2]){
            niveau = 2;
            selection_vie = false;
        }
        if (keys[SDL_SCANCODE_KP_3]){
            niveau = 3;
            selection_vie = false;
        }

    }
    SDL_DestroyTexture(maselect);
    SDL_FreeSurface(select);

    SDL_Delay(1000);
    
    SDL_Surface* select_vit;
    select_vit = IMG_Load("Selection_vitesse.png");
    if(!select_vit)
            {
                printf("Erreur de chargement de l'image : %s \n",SDL_GetError());
                return -1;
            }
    SDL_Texture* maselect_vit = SDL_CreateTextureFromSurface(renderer,select_vit);

    while (selection_vitesse){
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, maselect_vit, NULL, NULL);
        SDL_RenderPresent(renderer);
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                selection_vitesse = false;
        }
        if (keys[SDL_SCANCODE_KP_1]){
            vitesse = 1;
            selection_vitesse = false;
        }
        if (keys[SDL_SCANCODE_KP_2]){
            vitesse = 2;
            selection_vitesse = false;
        }
        if (keys[SDL_SCANCODE_KP_3]){
            vitesse = 3;
            selection_vitesse = false;
        }



    }
    SDL_DestroyTexture(maselect_vit);
    SDL_FreeSurface(select_vit);
    
   

    Entity player = {
        .x = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2,
        .y = SCREEN_HEIGHT - 100,
        .w = PLAYER_WIDTH,
        .h = PLAYER_HEIGHT,
        .vx = 0,
        .vy = 0,
        .alive = true,
        .bullet_activ = false,
        .pv = 3 };

    Entity bullet = {0};
    Entity heart = {
        .vy = 300,
        .alive = false,
    };
    bool endscreen = true;
    Army army;
    army.nb = 20;
    army.direction = true;
    new_ennemy(&army, vitesse);

    while (endgame.running)
    {
        Uint32 ticks = SDL_GetTicks();
        float dt = (ticks - last_ticks) / 1000.0f;
        if (dt > 0.05f)
            dt = 0.05f;
        last_ticks = ticks;

        SDL_PumpEvents();
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        handle_input(&endgame.running, keys, &player, &bullet);
        update(&player, &bullet, dt, &army, &heart, niveau, vitesse);
        render(renderer, &player, &bullet, &army, &heart);
        end(&player, &army, &endgame);

        SDL_Event event;     //pour sortir de la partie si on ne veut pas la finir
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                endgame.running = false;
        }
        if (keys[SDL_SCANCODE_ESCAPE]){
            endgame.running = false;
            endscreen = false;
        }

    }

    SDL_Surface* image;
        if (endgame.victory){
            image = IMG_Load("victoire.png");
            if(!image)
            {
                printf("Erreur de chargement de l'image : %s \n",SDL_GetError());
                return -1;
            }

        }
        else{
            image = IMG_Load("defaite.png");
            if(!image)
            {
                printf("Erreur de chargement de l'image : %s \n",SDL_GetError());
                return -1;
            }
        }

    
    while (endscreen){
        SDL_RenderClear(renderer);
        SDL_Texture* monimage = SDL_CreateTextureFromSurface(renderer,image);
        SDL_RenderCopy(renderer, monimage, NULL, NULL);
        SDL_RenderPresent(renderer);
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                endscreen = false;
        }
        if (keys[SDL_SCANCODE_SPACE]){
            endscreen = false;
        }
        SDL_DestroyTexture(monimage);
    }
    SDL_FreeSurface(image);

    cleanup(window, renderer);
    SDL_Quit;
    
    
    return 0;
}
