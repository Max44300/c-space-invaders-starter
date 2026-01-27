#ifndef ENTITY_H
#define ENTITY_H

typedef struct
{
    float x, y;
    float vx, vy;
    int w, h;
    bool alive;
    bool bullet_activ;
    int pv;
} Entity;

typedef struct
{
    int nb;
    Entity ennemies[20];
    Entity bullets[20];
    bool direction; //true pour aller à droite, false pour aller à gauche
} Army;

typedef struct  
{
    bool running;
    bool victory;
}Endgame;



#endif
