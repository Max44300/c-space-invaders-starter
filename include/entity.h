#ifndef ENTITY_H
#define ENTITY_H

typedef struct
{
    float x, y;
    float vx, vy;
    int w, h;
    bool alive;
} Entity;

typedef struct
{
    int nb;
    Entity ennemies[20];
} Army;


#endif
