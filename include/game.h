#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "entity.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 20
#define PLAYER_SPEED 400.0f

#define BULLET_WIDTH 10
#define BULLET_HEIGHT 20
#define BULLET_SPEED 600.0f

#define ENNEMY_WIDTH 40
#define ENNEMY_HEIGHT 20
#define ENNEMY_SPEED_INIT 30

void new_ennemy(Army *army, int vitesse);
bool init(SDL_Window **window, SDL_Renderer **renderer);
void handle_input(bool *running, const Uint8 *keys, Entity *player, Entity *bullet);
void update(Entity *player, Entity *bullet, float dt, Army *army, Entity *heart, int niveau, int vitesse);
void render(SDL_Renderer *renderer, Entity *player, Entity *bullet, Army *army, Entity *heart);
void end(Entity *player, Army *army, Endgame *endgame);
void cleanup(SDL_Window *window, SDL_Renderer *renderer);

#endif
