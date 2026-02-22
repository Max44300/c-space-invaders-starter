#ifndef ENTITY_H
#define ENTITY_H

typedef enum 
{
    Normal,         //types pour les ennemis
    Resistant,      //types pour les ennemis
    Sniper,         //types pour les ennemis
    Invicible       //type pour le joueur, après avoir récupéré un totem
} Ennemytype;


typedef struct
{
    float x, y;
    float vx, vy;
    int w, h;
    bool alive;
    bool bullet_activ;
    int pv;
    Ennemytype type;
    Uint32 tick;
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
