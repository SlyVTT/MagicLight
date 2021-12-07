#include <stdlib.h>
#include <stdio.h>

#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/gint.h>

#include "structs.h"
#include "primitives.h"

#define DEBUG 0

#define R_ROUGE 0
#define R_VERT 1
#define R_BLEU 2
#define R_NOIR 3
#define R_ROSE 4
#define R_CYAN 5
#define R_JAUNE 6
#define R_BLANC 7

#define LAYER_BELOW_RAYS 0
#define LAYER_ABOVE_RAYS 1


unsigned int SIZE = 16;
unsigned int OFFSET_X = 12;
unsigned int OFFSET_Y = 12;

unsigned int OFFSET_X_MINIMAP = 312;
unsigned int OFFSET_Y_MINIMAP = 160;
unsigned char SCALE_X_MINIMAP = 8;
unsigned char SCALE_Y_MINIMAP = 6;

unsigned char ALPHA_OPAQUE_DEFAULT = 80; //191
unsigned char ALPHA_TRANSPARENT_DEFAULT = 40; //62

unsigned char ALPHA_OPAQUE = ALPHA_OPAQUE_DEFAULT;
unsigned char ALPHA_TRANSPARENT = ALPHA_TRANSPARENT_DEFAULT;
unsigned char ALPHA_BLOQUE = 1;

unsigned char SPEEDUP_RAY = 1;
unsigned char SPEEDUP_WALL = 1;

char LEVELCOLOR=0;


#define NB_LEVELS 4

unsigned char NbReceptorsLevel;
unsigned char NbDoorsLevel;
unsigned char NbBlocksLevel;
unsigned char NbTreasureLevel;
unsigned char NbObstacleLevel;

selection selected = JOUEUR;

unsigned int score;

unsigned char SIZE_MAP_X=13;
unsigned char  SIZE_MAP_Y=11;

Player joueur;
Light lumiere;
//MapItem carte[SIZE_MAP_X][SIZE_MAP_Y];
Wall murs[4];

Blocks* BlocRouge;
Blocks* BlocVert ;
Blocks* BlocBleu;
Blocks* BlocNoir;


Blocks* BlocksCollection=NULL;
Receptor* ReceptorCollection=NULL;
Door* DoorCollection=NULL;
Treasure* TreasureCollection=NULL;
Obstacle* ObstaclesCollection=NULL;


Minimap Map[ NB_LEVELS ];

unsigned char ALPHA_VALIDE = 255;
unsigned char ALPHA_NON_VALIDE = 100;

unsigned char EPSILON = 16;

extern bopti_image_t maintitle;
extern bopti_image_t leveltiles;
extern bopti_image_t sprites;
extern bopti_image_t player;
extern bopti_image_t light;
extern bopti_image_t cursor;
extern bopti_image_t parchemin;
extern bopti_image_t treasures;
extern bopti_image_t chests;

extern font_t font_fantasy;
extern font_t font_tiny;

bool doneMain = false;
bool donePause = false;
bool doneOption = false;
bool doneGame = false;
bool doneStart = false;
bool doneTitle = false;

unsigned int compteur_mouvement = 0;
unsigned char frame_cursor = 0;
unsigned char frame_light = 0;
unsigned char frame=0;
bool mouvement=false;
orientations direction=HAUT;

unsigned char nextLevel;
unsigned char currentLevel;
unsigned char selectStartMenu = 0;
unsigned char selectOptionMenu = 0;
unsigned char selectOptionPause = 0;

void initMap( void )
{
       for( unsigned char k=0; k < NB_LEVELS; k++) Map[k].visited = true;
       currentLevel = 0;
}

void loadLevel( unsigned char numLevel )
{
       free(BlocksCollection);
       free(ReceptorCollection);
       free(DoorCollection);
       free(TreasureCollection);
       free(ObstaclesCollection);

       BlocksCollection=NULL;
       ReceptorCollection=NULL;
       DoorCollection=NULL;
       TreasureCollection=NULL;
       ObstaclesCollection=NULL;

       if (numLevel==0)
       {
              currentLevel = numLevel;

              LEVELCOLOR = 0;
              SIZE_MAP_X=13;
              SIZE_MAP_Y=11;

              NbReceptorsLevel = 3;
              NbDoorsLevel = 1;
              NbBlocksLevel = 4;
              NbTreasureLevel = 0;
              NbObstacleLevel = 8;

              BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
              ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
              DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
              TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
              ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );

              Map[numLevel].x=0;
              Map[numLevel].y=0;
              Map[numLevel].R=0;
              Map[numLevel].G=0;
              Map[numLevel].B=255;
              Map[numLevel].A=255;
              Map[numLevel].visited=true;

              joueur = { 7, 9, HAUT };
              lumiere = { 6, 5, 1,1,1,0, false };

              ObstaclesCollection[0] = {2,2,BLOCK_SUN};
              ObstaclesCollection[1] = {10,2,BLOCK_SUN};
              ObstaclesCollection[2] = {2,8,BLOCK_SUN};
              ObstaclesCollection[3] = {10,8,BLOCK_SUN};
              ObstaclesCollection[4] = {4,5,BLOCK_WATER};
              ObstaclesCollection[5] = {8,5,BLOCK_WATER};
              ObstaclesCollection[6] = {6,3,BLOCK_WATER};
              ObstaclesCollection[7] = {6,7,BLOCK_WATER};

              BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
              BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
              BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
              BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

              BlocRouge = &BlocksCollection[0];
              BlocVert = &BlocksCollection[1];
              BlocBleu = &BlocksCollection[2];
              BlocNoir = &BlocksCollection[3];

              ReceptorCollection[0] = { 5, 0, BAS, 255, 0, 0, false };  // Receptor RED
              ReceptorCollection[1] = { 12, 4, GAUCHE, 0, 255, 0, false }; // Receptor GREEN
              ReceptorCollection[2] = { 0, 6, DROITE, 0, 0, 255, false }; // Receptor BLUE
              // ReceptorCollection[3] = { 10, 8, GAUCHE, 0, 0, 0, false }; // Receptor BLACK
              // ReceptorCollection[4] = { 8, 0, BAS, 255, 0, 255, false }; // Receptor PINK
              // ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
              // ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
              // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

              DoorCollection[0] = { 6,0, HAUT, false, true, 3, { R_ROUGE,R_BLEU, R_VERT }, 2}; // Door NORTH
              //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 1, { R_BLEU, -1, -1 }, 0}; // Door WEST
              //DoorCollection[2] = { 12, 5, DROITE, false, true, 1, { R_VERT, -1, -1 }, 0}; // Door EAST
              //DoorCollection[3] = { 6, 10, BAS, false, false, 0, { -1 -1, -1 }, -1}; // Door SOUTH

              //TreasureCollection[0] = {2,2, T_RED, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[1] = {11,9, T_YELLOW, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[2] = {10,4, T_GREEN, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[3] = {1,1, T_BLUE, true, PIERRE_BLANCHE, 100 };
       }
       else if (numLevel==1)
       {
              currentLevel = numLevel;

              LEVELCOLOR = 1;
              SIZE_MAP_X=13;
              SIZE_MAP_Y=11;

              NbReceptorsLevel = 4;
              NbDoorsLevel = 2;
              NbBlocksLevel = 4;
              NbTreasureLevel = 0;
              NbObstacleLevel = 16;

              BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
              ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
              DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
              TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
              ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );

              Map[numLevel].x=-1;
              Map[numLevel].y=-1;
              Map[numLevel].R=0;
              Map[numLevel].G=0;
              Map[numLevel].B=255;
              Map[numLevel].A=255;
              Map[numLevel].visited=true;

              joueur = { 7, 9, HAUT };
              lumiere = { 6, 5, 1,1,1,0, false };

              ObstaclesCollection[0] = {1,1,BLOCK_STONE};
              ObstaclesCollection[1] = {11,1,BLOCK_STONE};
              ObstaclesCollection[2] = {1,9,BLOCK_STONE};
              ObstaclesCollection[3] = {11,9,BLOCK_STONE};

              ObstaclesCollection[4] = {1,2,BLOCK_WATER};
              ObstaclesCollection[5] = {2,1,BLOCK_WATER};
              ObstaclesCollection[6] = {2,2,BLOCK_WATER};

              ObstaclesCollection[7] = {10,1,BLOCK_WATER};
              ObstaclesCollection[8] = {10,2,BLOCK_WATER};
              ObstaclesCollection[9] = {11,2,BLOCK_WATER};

              ObstaclesCollection[10] = {10,9,BLOCK_WATER};
              ObstaclesCollection[11] = {10,8,BLOCK_WATER};
              ObstaclesCollection[12] = {11,8,BLOCK_WATER};

              ObstaclesCollection[13] = {1,8,BLOCK_WATER};
              ObstaclesCollection[14] = {2,8,BLOCK_WATER};
              ObstaclesCollection[15] = {2,9,BLOCK_WATER};

              BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
              BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
              BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
              BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

              BlocRouge = &BlocksCollection[0];
              BlocVert = &BlocksCollection[1];
              BlocBleu = &BlocksCollection[2];
              BlocNoir = &BlocksCollection[3];

              ReceptorCollection[0] = { 5, 0, BAS, 255, 0, 0, false };  // Receptor RED
              ReceptorCollection[1] = { 12, 4, GAUCHE, 0, 255, 0, false }; // Receptor GREEN
              ReceptorCollection[2] = { 0, 6, DROITE, 0, 0, 255, false }; // Receptor BLUE
              ReceptorCollection[3] = { 6, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK
              // ReceptorCollection[4] = { 8, 0, BAS, 255, 0, 255, false }; // Receptor PINK
              // ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
              // ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
              // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

              DoorCollection[0] = { 6,0, HAUT, false, false, 0, { R_ROUGE,R_BLEU, -1 }, 1}; // Door NORTH
              //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 1, { R_BLEU, -1, -1 }, 0}; // Door WEST
              DoorCollection[1] = { 12, 5, DROITE, false, true, 2, { R_VERT, R_NOIR, -1 }, 0}; // Door EAST
              //DoorCollection[3] = { 6, 10, BAS, false, false, 0, { -1 -1, -1 }, -1}; // Door SOUTH

              //TreasureCollection[0] = {2,2, T_RED, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[1] = {11,9, T_YELLOW, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[2] = {10,4, T_GREEN, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[3] = {1,1, T_BLUE, true, PIERRE_BLANCHE, 100 };
       }
       else if (numLevel==2)
       {
              currentLevel = numLevel;

              LEVELCOLOR = 2;
              SIZE_MAP_X=13;
              SIZE_MAP_Y=11;

              NbReceptorsLevel = 8;
              NbDoorsLevel = 3;
              NbBlocksLevel = 4;
              NbTreasureLevel = 0;
              NbObstacleLevel = 0;

              BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
              ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
              DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
              TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
              ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );

              Map[numLevel].x=0;
              Map[numLevel].y=-1;
              Map[numLevel].R=0;
              Map[numLevel].G=0;
              Map[numLevel].B=255;
              Map[numLevel].A=255;
              Map[numLevel].visited=true;

              joueur = { 7, 9, HAUT };
              lumiere = { 6, 5, 1,1,1,0, false };

              //ObstaclesCollection[0] = {2,2,BLOCK_SUN};
              //ObstaclesCollection[1] = {10,2,BLOCK_SUN};
              //ObstaclesCollection[2] = {2,8,BLOCK_SUN};
              //ObstaclesCollection[3] = {10,8,BLOCK_SUN};
              //ObstaclesCollection[4] = {4,5,BLOCK_WATER};
              //ObstaclesCollection[5] = {8,5,BLOCK_WATER};
              //ObstaclesCollection[6] = {6,3,BLOCK_WATER};
              //ObstaclesCollection[7] = {6,7,BLOCK_WATER};

              BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
              BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
              BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
              BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

              BlocRouge = &BlocksCollection[0];
              BlocVert = &BlocksCollection[1];
              BlocBleu = &BlocksCollection[2];
              BlocNoir = &BlocksCollection[3];

              ReceptorCollection[0] = { 5, 0, BAS, 255, 0, 0, false };  // Receptor RED
              ReceptorCollection[1] = { 12, 4, GAUCHE, 0, 255, 0, false }; // Receptor GREEN
              ReceptorCollection[2] = { 0, 6, DROITE, 0, 0, 255, false }; // Receptor BLUE
              ReceptorCollection[3] = { 4, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK
              ReceptorCollection[4] = { 8, 0, BAS, 255, 0, 255, false }; // Receptor PINK
              ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
              ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
              ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

              //DoorCollection[0] = { 6,0, HAUT, false, true, 1, { R_ROUGE,R_BLEU, R_VERT }, 1}; // Door NORTH
              DoorCollection[0] = { 0, 5, GAUCHE, false, true, 3, { R_BLEU, R_ROUGE, R_VERT }, 1}; // Door WEST
              DoorCollection[1] = { 12, 5, DROITE, false, true, 1, { R_BLANC, -1, -1 }, 3}; // Door EAST
              DoorCollection[2] = { 6, 10, BAS, false, true, 2, { R_NOIR, R_ROSE, -1 }, 0}; // Door SOUTH

              //TreasureCollection[0] = {2,2, T_RED, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[1] = {11,9, T_YELLOW, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[2] = {10,4, T_GREEN, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[3] = {1,1, T_BLUE, true, PIERRE_BLANCHE, 100 };
       }
       else if (numLevel==3)
       {
              currentLevel = numLevel;

              LEVELCOLOR = 3;
              SIZE_MAP_X=13;
              SIZE_MAP_Y=11;

              NbReceptorsLevel = 4;
              NbDoorsLevel = 2;
              NbBlocksLevel = 4;
              NbTreasureLevel = 0;
              NbObstacleLevel = 8;

              BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
              ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
              DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
              TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
              ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );

              Map[numLevel].x=1;
              Map[numLevel].y=-1;
              Map[numLevel].R=0;
              Map[numLevel].G=0;
              Map[numLevel].B=255;
              Map[numLevel].A=255;
              Map[numLevel].visited=true;;

              joueur = { 7, 9, HAUT };
              lumiere = { 6, 5, 1,1,1,0, false };

              ObstaclesCollection[0] = {2,2,BLOCK_SUN};
              ObstaclesCollection[1] = {10,2,BLOCK_SUN};
              ObstaclesCollection[2] = {2,8,BLOCK_SUN};
              ObstaclesCollection[3] = {10,8,BLOCK_SUN};
              ObstaclesCollection[4] = {4,5,BLOCK_WATER};
              ObstaclesCollection[5] = {8,5,BLOCK_WATER};
              ObstaclesCollection[6] = {6,3,BLOCK_WATER};
              ObstaclesCollection[7] = {6,7,BLOCK_WATER};

              BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
              BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
              BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
              BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

              BlocRouge = &BlocksCollection[0];
              BlocVert = &BlocksCollection[1];
              BlocBleu = &BlocksCollection[2];
              BlocNoir = &BlocksCollection[3];

              ReceptorCollection[0] = { 5, 0, BAS, 255, 0, 0, false };  // Receptor RED
              ReceptorCollection[1] = { 12, 4, GAUCHE, 0, 255, 0, false }; // Receptor GREEN
              ReceptorCollection[2] = { 0, 6, DROITE, 0, 0, 255, false }; // Receptor BLUE
              // ReceptorCollection[3] = { 10, 8, GAUCHE, 0, 0, 0, false }; // Receptor BLACK
              // ReceptorCollection[4] = { 8, 0, BAS, 255, 0, 255, false }; // Receptor PINK
              // ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
              ReceptorCollection[3] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
              // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

              DoorCollection[0] = { 6,0, HAUT, false, false, 0, { R_ROUGE,R_BLEU, R_VERT }, 1}; // Door NORTH
              DoorCollection[1] = { 0, 5, GAUCHE, false, true, 1, { R_BLEU, 3, -1 }, 2}; // Door WEST
              //DoorCollection[2] = { 12, 5, DROITE, false, true, 1, { R_VERT, -1, -1 }, 0}; // Door EAST
              //DoorCollection[3] = { 6, 10, BAS, false, false, 0, { -1 -1, -1 }, -1}; // Door SOUTH

              //TreasureCollection[0] = {2,2, T_RED, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[1] = {11,9, T_YELLOW, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[2] = {10,4, T_GREEN, false, PIERRE_BLANCHE, 100 };
              //TreasureCollection[3] = {1,1, T_BLUE, true, PIERRE_BLANCHE, 100 };
       }

}


void exitAndFree( void )
{
       free(BlocksCollection);
       free(ReceptorCollection);
       free(DoorCollection);
       free(TreasureCollection);
       free(ObstaclesCollection);
}

void initWalls( void )
{
       murs[0] = { HORIZONTAL, OFFSET_X+SIZE*1, OFFSET_X+SIZE*(SIZE_MAP_X-1), OFFSET_Y+SIZE };
       murs[1] = { VERTICAL, OFFSET_Y+SIZE*1, OFFSET_Y+SIZE*(SIZE_MAP_Y-1), OFFSET_X+SIZE*(SIZE_MAP_X-1) };
       murs[2] = { HORIZONTAL, OFFSET_X+SIZE*1, OFFSET_X+SIZE*(SIZE_MAP_X-1), OFFSET_Y+SIZE*(SIZE_MAP_Y-1) };
       murs[3] = { VERTICAL, OFFSET_Y+SIZE*1, OFFSET_Y+SIZE*(SIZE_MAP_Y-1), OFFSET_X+SIZE };

       for(unsigned char k=0; k<SIZE_MAP_X; k++)
              dprint_opt( OFFSET_X+SIZE*k+SIZE/2, OFFSET_Y-SIZE/2, C_WHITE, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE, "%d", k );

       for( unsigned char k=1; k<SIZE_MAP_Y; k++)
              dprint_opt( OFFSET_X-SIZE/2, OFFSET_Y+k*SIZE+SIZE/2, C_WHITE, C_NONE, DTEXT_CENTER, DTEXT_MIDDLE, "%d", k );
}

void renderMap( void )
{
       //dimage(OFFSET_X, OFFSET_Y, &mapbackgrd);
       unsigned int OFFSET_X_TILE = 0;

       OFFSET_X_TILE = 64*LEVELCOLOR;

       for(unsigned char x=0; x<SIZE_MAP_X; x++)
              for( unsigned char y=0; y<SIZE_MAP_Y; y++)
              {
                     unsigned int lX=OFFSET_X+x*SIZE;
                     unsigned int lY=OFFSET_Y+y*SIZE;

                     if(y==0)
                     {
                            if (x==0)
                                   dsubimage( lX, lY, &leveltiles, OFFSET_X_TILE+0,0,16,16,  DIMAGE_NONE);
                            else if (x==SIZE_MAP_X-1)
                                   dsubimage( lX, lY, &leveltiles, OFFSET_X_TILE+16,0,16,16,  DIMAGE_NONE);
                            else
                                   dsubimage( lX, lY, &leveltiles, OFFSET_X_TILE+32,0,16,16,  DIMAGE_NONE);
                     }
                     else if(y==SIZE_MAP_Y-1)
                     {
                            if (x==0)
                                   dsubimage( lX, lY, &leveltiles, OFFSET_X_TILE+0,16,16,16,  DIMAGE_NONE);
                            else if (x==SIZE_MAP_X-1)
                                   dsubimage( lX, lY, &leveltiles, OFFSET_X_TILE+16,16,16,16,  DIMAGE_NONE);
                            else
                                   dsubimage( lX, lY, &leveltiles, OFFSET_X_TILE+48,16,16,16,  DIMAGE_NONE);
                     }
                     else
                     {
                            if (x==0)
                                   dsubimage( lX, lY, &leveltiles, OFFSET_X_TILE+48,0,16,16,  DIMAGE_NONE);
                            else if (x==SIZE_MAP_X-1)
                                   dsubimage( lX, lY, &leveltiles, OFFSET_X_TILE+32,16,16,16,  DIMAGE_NONE);
                            else if ((x+y)%2==0)
                                   dsubimage( lX, lY, &leveltiles, OFFSET_X_TILE+0,32,16,16,  DIMAGE_NONE);
                            else
                                   dsubimage( lX, lY, &leveltiles, OFFSET_X_TILE+16,32,16,16,  DIMAGE_NONE);
                     }
              }
}

int computeLightModification( unsigned int currentCellX, unsigned int currentCellY, unsigned char* R, unsigned char* G, unsigned char* B, unsigned char* A )
{
       if (*A!=ALPHA_BLOQUE)
       {
              for( unsigned char k=0; k<NbBlocksLevel; k++ )
              {
                     if (BlocksCollection[k].x == currentCellX)
                            if (BlocksCollection[k].y == currentCellY)
                            {
                                   if (*R!=BlocNoir->R || *G!=BlocNoir->G || *B!=BlocNoir->B || *A!=ALPHA_OPAQUE )      // We check if the light is not already a black ray
                                   {
                                          if (BlocksCollection[k].type == OPAQUE)
                                          {
                                                 if (*R==lumiere.R && *G==lumiere.G && *B==lumiere.B && *A==lumiere.A)
                                                 {
                                                        *R = (*R-lumiere.R) & BlocksCollection[k].R;
                                                        *G = (*G-lumiere.G) & BlocksCollection[k].G;
                                                        *B = (*B-lumiere.B) & BlocksCollection[k].B;
                                                        *A = ALPHA_OPAQUE;
                                                 }
                                                 else
                                                 {
                                                        *R = *R & BlocksCollection[k].R;
                                                        *G = *G & BlocksCollection[k].G;
                                                        *B = *B & BlocksCollection[k].B;
                                                        *A = ALPHA_OPAQUE;
                                                 }
                                          }
                                          if (BlocksCollection[k].type == TRANSPARENT)
                                          {
                                                 if (*R==lumiere.R && *G==lumiere.G && *B==lumiere.B && *A==lumiere.A)
                                                 {
                                                        *R = (*R-lumiere.R) | BlocksCollection[k].R;
                                                        *G = (*G-lumiere.G) | BlocksCollection[k].G;
                                                        *B = (*B-lumiere.B) | BlocksCollection[k].B;
                                                        *A = ALPHA_TRANSPARENT;
                                                 }
                                                 else
                                                 {
                                                        *R = *R | BlocksCollection[k].R;
                                                        *G = *G | BlocksCollection[k].G;
                                                        *B = *B | BlocksCollection[k].B;
                                                        *A = ALPHA_TRANSPARENT;
                                                 }
                                          }
                                   }
                            }
              }

              for( unsigned char k=0; k<NbObstacleLevel; k++ )
              {
                     if (ObstaclesCollection[k].type!=BLOCK_WATER)
                            if (ObstaclesCollection[k].x == currentCellX)
                                   if (ObstaclesCollection[k].y == currentCellY)
                                   {
                                          *A = ALPHA_BLOQUE;
                                   }
              }

              if (*A == ALPHA_BLOQUE) return 1;
       }
       return 0;
}

void rayCast( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char* Rs, unsigned char* Gs, unsigned char* Bs, unsigned char* As )
{
       // Cette fonction est un algo type BRESENHAM qui lance un rayon depuis la lumi�re jusqu'� chaque pixel des murs
       // Elle analyse la pr�sence de blocs de glace de couleurs qui peuvent changer la couleur de la lumi�re du d�part (Rs, Gs, Bs)
       // Elle dessine le point sur le mur de la bonne couleur

       // ATTENTION : toujours mettre x1 et y1 comme �tant en premier et correspondant � la source de lumi�re !!!!


       char currentCellX = -1;
       char currentCellY = -1;

       unsigned int x,y;
       int dx,dy,swp,temp,s1,s2,p,i;

       x=x1;
       y=y1;
       dx=_abs(x2-x1);
       dy=_abs(y2-y1);
       s1=_sgn(x2-x1);
       s2=_sgn(y2-y1);
       swp=0;

       if(dy>dx)
       {
              temp=dx;
              dx=dy;
              dy=temp;
              swp=1;
       }
       p=2*dy-dx;
       for(i=0; i<dx; i+=SPEEDUP_RAY)
       {
              currentCellX = (x-OFFSET_X)/SIZE;
              currentCellY = (y-OFFSET_Y)/SIZE;
              int result = computeLightModification( currentCellX, currentCellY, Rs, Gs, Bs, As );


              if (result==1)
              {
                     if ((*Rs!=lumiere.R || *Gs!=lumiere.G || *Bs!=lumiere.B)  && *As==ALPHA_BLOQUE)
                            _boxRGBA( x-1, y-1, x+1, y+1,  *Rs, *Gs, *Bs, 255 );

                     // if (*Rs==lumiere.R && *Gs==lumiere.G && *Bs==lumiere.B && *As==ALPHA_BLOQUE)
                     //        _boxRGBA( x-1, y-1, x+1, y+1,  *Rs, *Gs, *Bs, 255 );
              }
              else if (result==0)
              {
                     _pixelRGBA(  x, y, *Rs, *Gs, *Bs, *As );
              }


              while(p>=0)
              {
                     p=p-2*dx;
                     if(swp)
                            x+=s1*SPEEDUP_RAY;
                     else
                            y+=s2*SPEEDUP_RAY;
              }
              p=p+2*dy;
              if(swp)
                     y+=s2*SPEEDUP_RAY;
              else
                     x+=s1*SPEEDUP_RAY;
       }
}

void renderWalls( void )
{
       unsigned char lR = lumiere.R;
       unsigned char lG = lumiere.G;
       unsigned char lB = lumiere.B;
       unsigned char lA = lumiere.A;

       unsigned int lX = SIZE*lumiere.x+OFFSET_X+SIZE/2;
       unsigned int lY = SIZE*lumiere.y+OFFSET_Y+SIZE/2;

       for( unsigned char k=0; k<4; k++ )
       {
              if (murs[k].direction == HORIZONTAL)
                     for( unsigned int z=murs[k].mini; z<=murs[k].maxi; z+=SPEEDUP_WALL)
                     {
                            lR = lumiere.R;
                            lG = lumiere.G;
                            lB = lumiere.B;
                            lA = lumiere.A;

                            rayCast( lX, lY, z, murs[k].fixe,  &lR,&lG,&lB,&lA );

                            if (lR!=lumiere.R || lG!=lumiere.G || lB!=lumiere.B || lA!=lumiere.A )
                            {
                                   if(lA!=ALPHA_BLOQUE)
                                   {
                                          _pixelRGBA(  z, murs[k].fixe+1,  lR,lG,lB, 255 );
                                          _pixelRGBA(  z, murs[k].fixe,  lR,lG,lB, 255 );
                                          _pixelRGBA(  z, murs[k].fixe-1,  lR,lG,lB, 255 );
                                   }
                            }
                     }

              if (murs[k].direction == VERTICAL)
                     for( unsigned int z=murs[k].mini; z<=murs[k].maxi; z+=SPEEDUP_WALL)
                     {
                            lR = lumiere.R;
                            lG = lumiere.G;
                            lB = lumiere.B;
                            lA = lumiere.A;

                            rayCast(  lX, lY, murs[k].fixe, z, &lR,&lG,&lB,&lA );

                            if (lR!=lumiere.R || lG!=lumiere.G || lB!=lumiere.B || lA!=lumiere.A )
                            {
                                   if (lA!=ALPHA_BLOQUE)
                                   {
                                          _pixelRGBA(  murs[k].fixe+1, z, lR,lG,lB,255 );
                                          _pixelRGBA(  murs[k].fixe, z, lR,lG,lB,255 );
                                          _pixelRGBA(  murs[k].fixe-1, z, lR,lG,lB,255 );
                                   }
                            }
                     }

       }
}

void renderObstacles( int passnumber )
{
       if (passnumber == LAYER_BELOW_RAYS)
       {

              for( unsigned char k=0; k<NbObstacleLevel; k++ )
              {
                     unsigned int Xb = ObstaclesCollection[k].x;
                     unsigned int Yb = ObstaclesCollection[k].y;
                     unsigned char Tb = ObstaclesCollection[k].type;

                     unsigned int lX = SIZE*Xb+OFFSET_X;
                     unsigned int lY = SIZE*Yb+OFFSET_Y;

                     if (Tb==BLOCK_WATER) dsubimage( lX, lY, &sprites, 64,112,16,16,  DIMAGE_NONE);
              }
       }
       else if (passnumber == LAYER_ABOVE_RAYS)
       {

              for( unsigned char k=0; k<NbObstacleLevel; k++ )
              {
                     unsigned int Xb = ObstaclesCollection[k].x;
                     unsigned int Yb = ObstaclesCollection[k].y;
                     unsigned char Tb = ObstaclesCollection[k].type;

                     unsigned int lX = SIZE*Xb+OFFSET_X;
                     unsigned int lY = SIZE*Yb+OFFSET_Y;

                     if (Tb==BLOCK_STONE) dsubimage( lX, lY, &sprites, 0,112,16,16,  DIMAGE_NONE);
                     else if (Tb==BLOCK_MOON) dsubimage( lX, lY, &sprites, 16,112,16,16,  DIMAGE_NONE);
                     else if (Tb==BLOCK_WOOD) dsubimage( lX, lY, &sprites, 32,112,16,16,  DIMAGE_NONE);
                     else if (Tb==BLOCK_SUN) dsubimage( lX, lY, &sprites, 48,112,16,16,  DIMAGE_NONE);
              }
       }
}

void renderTreasures( void )
{
       for( unsigned char k=0; k<NbTreasureLevel; k++ )
       {
              unsigned int Xb = TreasureCollection[k].x;
              unsigned int Yb = TreasureCollection[k].y;
              chest Tb = TreasureCollection[k].type;
              bool Ob = TreasureCollection[k].isopen;

              unsigned int lX = SIZE*Xb+OFFSET_X;
              unsigned int lY = SIZE*Yb+OFFSET_Y;

              if (Tb==T_RED)
              {
                     if (Ob==false)
                     {
                            dsubimage( lX, lY, &chests, 0,0,16,16,  DIMAGE_NONE);
                     }
                     else
                     {
                            dsubimage( lX, lY, &chests, 0,48,16,16,  DIMAGE_NONE);
                     }
              }
              else if (Tb==T_YELLOW)
              {
                     if (Ob==false)
                     {
                            dsubimage( lX, lY, &chests, 16,0,16,16,  DIMAGE_NONE);
                     }
                     else
                     {
                            dsubimage( lX, lY, &chests, 16,48,16,16,  DIMAGE_NONE);
                     }
              }
              else if (Tb==T_GREEN)
              {
                     if (Ob==false)
                     {
                            dsubimage( lX, lY, &chests, 32,0,16,16,  DIMAGE_NONE);
                     }
                     else
                     {
                            dsubimage( lX, lY, &chests, 32,48,16,16,  DIMAGE_NONE);
                     }
              }
              else if (Tb==T_BLUE)
              {
                     if (Ob==false)
                     {
                            dsubimage( lX, lY, &chests, 48,0,16,16,  DIMAGE_NONE);
                     }
                     else
                     {
                            dsubimage( lX, lY, &chests, 48,48,16,16,  DIMAGE_NONE);
                     }
              }
       }

}

void renderMovable( void )
{
       for( unsigned char k=0; k<NbBlocksLevel; k++ )
       {
              unsigned int Xb = BlocksCollection[k].x;
              unsigned int Yb = BlocksCollection[k].y;
              unsigned char Rb = BlocksCollection[k].R;
              unsigned char Gb = BlocksCollection[k].G;
              unsigned char Bb = BlocksCollection[k].B;
              //unsigned char Ab = BlocksCollection[k].A;

              unsigned int lX = SIZE*Xb+OFFSET_X;
              unsigned int lY = SIZE*Yb+OFFSET_Y;

              if (Rb==255 && Gb==0 && Bb==0) dsubimage( lX, lY, &sprites, 16,0,16,16,  DIMAGE_NONE);
              else if (Rb==0 && Gb==0 && Bb==255) dsubimage( lX, lY, &sprites, 32,0,16,16,  DIMAGE_NONE);
              else if (Rb==0 && Gb==255 && Bb==0) dsubimage( lX, lY, &sprites, 48,0,16,16,  DIMAGE_NONE);
              else if (Rb==0 && Gb==0 && Bb==0) dsubimage( lX, lY, &sprites, 64,0,16,16,  DIMAGE_NONE);
              else if (Rb==255 && Gb==255 && Bb==255) dsubimage( lX, lY, &sprites, 0,0,16,16,  DIMAGE_NONE);
       }

}

void renderPlayer( void )
{
       unsigned int lX = SIZE*joueur.x+OFFSET_X;
       unsigned int lY = SIZE*joueur.y+OFFSET_Y;

       if (direction==BAS)
       {
              if (mouvement==true)
              {
                     if (frame==0) dsubimage( lX, lY-11, &player, 0,0,16,16,  DIMAGE_NONE);
                     else if (frame==1) dsubimage( lX, lY-6, &player, 16,0,16,16,  DIMAGE_NONE);
                     else if (frame==2) dsubimage( lX, lY, &player, 32,0,16,16,  DIMAGE_NONE);

                     frame++;
                     if  (frame==3) mouvement = false;
              }
              else
              {
                     dsubimage( lX, lY, &player, 0,0,16,16,  DIMAGE_NONE);
                     frame = 0;
              }
       }
       else if (direction==DROITE)
       {
              if (mouvement==true)
              {
                     if (frame==0) dsubimage( lX-11, lY, &player, 0,16,16,16,  DIMAGE_NONE);
                     else if (frame==1) dsubimage( lX-6, lY, &player, 16,16,16,16,  DIMAGE_NONE);
                     else if (frame==2) dsubimage( lX, lY, &player, 32,16,16,16,  DIMAGE_NONE);

                     frame++;
                     if  (frame==3) mouvement = false;
              }
              else
              {
                     dsubimage( lX, lY, &player, 0,16,16,16,  DIMAGE_NONE);
                     frame = 0;
              }
       }
       else if (direction==HAUT)
       {
              if (mouvement==true)
              {
                     if (frame==0) dsubimage( lX, lY+11, &player, 0,32,16,16,  DIMAGE_NONE);
                     else if (frame==1) dsubimage( lX, lY+6, &player, 16,32,16,16,  DIMAGE_NONE);
                     else if (frame==2) dsubimage( lX, lY, &player, 32,32,16,16,  DIMAGE_NONE);

                     frame++;
                     if  (frame==3) mouvement = false;
              }
              else
              {
                     dsubimage( lX, lY, &player, 0,32,16,16,  DIMAGE_NONE);
                     frame = 0;
              }
       }
       else if (direction==GAUCHE)
       {
              if (mouvement==true)
              {
                     if (frame==0) dsubimage( lX+11, lY, &player, 0,48,16,16,  DIMAGE_NONE);
                     else if (frame==1) dsubimage( lX+6, lY, &player, 16,48,16,16,  DIMAGE_NONE);
                     else if (frame==2) dsubimage( lX, lY, &player, 32,48,16,16,  DIMAGE_NONE);

                     frame++;
                     if  (frame==3) mouvement = false;
              }
              else
              {
                     dsubimage( lX, lY, &player, 0,48,16,16,  DIMAGE_NONE);
                     frame = 0;
              }
       }
}

void renderLight( void )
{
       unsigned int lX = SIZE*lumiere.x+OFFSET_X;
       unsigned int lY = SIZE*lumiere.y+OFFSET_Y;

       frame_light%=6;
       dsubimage( lX, lY, &light, 16*frame_light,0,16,16,  DIMAGE_NONE);
       frame_light++;
}

void drawMinimap( void )
{
       for( unsigned char k=0; k< NB_LEVELS; k++ )
       {
              if (Map[k].visited==true)
              {
                     unsigned char R= Map[k].R;
                     unsigned char G= Map[k].G;
                     unsigned char B= Map[k].B;
                     unsigned char A= Map[k].A;

                     unsigned int lX = OFFSET_X_MINIMAP + Map[k].x*(SCALE_X_MINIMAP+2);
                     unsigned int lY = OFFSET_Y_MINIMAP + Map[k].y*(SCALE_Y_MINIMAP+2);

                     if (k==currentLevel)
                            _boxRGBA( lX-4, lY-3, lX+4, lY+3, 0, 255, 0, A/2);

                     _rectangleRGBA( lX-4, lY-3, lX+4, lY+3, R, G, B, A);
              }
       }
}

void drawInterface( void )
{
       unsigned int midX, midY;

       dsubimage( 232, 0, &parchemin, 0,0, 164, 210,  DIMAGE_NONE);
       dfont( &font_fantasy );

       dprint( 256, 36, C_RGB(150,150,150), "MAGIC Light v0.3B");
       dprint( 255, 35, C_BLACK, "MAGIC Light v0.3B");

       dfont( &font_tiny );

       dprint( 251, 56, C_RGB(150,150,150), "Score : %d", score );
       dprint( 250, 55, C_BLACK, "Score : %d", score );

       dprint( 251, 66, C_RGB(150,150,150), "Level : %d", currentLevel );
       dprint( 250, 65, C_BLACK, "Level : %d",  currentLevel );


       midY=(200+224)/2;

       midX=(66)/2;
       dsubimage( midX-8, midY-8, &player, 0,0,16,16,  DIMAGE_NONE);

       midX+=66;
       dsubimage( midX-8, midY-8, &light, 0,0,16,16,  DIMAGE_NONE);

       midX+=66;
       dsubimage( midX-8, midY-8, &sprites, 16,0,16,16,  DIMAGE_NONE);

       midX+=66;
       dsubimage( midX-8, midY-8, &sprites, 48,0,16,16,  DIMAGE_NONE);

       midX+=66;
       dsubimage( midX-8, midY-8, &sprites, 32,0,16,16,  DIMAGE_NONE);

       midX+=66;
       dsubimage( midX-8, midY-8, &sprites, 64,0,16,16,  DIMAGE_NONE);

       if (selected >=1 && selected <=6)
       {

              if (frame_cursor==0) dsubimage( 66*(selected)-33-11, midY-11, &cursor, 0,0,22,22,  DIMAGE_NONE);
              else if (frame_cursor==1) dsubimage( 66*(selected)-33-11, midY-11, &cursor, 22,0,22,22,  DIMAGE_NONE);
              else if (frame_cursor==2) dsubimage( 66*(selected)-33-11, midY-11, &cursor, 44,0,22,22,  DIMAGE_NONE);
              else if (frame_cursor==3) dsubimage( 66*(selected)-33-11, midY-11, &cursor, 22,0,22,22,  DIMAGE_NONE);
       }
}

bool isValidMove( selection selected, unsigned char x, unsigned char y, orientations sens )
{
       for(unsigned char k=0; k<NbDoorsLevel; k++)
       {
              if ( x==DoorCollection[k].x && y==DoorCollection[k].y)
              {
                     if (DoorCollection[k].isdoor && DoorCollection[k].isopen)
                     {
                            loadLevel(  DoorCollection[k].nextLevelNumber );
                            joueur = { 11-10*x/12, 9-8*y/10, sens };
                            return false;
                     }
              }
       }

       if (x<1 || x>SIZE_MAP_X-2 || y<1 || y>SIZE_MAP_Y-2)
       {
              return false;
       }
       else if (selected == JOUEUR)
       {
              if (x==lumiere.x && y==lumiere.y) return false;
              else if (x==BlocRouge->x && y==BlocRouge->y) return false;
              else if (x==BlocVert->x && y==BlocVert->y) return false;
              else if (x==BlocBleu->x && y==BlocBleu->y) return false;
              else if (x==BlocNoir->x && y==BlocNoir->y) return false;
              else
              {
                     for (unsigned char k =0; k<NbObstacleLevel; k++)
                            if (ObstaclesCollection[k].x==x && ObstaclesCollection[k].y==y)
                                   return false;
                     return true;
              }
       }
       else if (selected == LUMIERE)
       {
              if (x==joueur.x && y==joueur.y) return false;
              else
              {
                     for (unsigned char k =0; k<NbObstacleLevel; k++)
                            if (ObstaclesCollection[k].x==x && ObstaclesCollection[k].y==y)
                                   return false;
                     return true;
              }
       }
       else if (selected == ROUGE)
       {
              if (x==joueur.x && y==joueur.y) return false;
              else if (x==BlocVert->x && y==BlocVert->y) return false;
              else if (x==BlocBleu->x && y==BlocBleu->y) return false;
              else if (x==BlocNoir->x && y==BlocNoir->y) return false;
              else
              {
                     for (unsigned char k =0; k<NbObstacleLevel; k++)
                            if (ObstaclesCollection[k].x==x && ObstaclesCollection[k].y==y)
                                   return false;
                     return true;
              }
       }
       else if (selected == VERT)
       {
              if (x==joueur.x && y==joueur.y) return false;
              else if (x==BlocRouge->x && y==BlocRouge->y) return false;
              else if (x==BlocBleu->x && y==BlocBleu->y) return false;
              else if (x==BlocNoir->x && y==BlocNoir->y) return false;
              else
              {
                     for (unsigned char k =0; k<NbObstacleLevel; k++)
                            if (ObstaclesCollection[k].x==x && ObstaclesCollection[k].y==y)
                                   return false;
                     return true;
              }
       }
       else if (selected == BLEU)
       {
              if (x==joueur.x && y==joueur.y) return false;
              else if (x==BlocRouge->x && y==BlocRouge->y) return false;
              else if (x==BlocVert->x && y==BlocVert->y) return false;
              else if (x==BlocNoir->x && y==BlocNoir->y) return false;
              else
              {
                     for (unsigned char k =0; k<NbObstacleLevel; k++)
                            if (ObstaclesCollection[k].x==x && ObstaclesCollection[k].y==y)
                                   return false;
                     return true;
              }
       }
       else if (selected == NOIR)
       {
              if (x==joueur.x && y==joueur.y) return false;
              else if (x==BlocRouge->x && y==BlocRouge->y) return false;
              else if (x==BlocVert->x && y==BlocVert->y) return false;
              else if (x==BlocBleu->x && y==BlocBleu->y) return false;
              else
              {
                     for (unsigned char k =0; k<NbObstacleLevel; k++)
                            if (ObstaclesCollection[k].x==x && ObstaclesCollection[k].y==y)
                                   return false;
                     return true;
              }
       }
       return false;
}

void gameMechanics(  selection what, orientations touche )
{
       if (touche==GAUCHE)
       {
              if (what==JOUEUR && isValidMove(JOUEUR, joueur.x-1, joueur.y, touche ))
              {
                     joueur.x--;
                     frame=0;
                     direction=GAUCHE;
                     mouvement=true;
              }
              else if (what==JOUEUR && !isValidMove(JOUEUR, joueur.x-1, joueur.y, touche))
              {
                     frame=0;
                     direction=GAUCHE;
              }
              else if (what==LUMIERE && isValidMove(LUMIERE, lumiere.x-1, lumiere.y, touche))     lumiere.x--;
              else if (what==ROUGE && isValidMove(ROUGE, BlocRouge->x-1, BlocRouge->y, touche ))     BlocRouge->x--;
              else if (what==VERT && isValidMove(VERT, BlocVert->x-1, BlocVert->y, touche))     BlocVert->x--;
              else if (what==BLEU && isValidMove(BLEU, BlocBleu->x-1, BlocBleu->y, touche))     BlocBleu->x--;
              else if (what==NOIR && isValidMove(NOIR, BlocNoir->x-1, BlocNoir->y, touche))     BlocNoir->x--;
       }
       if (touche==DROITE)
       {
              if (what==JOUEUR && isValidMove(JOUEUR, joueur.x+1, joueur.y, touche))
              {
                     joueur.x++;
                     frame=0;
                     direction=DROITE;
                     mouvement=true;
              }
              else if (what==JOUEUR && !isValidMove(JOUEUR, joueur.x+1, joueur.y, touche))
              {
                     frame=0;
                     direction=DROITE;
              }
              else if (what==LUMIERE && isValidMove(LUMIERE, lumiere.x+1, lumiere.y, touche))     lumiere.x++;
              else if (what==ROUGE && isValidMove(ROUGE, BlocRouge->x+1, BlocRouge->y, touche))     BlocRouge->x++;
              else if (what==VERT && isValidMove(VERT, BlocVert->x+1, BlocVert->y, touche))     BlocVert->x++;
              else if (what==BLEU && isValidMove(BLEU, BlocBleu->x+1, BlocBleu->y, touche))     BlocBleu->x++;
              else if (what==NOIR && isValidMove(NOIR, BlocNoir->x+1, BlocNoir->y, touche))     BlocNoir->x++;
       }
       if (touche==HAUT)
       {
              if (what==JOUEUR && isValidMove(JOUEUR, joueur.x, joueur.y-1, touche))
              {
                     joueur.y--;
                     frame=0;
                     direction=HAUT;
                     mouvement=true;
              }
              else if (what==JOUEUR && !isValidMove(JOUEUR, joueur.x, joueur.y-1, touche))
              {
                     frame=0;
                     direction=HAUT;
              }
              else if (what==LUMIERE && isValidMove(LUMIERE, lumiere.x, lumiere.y-1, touche))     lumiere.y--;
              else if (what==ROUGE && isValidMove(ROUGE, BlocRouge->x, BlocRouge->y-1, touche))     BlocRouge->y--;
              else if (what==VERT && isValidMove(VERT, BlocVert->x, BlocVert->y-1, touche))     BlocVert->y--;
              else if (what==BLEU && isValidMove(BLEU, BlocBleu->x, BlocBleu->y-1, touche))     BlocBleu->y--;
              else if (what==NOIR && isValidMove(NOIR, BlocNoir->x, BlocNoir->y-1, touche))     BlocNoir->y--;
       }
       if (touche==BAS)
       {
              if (what==JOUEUR && isValidMove(JOUEUR, joueur.x, joueur.y+1, touche))
              {
                     joueur.y++;
                     frame=0;
                     direction=BAS;
                     mouvement=true;
              }
              else if (what==JOUEUR && !isValidMove(JOUEUR, joueur.x, joueur.y+1, touche))
              {
                     frame=0;
                     direction=BAS;
              }
              else if (what==LUMIERE && isValidMove(LUMIERE, lumiere.x, lumiere.y+1, touche))     lumiere.y++;
              else if (what==ROUGE && isValidMove(ROUGE, BlocRouge->x, BlocRouge->y+1, touche))     BlocRouge->y++;
              else if (what==VERT && isValidMove(VERT, BlocVert->x, BlocVert->y+1, touche))     BlocVert->y++;
              else if (what==BLEU && isValidMove(BLEU, BlocBleu->x, BlocBleu->y+1, touche))     BlocBleu->y++;
              else if (what==NOIR && isValidMove(NOIR, BlocNoir->x, BlocNoir->y+1, touche))     BlocNoir->y++;
       }
}

void renderReceptors( void )
{
       for( unsigned char k=0; k<NbReceptorsLevel; k++ )
       {
              unsigned int lX = SIZE*ReceptorCollection[k].x+OFFSET_X+SIZE/2;
              unsigned int lY = SIZE*ReceptorCollection[k].y+OFFSET_Y+SIZE/2;
              unsigned char Rr = ReceptorCollection[k].R;
              unsigned char Gr = ReceptorCollection[k].G;
              unsigned char Br = ReceptorCollection[k].B;
              unsigned int direction = ReceptorCollection[k].direction;
              bool valide = ReceptorCollection[k].isvalide;

              unsigned char Rc,Gc,Bc;

              if (valide==false)
              {
                     Rc=0;
                     Gc=0;
                     Bc=0;
              }
              else
              {
                     Rc=255;
                     Gc=255;
                     Bc=255;
              }

              _boxRGBA(  lX-4, lY-4, lX+4, lY+4, Rc, Gc, Bc, 255 );
              _boxRGBA(  lX-2, lY-2, lX+2, lY+2, Rr, Gr, Br, 255 );


              if (direction==GAUCHE)
              {
                     _lineRGBA(  lX-4, lY-1, lX-8, lY-1, Rc, Gc, Bc, 255 );
                     _lineRGBA(  lX-4, lY, lX-8, lY, Rc, Gc, Bc, 255 );
                     _lineRGBA(  lX-4, lY+1, lX-8, lY+1, Rc, Gc, Bc, 255 );
              }
              else if (direction==DROITE)
              {
                     _lineRGBA(  lX+4, lY-1, lX+8, lY-1, Rc, Gc, Bc, 255 );
                     _lineRGBA(  lX+4, lY, lX+8, lY, Rc, Gc, Bc, 255 );
                     _lineRGBA(  lX+4, lY+1, lX+8, lY+1, Rc, Gc, Bc, 255 );
              }
              else if (direction==HAUT)
              {
                     _lineRGBA(  lX-1, lY-4, lX-1, lY-8, Rc, Gc, Bc, 255 );
                     _lineRGBA(  lX, lY-4, lX, lY-8, Rc, Gc, Bc, 255 );
                     _lineRGBA(  lX+1, lY-4, lX+1, lY-8, Rc, Gc, Bc, 255 );
              }
              else if (direction==BAS)
              {
                     _lineRGBA(  lX-1, lY+4, lX-1, lY+8, Rc, Gc, Bc, 255 );
                     _lineRGBA(  lX, lY+4, lX, lY+8, Rc, Gc, Bc, 255 );
                     _lineRGBA(  lX+1, lY+4, lX+1, lY+8, Rc, Gc, Bc, 255 );
              }

       }
}

void checkReceptors( void  )
{
       for( unsigned char k=0; k<NbReceptorsLevel; k++ )
       {
              unsigned int lX1=0, lX2=0, lX3=0;
              unsigned int lY1=0, lY2=0, lY3=0;

              unsigned int lX = SIZE*ReceptorCollection[k].x+OFFSET_X+SIZE/2;
              unsigned int lY = SIZE*ReceptorCollection[k].y+OFFSET_Y+SIZE/2;
              unsigned char Rr = ReceptorCollection[k].R;
              unsigned char Gr = ReceptorCollection[k].G;
              unsigned char Br = ReceptorCollection[k].B;
              unsigned int direction = ReceptorCollection[k].direction;

              if (direction==GAUCHE)
              {
                     // all 7 changed to 8
                     lX1=lX-7;
                     lX2=lX-7;
                     lX3=lX-7;
                     lY1=lY-1;
                     lY2=lY;
                     lY3=lY+1;
              }
              else if (direction==DROITE)
              {
                     lX1=lX+7;
                     lX2=lX+7;
                     lX3=lX+7;
                     lY1=lY-1;
                     lY2=lY;
                     lY3=lY+1;
              }
              else if (direction==HAUT)
              {
                     lX1=lX-1;
                     lX2=lX;
                     lX3=lX+1;
                     lY1=lY-7;
                     lY2=lY-7;
                     lY3=lY-7;
              }
              else if (direction==BAS)
              {
                     lX1=lX-1;
                     lX2=lX;
                     lX3=lX+1;
                     lY1=lY+7;
                     lY2=lY+7;
                     lY3=lY+7;
              }

              unsigned char R1, G1, B1;
              unsigned char R2, G2, B2;
              unsigned char R3, G3, B3;
              unsigned char Rm, Gm, Bm;

              _getPixelRGB(  lX1, lY1, &R1, &G1, &B1 );
              _getPixelRGB(  lX2, lY2, &R2, &G2, &B2 );
              _getPixelRGB(  lX3, lY3, &R3, &G3, &B3 );

              Rm = (R1+R2+R3)/3;
              Gm = (G1+G2+G3)/3;
              Bm = (B1+B2+B3)/3;

//              _boxRGBA( 310, 10+k*30, 330, 30+k*30, Rm, Gm, Bm, 255 );
//              _rectangleRGBA( 310, 10+k*30, 330, 30+k*30, 0, 0, 0, 255 );

//              _boxRGBA( 340, 10+k*30, 360, 30+k*30, Rr, Gr, Br, 255 );
//              _rectangleRGBA( 340, 10+k*30, 360, 30+k*30, 0, 0, 0, 255 );


              if (Rm>=Rr-EPSILON && Rm<=Rr+EPSILON && Gm>=Gr-EPSILON && Gm<=Gr+EPSILON && Bm>=Br-EPSILON && Bm<=Br+EPSILON)
              {
                     ReceptorCollection[k].isvalide = true;
              }
              else
              {
                     ReceptorCollection[k].isvalide = false;
              }
       }
}

void renderDoors( void )
{
       for( unsigned char k=0; k<NbDoorsLevel; k++ )
       {
              unsigned int lX = SIZE*DoorCollection[k].x+OFFSET_X;
              unsigned int lY = SIZE*DoorCollection[k].y+OFFSET_Y;

              int OFFSET_Y_TILE = 16*LEVELCOLOR;

              if (DoorCollection[k].isdoor==false) // check if it is a door or a wall (draw walls)
              {
                     if (DoorCollection[k].direction == HAUT)
                     {
                            dsubimage( lX, lY, &sprites, 0,OFFSET_Y_TILE+16,16,16,  DIMAGE_NONE);
                     }
                     else if (DoorCollection[k].direction == GAUCHE)
                     {
                            dsubimage( lX, lY, &sprites, 16,OFFSET_Y_TILE+16,16,16,  DIMAGE_NONE);
                     }
                     else if (DoorCollection[k].direction == BAS)
                     {
                            dsubimage( lX, lY, &sprites, 32,OFFSET_Y_TILE+16,16,16,  DIMAGE_NONE);
                     }
                     else if (DoorCollection[k].direction == DROITE)
                     {
                            dsubimage( lX, lY, &sprites, 48,OFFSET_Y_TILE+16,16,16,  DIMAGE_NONE);
                     }
              }
              else if (DoorCollection[k].isopen==false) // check if the door is open (draw stairs)
              {
                     if (DoorCollection[k].direction == HAUT)
                     {
                            dsubimage( lX, lY, &sprites, 0,80,16,16,  DIMAGE_NONE);
                     }
                     else if (DoorCollection[k].direction == GAUCHE)
                     {
                            dsubimage( lX, lY, &sprites, 16,80,16,16,  DIMAGE_NONE);
                     }
                     else if (DoorCollection[k].direction == BAS)
                     {
                            dsubimage( lX, lY, &sprites, 32,80,16,16,  DIMAGE_NONE);
                     }
                     else if (DoorCollection[k].direction == DROITE)
                     {
                            dsubimage( lX, lY, &sprites, 48,80,16,16,  DIMAGE_NONE);
                     }
              }
              else  // check if the door is open (draw doors)
              {
                     if (DoorCollection[k].direction == HAUT)
                     {
                            dsubimage( lX, lY, &sprites, 0,96,16,16,  DIMAGE_NONE);
                     }
                     else if (DoorCollection[k].direction == GAUCHE)
                     {
                            dsubimage( lX, lY, &sprites, 16,96,16,16,  DIMAGE_NONE);
                     }
                     else if (DoorCollection[k].direction == BAS)
                     {
                            dsubimage( lX, lY, &sprites, 32,96,16,16,  DIMAGE_NONE);
                     }
                     else if (DoorCollection[k].direction == DROITE)
                     {
                            dsubimage( lX, lY, &sprites, 48,96,16,16,  DIMAGE_NONE);
                     }
              }
       }

}

void checkDoors( void )
{
       for( unsigned char k=0; k<NbDoorsLevel; k++ )
       {
              if (DoorCollection[k].isdoor==true) // check if it is a door or a wall
              {
                     bool temp = true;
                     for(int j=0; j<DoorCollection[k].nbreceptor; j++ )
                     {
                            int receptorReference =  DoorCollection[k].receptors[j];
                            bool currentrecept = ReceptorCollection[receptorReference].isvalide;
                            Receptor* tempRecept = &ReceptorCollection[receptorReference];

                            temp=temp && currentrecept;

                            if (currentrecept)
                            {
                                   if(DoorCollection[k].direction==HAUT)
                                   {
                                          int X = DoorCollection[k].x*SIZE+OFFSET_X;
                                          int Y = DoorCollection[k].y*SIZE-SIZE/2+OFFSET_Y;

                                          if (DoorCollection[k].nbreceptor==1)
                                          {
                                                 _boxRGBA( X+SIZE/2-1, Y-1, X+SIZE/2+1, Y+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X+SIZE/2-2, Y-2, X+SIZE/2+2, Y+2, 150, 150, 150, 255);
                                          }
                                          else if (DoorCollection[k].nbreceptor==2)
                                          {
                                                 _boxRGBA( X+SIZE/3*(j+1)-1, Y-1, X+SIZE/3*(j+1)+1, Y+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X+SIZE/3*(j+1)-2, Y-2, X+SIZE/3*(j+1)+2, Y+2, 150, 150, 150, 255);
                                          }
                                          else if (DoorCollection[k].nbreceptor==3)
                                          {
                                                 _boxRGBA( X+SIZE/3*(j+1)-1, Y-1, X+SIZE/3*(j+1)+1, Y+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X+SIZE/3*(j+1)-2, Y-2, X+SIZE/3*(j+1)+2, Y+2, 150, 150, 150, 255);
                                          }
                                   }
                                   else if(DoorCollection[k].direction==BAS)
                                   {
                                          int X = DoorCollection[k].x*SIZE+OFFSET_X;
                                          int Y = DoorCollection[k].y*SIZE+3*SIZE/2+OFFSET_Y;

                                          if (DoorCollection[k].nbreceptor==1)
                                          {
                                                 _boxRGBA( X+SIZE/2-1, Y-1, X+SIZE/2+1, Y+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X+SIZE/2-2, Y-2, X+SIZE/2+2, Y+2, 150, 150, 150, 255);
                                          }
                                          else if (DoorCollection[k].nbreceptor==2)
                                          {
                                                 _boxRGBA( X+SIZE/3*(j+1)-1, Y-1, X+SIZE/3*(j+1)+1, Y+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X+SIZE/3*(j+1)-2, Y-2, X+SIZE/3*(j+1)+2, Y+2, 150, 150, 150, 255);
                                          }
                                          else if (DoorCollection[k].nbreceptor==3)
                                          {
                                                 _boxRGBA( X+SIZE/3*(j+1)-1, Y-1, X+SIZE/3*(j+1)+1, Y+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X+SIZE/3*(j+1)-2, Y-2, X+SIZE/3*(j+1)+2, Y+2, 150, 150, 150, 255);
                                          }
                                   }
                                   else if(DoorCollection[k].direction==GAUCHE)
                                   {
                                          int X = DoorCollection[k].x*SIZE-SIZE/2+OFFSET_X;
                                          int Y = DoorCollection[k].y*SIZE+OFFSET_Y;

                                          if (DoorCollection[k].nbreceptor==1)
                                          {
                                                 _boxRGBA( X-1, Y+SIZE/2-1, X+1, Y+SIZE/2+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X-2, Y+SIZE/2-2, X+2, Y+SIZE/2+2, 150, 150, 150, 255);
                                          }
                                          else if (DoorCollection[k].nbreceptor==2)
                                          {
                                                 _boxRGBA( X-1, Y+SIZE/3*(j+1)-1, X+1, Y+SIZE/3*(j+1)+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X-2, Y+SIZE/3*(j+1)-2, X+2, Y+SIZE/3*(j+1)+2, 150, 150, 150, 255);
                                          }
                                          else if (DoorCollection[k].nbreceptor==3)
                                          {
                                                 _boxRGBA( X-1, Y+SIZE/4*(j+1)-1, X+1, Y+SIZE/4*(j+1)+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X-2, Y+SIZE/4*(j+1)-2, X+2, Y+SIZE/4*(j+1)+2, 150, 150, 150, 255);
                                          }
                                   }
                                   else if(DoorCollection[k].direction==DROITE)
                                   {
                                          int X = DoorCollection[k].x*SIZE+3*SIZE/2+OFFSET_X;
                                          int Y = DoorCollection[k].y*SIZE+OFFSET_Y;

                                          if (DoorCollection[k].nbreceptor==1)
                                          {
                                                 _boxRGBA( X-1, Y+SIZE/2-1, X+1, Y+SIZE/2+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X-2, Y+SIZE/2-2, X+2, Y+SIZE/2+2, 150, 150, 150, 255);
                                          }
                                          else if (DoorCollection[k].nbreceptor==2)
                                          {
                                                 _boxRGBA( X-1, Y+SIZE/3*(j+1)-1, X+1, Y+SIZE/3*(j+1)+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X-2, Y+SIZE/3*(j+1)-2, X+2, Y+SIZE/3*(j+1)+2, 150, 150, 150, 255);
                                          }
                                          else if (DoorCollection[k].nbreceptor==3)
                                          {
                                                 _boxRGBA( X-1, Y+SIZE/4*(j+1)-1, X+1, Y+SIZE/4*(j+1)+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                                                 _rectangleRGBA( X-2, Y+SIZE/4*(j+1)-2, X+2, Y+SIZE/4*(j+1)+2, 150, 150, 150, 255);
                                          }
                                   }
                            }
                     }
                     DoorCollection[k].isopen=temp;
              }
       }
}

int my_profile(int key, int duration, int count)
{
       // key: Key being repeated
       // duration: How long that key has been pressed
       // count: How many times it's been repeated already
       // Return value: how long to wait before next repeat (�s)

       // Only allow KEY_LEFT and KEY_RIGHT
       if(key != KEY_LEFT && key != KEY_RIGHT)
              return -1;

       // 500 ms for first repeat, 50 ms after that; stop after 10 repeats
       if(count == 0)
              return 500*1000;
       else
              return 50*1000;

       /*else if(count < 10)
              return 50*1000;
       else
              return -1;
        */
}

static int get_inputs_option(void)
{
       //int opt = GETKEY_DEFAULT & ~GETKEY_REP_ARROWS;
       //int timeout = 1;

       getkey_set_repeat_profile(my_profile);
       int opt = GETKEY_DEFAULT | GETKEY_REP_PROFILE;
       int timeout = 1;
//getkey_opt(opt, &timeout);

       while(1)
       {
              key_event_t ev = getkey_opt(opt, &timeout);
              if(ev.type == KEYEV_NONE) return -1;

              int key = ev.key;

              if (key==KEY_EXIT)
              {
                     doneOption = true;
              }
              if (key==KEY_UP && selectOptionMenu>0)
              {
                     selectOptionMenu--;
              }
              if (key==KEY_DOWN && selectOptionMenu<3)
              {
                     selectOptionMenu++;
              }
              if (key==KEY_LEFT && selectOptionMenu==1)
              {
                     if (ALPHA_TRANSPARENT>0)
                            ALPHA_TRANSPARENT--;
              }
              if (key==KEY_RIGHT && selectOptionMenu==1)
              {
                     if (ALPHA_TRANSPARENT<255)
                            ALPHA_TRANSPARENT++;
              }
              if (key==KEY_LEFT && selectOptionMenu==2)
              {
                     if (ALPHA_OPAQUE>0)
                            ALPHA_OPAQUE--;
              }
              if (key==KEY_RIGHT && selectOptionMenu==2)
              {
                     if (ALPHA_OPAQUE<255)
                            ALPHA_OPAQUE++;
              }
              if (key==KEY_EXE && selectOptionMenu==3)
              {
                     ALPHA_OPAQUE = ALPHA_OPAQUE_DEFAULT;
                     ALPHA_TRANSPARENT = ALPHA_TRANSPARENT_DEFAULT;
              }
              if (key ==KEY_EXE && selectOptionMenu!=3)
              {
                     doneOption = true;
              }
       }
}

void drawOption( void )
{
       doneOption = false;

       unsigned int lX = 0;
       unsigned int lY = 0;

       frame_light = 0;

       while (!doneOption)
       {
              dclear(C_RGB(0,0,0));

              dimage( 0, 10, &maintitle);

              dfont( &font_fantasy );
              dprint( 90, 95, C_RGB(150,150,150), "Back to Game");
              dprint( 89, 94, C_BLACK, "Back to Game");

              dfont( &font_fantasy );
              dprint( 90, 110, C_RGB(150,150,150), "Adjust Color Alpha");
              dprint( 89, 109, C_BLACK, "Adjust Color Alpha");

              dfont( &font_fantasy );
              dprint( 90, 125, C_RGB(150,150,150), "Adjust Black Alpha");
              dprint( 89, 124, C_BLACK, "Adjust Black Alpha");

              dfont( &font_fantasy );
              dprint( 90, 140, C_RGB(150,150,150), "Set Default Settings");
              dprint( 89, 139, C_BLACK, "Set Default Settings");

              dfont( &font_tiny );

              _boxRGBA( 220, 105,  253, 117,  255, 0, 0, ALPHA_TRANSPARENT );
              _boxRGBA( 254, 105,  286, 117,  0, 255, 0, ALPHA_TRANSPARENT );
              _boxRGBA( 287, 105,  320, 117,  0, 0, 255, ALPHA_TRANSPARENT );
              _rectangleRGBA(220, 105,  320, 117,  255, 255, 255, 255);

              dprint( 265, 107, C_BLACK, "%d", ALPHA_TRANSPARENT);

              _boxRGBA( 220, 120,  320, 132,  0, 0, 0, ALPHA_OPAQUE );
              _rectangleRGBA(220, 120,  320, 132,  255, 255, 255, 255);

              dprint( 265, 122, C_WHITE, "%d", ALPHA_OPAQUE);

              if (selectOptionMenu == 0)
              {
                     lX = 70;
                     lY = 89;
              }
              else if (selectOptionMenu == 1)
              {
                     lX = 70;
                     lY = 104;
              }
              else if (selectOptionMenu == 2)
              {
                     lX = 70;
                     lY = 119;
              }
              else if (selectOptionMenu == 3)
              {
                     lX = 70;
                     lY = 134;
              }

              frame_light%=6;
              dsubimage( lX, lY, &light, 16*frame_light,0,16,16,  DIMAGE_NONE);
              frame_light++;


              dfont( &font_tiny );
              dprint( 90, 160, C_RGB(150,150,150), "[UP] [DOWN] : select / [LEFT] [RIGHT] : adjust");
              dprint( 89, 159, C_BLACK, "[UP] [DOWN] : select / [LEFT] [RIGHT] : adjust");
              dprint( 90, 170, C_RGB(150,150,150), "[EXE] : validate / [EXIT] : escape");
              dprint( 89, 169, C_BLACK,"[EXE] : validate / [EXIT] : escape");

              dupdate();

              get_inputs_option();
       }
}

static int get_inputs_quit(void)
{
       int opt = GETKEY_DEFAULT & ~GETKEY_REP_ARROWS;
       int timeout = 1;

       while(1)
       {
              key_event_t ev = getkey_opt(opt, &timeout);
              if(ev.type == KEYEV_NONE) return -1;

              int key = ev.key;

              if (key==KEY_EXIT)
              {
                     donePause = true;
                     selectOptionPause = 0;
              }
              if (key==KEY_UP && selectOptionPause>0)
              {
                     selectOptionPause--;
              }
              if (key==KEY_DOWN && selectOptionPause<1)
              {
                     selectOptionPause++;
              }
              if (key==KEY_EXE)
              {
                     donePause = true;
              }
       }
}

void drawQuit( void )
{
       donePause = false;

       unsigned int lX = 0;
       unsigned int lY = 0;

       frame_light = 0;

       while (!donePause)
       {
              dclear(C_RGB(0,0,0));

              dimage( 0, 10, &maintitle);
              dfont( &font_fantasy );
              dprint( 90, 95, C_RGB(150,150,150), "Game Currently in Pause Mode");
              dprint( 89, 94, C_BLACK, "Game Currently in Pause Mode");

              dfont( &font_fantasy );
              dprint( 90, 105, C_RGB(150,150,150), "Are You Sure You Want To Leave ?");
              dprint( 89, 104, C_BLACK, "Are You Sure You Want To Leave ?");

              dfont( &font_fantasy );
              dprint( 90, 125, C_RGB(150,150,150), "No - Get Me Back To Game");
              dprint( 89, 124, C_BLACK, "No - Get Me Back To Game");

              dfont( &font_fantasy );
              dprint( 90, 140, C_RGB(150,150,150), "Yes - For Now, But I'll Come Back !");
              dprint( 89, 139, C_BLACK, "Yes - For Now, But I'll Come Back !");

              dfont( &font_tiny );

              if (selectOptionPause == 0)
              {
                     lX = 70;
                     lY = 119;
              }
              else if (selectOptionPause == 1)
              {
                     lX = 70;
                     lY = 134;
              }

              frame_light%=6;
              dsubimage( lX, lY, &light, 16*frame_light,0,16,16,  DIMAGE_NONE);
              frame_light++;

              dfont( &font_tiny );
              dprint( 55, 170, C_RGB(150,150,150), "[UP] [DOWN] : select / [EXE] : validate / [EXIT] : escape");
              dprint( 54, 169, C_BLACK,"[UP] [DOWN] : select / [EXE] : validate / [EXIT] : escape");

              dupdate();

              get_inputs_quit();

       }
}

static int get_inputs_title(void)
{
       int opt = GETKEY_DEFAULT & ~GETKEY_REP_ARROWS;
       int timeout = 1;

       while(1)
       {
              key_event_t ev = getkey_opt(opt, &timeout);
              if(ev.type == KEYEV_NONE) return -1;

              int key = ev.key;

              if (key==KEY_EXE)
                     doneTitle = true;
       }
}

void drawTitle( void )
{
       doneTitle = false;

       unsigned int positionGuy = 75;
       int dir = 1;

       frame = 0;
       unsigned char flickers = 0;

       while (!doneTitle)
       {
              dclear(C_RGB(0,0,0));

              dimage( 0, 10, &maintitle);

              if (dir==1) // the guy is going to the right
              {
                     if (positionGuy>=285) dir=-1;

                     if (frame==0) dsubimage( positionGuy, 100, &player, 0,16,16,16,  DIMAGE_NONE);
                     else if (frame==1) dsubimage( positionGuy, 100, &player, 16,16,16,16,  DIMAGE_NONE);
                     else if (frame==2) dsubimage( positionGuy, 100, &player, 32,16,16,16,  DIMAGE_NONE);

                     frame++;
                     positionGuy+=1;
                     if  (frame==3) frame=0;
              }
              else if (dir==-1) // the guy is going to the left
              {
                     if (positionGuy<=75) dir=1;

                     if (frame==0) dsubimage( positionGuy, 100, &player, 0,48,16,16,  DIMAGE_NONE);
                     else if (frame==1) dsubimage( positionGuy, 100, &player, 16,48,16,16,  DIMAGE_NONE);
                     else if (frame==2) dsubimage( positionGuy, 100, &player, 32,48,16,16,  DIMAGE_NONE);

                     frame++;
                     positionGuy-=1;
                     if  (frame==3) frame=0;
              }

              if (flickers<20)
              {
                     dfont( &font_fantasy );
                     dprint( 120, 130, C_RGB(150,150,150), "[ Press EXE Key To Start ]");
                     dprint( 119, 129, C_BLACK, "[ Press EXE Key To Start ]");
              }

              dfont( &font_tiny );
              dprint( 110, 156, C_RGB(150,150,150), "Welcome to Magic Light Revision 0.3B");
              dprint( 109, 155, C_BLACK, "Welcome to Magic Light Revision 0.3B");
              dprint( 110, 166, C_RGB(150,150,150), "                    12/2021 - by SlyVTT                     ");
              dprint( 109, 165, C_BLACK, "                    12/2021 - by SlyVTT                     ");


              flickers++;
              if (flickers==40) flickers=0;

              dupdate();

              get_inputs_title();
       }
}

static int get_inputs_startmenu(void)
{
       int opt = GETKEY_DEFAULT & ~GETKEY_REP_ARROWS;
       int timeout = 1;

       while(1)
       {
              key_event_t ev = getkey_opt(opt, &timeout);
              if(ev.type == KEYEV_NONE) return -1;

              int key = ev.key;

              if (key==KEY_EXE)
                     doneStart = true;

              if (key==KEY_UP && selectStartMenu>0)
                     selectStartMenu--;

              if (key==KEY_DOWN && selectStartMenu<2)
                     selectStartMenu++;
       }
}


unsigned char drawStartMenu( void )
{
       doneStart = false;

       unsigned int lX = 0;
       unsigned int lY = 0;

       frame_light = 0;

       while (!doneStart)
       {
              dclear(C_RGB(0,0,0));

              dimage( 0, 10, &maintitle);

              dfont( &font_fantasy );
              dprint( 120, 100, C_RGB(150,150,150), "Continue Previous Game");
              dprint( 119, 99, C_RGB(200,200,200), "Continue Previous Game");

              dfont( &font_fantasy );
              dprint( 120, 120, C_RGB(150,150,150), "Start New Game");
              dprint( 119, 119, C_BLACK, "Start New Game");

              dfont( &font_fantasy );
              dprint( 120, 140, C_RGB(150,150,150), "Quit And Back To OS");
              dprint( 119, 139, C_BLACK, "Quit And Back To OS");

              if (selectStartMenu == 0)
              {
                     lX = 100;
                     lY = 94;
              }
              else if (selectStartMenu == 1)
              {
                     lX = 100;
                     lY = 114;
              }
              else if (selectStartMenu == 2)
              {
                     lX = 100;
                     lY = 134;
              }

              frame_light%=6;
              dsubimage( lX, lY, &light, 16*frame_light,0,16,16,  DIMAGE_NONE);
              frame_light++;


              dfont( &font_tiny );
              dprint( 90, 166, C_RGB(150,150,150), "[UP] [DOWN] : select / [EXE] : validate");
              dprint( 89, 165, C_BLACK,"[UP] [DOWN] : select / [EXE] : validate");

              dupdate();

              get_inputs_startmenu();
       }

       return selectStartMenu;
}


static int get_inputs(void)
{
       int opt = GETKEY_DEFAULT & ~GETKEY_REP_ARROWS;
       int timeout = 1;
       unsigned char choice_exit=0;

       while(1)
       {
              key_event_t ev = getkey_opt(opt, &timeout);
              if(ev.type == KEYEV_NONE) return -1;

              int key = ev.key;

              if (key==KEY_EXIT)
              {
                     drawQuit();
              }

              if (key==KEY_OPTN)
              {
                     drawOption();
              }

              if (key==KEY_F1)
              {
                     selected = JOUEUR;
              }

              if (key==KEY_F2)
              {
                     selected = LUMIERE;
              }

              if (key==KEY_F3)
              {
                     selected = ROUGE;
              }

              if (key==KEY_F4)
              {
                     selected = VERT;
              }

              if (key==KEY_F5)
              {
                     selected = BLEU;
              }

              if (key==KEY_F6)
              {
                     selected = NOIR;
              }

              if (key==KEY_LEFT)
              {
                     gameMechanics( selected, GAUCHE);
              }

              if (key==KEY_RIGHT)
              {
                     gameMechanics( selected, DROITE);
              }

              if (key==KEY_UP)
              {
                     gameMechanics( selected, HAUT);
              }

              if (key==KEY_DOWN)
              {
                     gameMechanics( selected, BAS);
              }

#if DEBUG==1
              if (key==KEY_1)
              {
                     if (currentLevel>0)
                     {
                            currentLevel-- ;
                            loadLevel( currentLevel );
                     }

              }

              if (key==KEY_7)
              {
                     if (currentLevel<NB_LEVELS -1)
                     {
                            currentLevel++ ;
                            loadLevel( currentLevel );
                     }
              }
#endif

       }
}




int main ( int argc, char** argv )
{
       doneGame = false;
       doneMain = false;
       doneOption = false;
       donePause = false;
       doneStart = false;
       doneTitle = false;


       drawTitle();

       unsigned char choice = drawStartMenu();

       if (choice==0)
       {
              // TODO : Load previous status
              loadLevel( 0 );
              initMap();
       }
       else if (choice==1)
       {
              // We start a new game, so we load Level #000
              loadLevel( 0 );
              initMap();
       }
       else if (choice==2)
       {
              exitAndFree();
              return 0;
       }


       initWalls();


       while (!doneGame)
       {
              dclear(C_RGB(0,0,0));

              frame_cursor++;
              frame_cursor = frame_cursor % 4;

              drawInterface(  );
              drawMinimap( );
              renderMap(  );

              checkDoors( );
              renderDoors( );

              renderObstacles( LAYER_BELOW_RAYS );
              renderObstacles( LAYER_ABOVE_RAYS );

              renderWalls(  );

              checkReceptors(  );
              renderReceptors(  );

              renderMovable(  );

              renderLight(  );
              renderTreasures( );
              renderPlayer(  );

              dupdate();

              get_inputs();

              if (selectOptionPause==1) doneGame=true;
       }

       exitAndFree( );

       return 0;
}
