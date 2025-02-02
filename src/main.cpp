#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include <gint/rtc.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/gint.h>
#include <gint/bfile.h>
#include <gint/std/stdlib.h>

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

const char* Revision = "Revision 1.0A";
const char* RevShort = "v1.0A";


static const uint16_t *filepath = u"\\\\fls0\\MgcLght.sav";
bool saveexist = false;
unsigned int sizeoffile;
GameSaveStatus sauvegarde;
LevelSaveStatus partielevel[33];


#define NB_LEVELS 32


unsigned char NbReceptorsLevel;
unsigned char NbDoorsLevel;
unsigned char NbBlocksLevel;
unsigned char NbTreasureLevel;
unsigned char NbObstacleLevel;
unsigned char NbMonsterLevel;
unsigned char NbBossLevel;
unsigned char NbBulletLevel;

selection selected = JOUEUR;

unsigned int score;
unsigned char life = 3;
unsigned char lifeMax = 5;

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
Monster* MonstersCollection=NULL;
Boss* BossCollection=NULL;
Bullet* BulletCollection=NULL;


Minimap Map[ 33 ];


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
extern bopti_image_t monstres;
extern bopti_image_t hearts;
extern bopti_image_t bossmages;
extern bopti_image_t bigboss;
extern bopti_image_t bigparch;
extern bopti_image_t bulletsbicolor;
extern bopti_image_t fragments;
extern bopti_image_t village;
extern bopti_image_t castletitle;
extern bopti_image_t theend;

uint16_t palette[256];

bool BlackFrag=false;
bool WhiteFrag=false;
bool RedFrag=false;
bool GreenFrag=false;
bool BlueFrag=false;


extern font_t font_fantasy;
extern font_t font_tiny;

bool GotoMainMenu=false;
bool doneMain = false;
bool donePause = false;
bool doneOption = false;
bool doneGame = false;
bool doneStart = false;
bool doneTitle = false;
bool doneLoose = false;
bool doneDifficulty = false;
bool doneStory = false;
bool doneEnd = false;
bool attackboss = false;

unsigned int compteur_mouvement = 0;
unsigned char frame_cursor = 0;
unsigned char frame_light = 0;
unsigned char frame_monster = 0;
unsigned char frame_boss = 0;
unsigned char frame=0;

unsigned char frame_boss_attack=0;
unsigned int xbossattack;
unsigned int ybossattack;
unsigned char frame_bullet=0;
unsigned char attacktype;
unsigned char attackpattern;

bool mouvement=false;
orientations direction=HAUT;

unsigned char nextLevel;
unsigned char currentLevel;
unsigned char selectStartMenu = 0;
unsigned char selectDifficultyMenu = 0;
unsigned char selectOptionMenu = 0;
unsigned char selectOptionPause = 0;
unsigned char selectOptionLoose = 0;
unsigned char selectStoryMenu = 0;
unsigned char selectEndMenu = 0;
unsigned char difficulty = 0;


void is_save_existing( void )
{
    int file = BFile_Open( filepath, BFile_ReadOnly );
    if (file>=0)
    {
        saveexist=true;
        BFile_Close( file );
    }
    else
    {
        saveexist=false;
        BFile_Close( file );
    }
}

bool is_save_existing_boolean( void )
{
    int file = BFile_Open( filepath, BFile_ReadOnly );
    if (file>=0)
    {
        BFile_Close( file );
        return true;
    }
    else
    {
        BFile_Close( file );
        return false;
    }
}

void delete_save( void )
{
    BFile_Remove( filepath );
}

void saveprogress( void )
{
    sizeoffile = sizeof( GameSaveStatus ) + 33*sizeof( LevelSaveStatus );
    unsigned int sizecopy = sizeoffile;

    unsigned char* buffer;
    buffer = (unsigned char*) malloc( sizeoffile );

    buffer[0] = sauvegarde.currentScore[0];
    buffer[1] = sauvegarde.currentScore[1];

    buffer[2] = sauvegarde.currentLevel;

    for (unsigned char k = 0; k<=32; k++ )
        buffer[3+k] = sauvegarde.visitedLevels[k];

    buffer[36] = sauvegarde.currentDifficulty;
    buffer[37] = sauvegarde.currentLife;
    buffer[38] = sauvegarde.redFrag;
    buffer[39] = sauvegarde.greenFrag;
    buffer[40] = sauvegarde.blueFrag;
    buffer[41] = sauvegarde.blackFrag;
    buffer[42] = sauvegarde.whiteFrag;
    buffer[43] = sauvegarde.terminator;

    ///TODO implement Room Saves

    int index;

    for (unsigned char k = 0; k<=32; k++ )
    {
        index = sizeof( GameSaveStatus )+k*sizeof( LevelSaveStatus );

        //for( int i=0; i<8; i++ )
        buffer[index+0] =  partielevel[k].chestStatusSave[ 0 ];
        buffer[index+1] =  partielevel[k].chestStatusSave[ 1 ];
        buffer[index+2] =  partielevel[k].chestStatusSave[ 2 ];
        buffer[index+3] =  partielevel[k].chestStatusSave[ 3 ];
        buffer[index+4] =  partielevel[k].chestStatusSave[ 4 ];
        buffer[index+5] =  partielevel[k].chestStatusSave[ 5 ];
        buffer[index+6] =  partielevel[k].chestStatusSave[ 6 ];
        buffer[index+7] =  partielevel[k].chestStatusSave[ 7 ];

        //for( unsigned char i=8; i<12; i++ )
        buffer[index+8] =  partielevel[k].doorStatusSave[ 0 ];
        buffer[index+9] =  partielevel[k].doorStatusSave[ 1 ];
        buffer[index+10] =  partielevel[k].doorStatusSave[ 2 ];
        buffer[index+11] =  partielevel[k].doorStatusSave[ 3 ];

        //for( unsigned char i=12; i<20; i++ )
        buffer[index+12] =  partielevel[k].bossStatusSave[ 0 ];
        buffer[index+13] =  partielevel[k].bossStatusSave[ 1 ];
        buffer[index+14] =  partielevel[k].bossStatusSave[ 2 ];
        buffer[index+15] =  partielevel[k].bossStatusSave[ 3 ];
        buffer[index+16] =  partielevel[k].bossStatusSave[ 4 ];
        buffer[index+17] =  partielevel[k].bossStatusSave[ 5 ];
        buffer[index+18] =  partielevel[k].bossStatusSave[ 6 ];
        buffer[index+19] =  partielevel[k].bossStatusSave[ 7 ];

        //for( unsigned char i=20; i<28; i++ )
        buffer[index+20] =  partielevel[k].monsterStatusSave[ 0 ];
        buffer[index+21] =  partielevel[k].monsterStatusSave[ 1 ];
        buffer[index+22] =  partielevel[k].monsterStatusSave[ 2 ];
        buffer[index+23] =  partielevel[k].monsterStatusSave[ 3 ];
        buffer[index+24] =  partielevel[k].monsterStatusSave[ 4 ];
        buffer[index+25] =  partielevel[k].monsterStatusSave[ 5 ];
        buffer[index+26] =  partielevel[k].monsterStatusSave[ 6 ];
        buffer[index+27] =  partielevel[k].monsterStatusSave[ 7 ];

        //for( unsigned char i=28; i<30; i++ )
        buffer[index+28] =  partielevel[k].redBlockSave[ 0 ];
        buffer[index+29] =  partielevel[k].redBlockSave[ 1 ];

        //for( unsigned char i=30; i<32; i++ )
        buffer[index+30] =  partielevel[k].greenBlockSave[ 0 ];
        buffer[index+31] =  partielevel[k].greenBlockSave[ 1 ];

        //for( unsigned char i=32; i<34; i++ )
        buffer[index+32] =  partielevel[k].blueBlockSave[ 0 ];
        buffer[index+33] =  partielevel[k].blueBlockSave[ 1 ];

        //for( unsigned char i=34; i<36; i++ )
        buffer[index+34] =  partielevel[k].blackBlockSave[ 0 ];
        buffer[index+35] =  partielevel[k].blackBlockSave[ 1 ];

        //for( unsigned char i=36; i<38; i++ )
        buffer[index+36] =  partielevel[k].lightSave[ 0 ];
        buffer[index+37] =  partielevel[k].lightSave[ 1 ];
    }

    /*
    int file = BFile_Open( filepath, BFile_ReadOnly );

    if ( file >= 0 )
    {
           BFile_Close( file );
           file = BFile_Remove( filepath );
    }


    */

    int file;

    BFile_Remove( filepath );
    BFile_Create( filepath, BFile_File, &sizecopy );

    file = BFile_Open( filepath, BFile_WriteOnly );
    BFile_Write( file, buffer, sizeoffile );

    BFile_Close( file );

    free( buffer );
}

void loadprogress( void )
{
    sizeoffile = sizeof( GameSaveStatus ) + 33*sizeof( LevelSaveStatus );
    unsigned int sizecopy = sizeoffile;

    unsigned char* buffer;
    buffer = (unsigned char*) malloc( sizeoffile );


    int file;
    file = BFile_Open( filepath, BFile_ReadOnly );
    BFile_Read( file, buffer, sizeoffile, -1 );
    BFile_Close( file );


    sauvegarde.currentScore[0] = buffer[0];
    sauvegarde.currentScore[1] = buffer[1];

    sauvegarde.currentLevel = buffer[2];

    for (unsigned char k = 0; k<=32; k++ )
        sauvegarde.visitedLevels[k] = buffer[3+k];

    sauvegarde.currentDifficulty = buffer[36];
    sauvegarde.currentLife = buffer[37];
    sauvegarde.redFrag = buffer[38];
    sauvegarde.greenFrag = buffer[39];
    sauvegarde.blueFrag = buffer[40];
    sauvegarde.blackFrag = buffer[41];
    sauvegarde.whiteFrag = buffer[42];
    sauvegarde.terminator = buffer[43];

    int index;

    for (int k = 0; k<=32; k++ )
    {
        index = sizeof(GameSaveStatus)+k*sizeof( LevelSaveStatus );

        //for( unsigned char i=0; i<8; i++ )
        partielevel[k].chestStatusSave[ 0 ] = buffer[index+0];
        partielevel[k].chestStatusSave[ 1 ] = buffer[index+1];
        partielevel[k].chestStatusSave[ 2 ] = buffer[index+2];
        partielevel[k].chestStatusSave[ 3 ] = buffer[index+3];
        partielevel[k].chestStatusSave[ 4 ] = buffer[index+4];
        partielevel[k].chestStatusSave[ 5 ] = buffer[index+5];
        partielevel[k].chestStatusSave[ 6 ] = buffer[index+6];
        partielevel[k].chestStatusSave[ 7 ] = buffer[index+7];

        //for( unsigned char i=8; i<12; i++ )
        partielevel[k].doorStatusSave[ 0 ] = buffer[index+8];
        partielevel[k].doorStatusSave[ 1 ] = buffer[index+9];
        partielevel[k].doorStatusSave[ 2 ] = buffer[index+10];
        partielevel[k].doorStatusSave[ 3 ] = buffer[index+11];

        //for( unsigned char i=12; i<20; i++ )
        partielevel[k].bossStatusSave[ 0 ] = buffer[index+12];
        partielevel[k].bossStatusSave[ 1 ] = buffer[index+13];
        partielevel[k].bossStatusSave[ 2 ] = buffer[index+14];
        partielevel[k].bossStatusSave[ 3 ] = buffer[index+15];
        partielevel[k].bossStatusSave[ 4 ] = buffer[index+16];
        partielevel[k].bossStatusSave[ 5 ] = buffer[index+17];
        partielevel[k].bossStatusSave[ 6 ] = buffer[index+18];
        partielevel[k].bossStatusSave[ 7 ] = buffer[index+19];

        //for( unsigned char i=20; i<28; i++ )
        partielevel[k].monsterStatusSave[ 0 ] = buffer[index+20];
        partielevel[k].monsterStatusSave[ 1 ] = buffer[index+21];
        partielevel[k].monsterStatusSave[ 2 ] = buffer[index+22];
        partielevel[k].monsterStatusSave[ 3 ] = buffer[index+23];
        partielevel[k].monsterStatusSave[ 4 ] = buffer[index+24];
        partielevel[k].monsterStatusSave[ 5 ] = buffer[index+25];
        partielevel[k].monsterStatusSave[ 6 ] = buffer[index+26];
        partielevel[k].monsterStatusSave[ 7 ] = buffer[index+27];

        //for( unsigned char i=28; i<30; i++ )
        partielevel[k].redBlockSave[ 0 ] = buffer[index+28];
        partielevel[k].redBlockSave[ 1 ] = buffer[index+29];

        //for( unsigned char i=30; i<32; i++ )
        partielevel[k].greenBlockSave[ 0 ] = buffer[index+30];
        partielevel[k].greenBlockSave[ 1 ] = buffer[index+31];

        //for( unsigned char i=32; i<34; i++ )
        partielevel[k].blueBlockSave[ 0 ] = buffer[index+32];
        partielevel[k].blueBlockSave[ 1 ] = buffer[index+33];

        //for( unsigned char i=34; i<36; i++ )
        partielevel[k].blackBlockSave[ 0 ] = buffer[index+34];
        partielevel[k].blackBlockSave[ 1 ] = buffer[index+35];

        //for( unsigned char i=36; i<38; i++ )
        partielevel[k].lightSave[ 0 ] = buffer[index+36];
        partielevel[k].lightSave[ 1 ] = buffer[index+37];
    }

    free( buffer );

}

void updateCurrentState( void  )
{
    sauvegarde.currentScore[0] = score/256;
    sauvegarde.currentScore[1] = score%256;
    sauvegarde.currentLevel = currentLevel;

    for( unsigned char k=0; k<=32; k++ )
        if (Map[k].visited==true) sauvegarde.visitedLevels[k] = 1;
        else sauvegarde.visitedLevels[k] = 0;

    sauvegarde.currentDifficulty = difficulty;
    sauvegarde.currentLife = life;

    if (RedFrag==true) sauvegarde.redFrag=true;
    else sauvegarde.redFrag=false;

    if (GreenFrag==true) sauvegarde.greenFrag=true;
    else sauvegarde.greenFrag=false;

    if (BlueFrag==true) sauvegarde.blueFrag=true;
    else sauvegarde.blueFrag=false;

    if (BlackFrag==true) sauvegarde.blackFrag=true;
    else sauvegarde.blackFrag=false;

    if (WhiteFrag==true) sauvegarde.whiteFrag=true;
    else sauvegarde.whiteFrag=false;


    for( unsigned char i=0; i<NbTreasureLevel; i++ )
        if (TreasureCollection[i].isopen==false && TreasureCollection[i].isvisible==true)
            partielevel[currentLevel].chestStatusSave[i]=0x00;
        else if (TreasureCollection[i].isopen==true && TreasureCollection[i].isvisible==true)
            partielevel[currentLevel].chestStatusSave[i]=0x01;
        else if (TreasureCollection[i].isvisible==false)
            partielevel[currentLevel].chestStatusSave[i]=0x02;


    for( unsigned char i=0; i<NbDoorsLevel; i++ )
        if (DoorCollection[i].isopen==true)
            partielevel[currentLevel].doorStatusSave[i]=0x01;
        else
            partielevel[currentLevel].doorStatusSave[i]=0x00;


    //TODO : save boss status
    //for( int i=0; i<NbBossLevel; i++ )

    //TODO : save monster status
    //for( int i=0; i<NbMonsterLevel; i++ )

    partielevel[currentLevel].redBlockSave[0]=BlocRouge->x;
    partielevel[currentLevel].redBlockSave[1]=BlocRouge->y;

    partielevel[currentLevel].greenBlockSave[0]=BlocVert->x;
    partielevel[currentLevel].greenBlockSave[1]=BlocVert->y;

    partielevel[currentLevel].blueBlockSave[0]=BlocBleu->x;
    partielevel[currentLevel].blueBlockSave[1]=BlocBleu->y;

    partielevel[currentLevel].blackBlockSave[0]=BlocNoir->x;
    partielevel[currentLevel].blackBlockSave[1]=BlocNoir->y;

    partielevel[currentLevel].lightSave[0]=lumiere.x;
    partielevel[currentLevel].lightSave[1]=lumiere.y;
}

void initMap( void )
{
    for( unsigned char k=0; k < 33; k++) Map[k].visited = false;
    currentLevel = 0;
}

void adjustLevel( int lev )
{

    for( int i=0; i<NbTreasureLevel; i++ )
        if (partielevel[lev].chestStatusSave[i]==0)
        {
            TreasureCollection[i].isopen = false;
            TreasureCollection[i].isvisible = true;
        }
        else if (partielevel[lev].chestStatusSave[i]==1)
        {
            TreasureCollection[i].isopen = true;
            TreasureCollection[i].isvisible = true;
        }
        else if (partielevel[lev].chestStatusSave[i]==2)
        {
            TreasureCollection[i].isopen = true;
            TreasureCollection[i].isvisible = false;
        }

    for( int i=0; i<NbDoorsLevel; i++ )
        if (partielevel[lev].doorStatusSave[i]==1) DoorCollection[i].isopen=true;
        else DoorCollection[i].isopen = false;

    for( int i=0; i<NbBossLevel; i++ )
    {
        //TODO : make appropriate code
    }

    for( int i=0; i<NbMonsterLevel; i++ )
    {
        //TODO : make appropriate code
    }

    BlocRouge->x = partielevel[lev].redBlockSave[0];
    BlocRouge->y = partielevel[lev].redBlockSave[1];

    BlocVert->x = partielevel[lev].greenBlockSave[0];
    BlocVert->y = partielevel[lev].greenBlockSave[1];

    BlocBleu->x = partielevel[lev].blueBlockSave[0];
    BlocBleu->y = partielevel[lev].blueBlockSave[1];

    BlocNoir->x = partielevel[lev].blackBlockSave[0];
    BlocNoir->y = partielevel[lev].blackBlockSave[1];

    lumiere.x = partielevel[lev].lightSave[0];
    lumiere.y = partielevel[lev].lightSave[1];

}


void loadLevel( unsigned char numLevel )
{
    free(BlocksCollection);
    free(ReceptorCollection);
    free(DoorCollection);
    free(TreasureCollection);
    free(ObstaclesCollection);
    free(MonstersCollection);

    BlocksCollection=NULL;
    ReceptorCollection=NULL;
    DoorCollection=NULL;
    TreasureCollection=NULL;
    ObstaclesCollection=NULL;
    MonstersCollection=NULL;

    attackboss=false;
    bool needcorrection=false;



    // if (sauvegarde.visitedLevels[numLevel]==1)   // The level to be loaded has already been played so we will have to make corrections as per save state
    if (Map[numLevel].visited==true)       needcorrection = true;
    else needcorrection = false;

    if (numLevel==0) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 4;
        NbDoorsLevel = 1;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 16;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=0;
        Map[numLevel].y=0;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=false;

        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        ObstaclesCollection[0] = {4,1,BLOCK_STONE};
        ObstaclesCollection[1] = {8,1,BLOCK_STONE};
        ObstaclesCollection[2] = {1,3,BLOCK_STONE};
        ObstaclesCollection[3] = {11,3,BLOCK_STONE};
        ObstaclesCollection[4] = {1,7,BLOCK_STONE};
        ObstaclesCollection[5] = {11,7,BLOCK_STONE};
        ObstaclesCollection[6] = {4,9,BLOCK_STONE};
        ObstaclesCollection[7] = {8,9,BLOCK_STONE};

        ObstaclesCollection[8] = {5,4,BLOCK_WATER};
        ObstaclesCollection[9] = {6,4,BLOCK_WATER};
        ObstaclesCollection[10] = {7,4,BLOCK_WATER};
        ObstaclesCollection[11] = {5,5,BLOCK_WATER};
        ObstaclesCollection[12] = {7,5,BLOCK_WATER};
        ObstaclesCollection[13] = {5,6,BLOCK_WATER};
        ObstaclesCollection[14] = {6,6,BLOCK_WATER};
        ObstaclesCollection[15] = {7,6,BLOCK_WATER};


        BlocksCollection[0] = { 2, 4, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 10, 4, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 10, 6, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 2, 6, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        ReceptorCollection[0] = { 2, 0, BAS, 255, 0, 0, false };  // Receptor RED
        ReceptorCollection[1] = { 10, 0, BAS, 0, 255, 0, false }; // Receptor GREEN
        ReceptorCollection[2] = { 10, 10, HAUT, 0, 0, 255, false }; // Receptor BLUE
        ReceptorCollection[3] = { 2, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK
        // ReceptorCollection[4] = { 8, 0, BAS, 255, 0, 255, false }; // Receptor PINK
        // ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
        // ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
        // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

        DoorCollection[0] = { 6,0, HAUT, false, true, 1, { R_ROUGE, -1, -1 }, 2}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 1, { R_BLEU, -1, -1 }, 0}; // Door WEST
        //DoorCollection[2] = { 12, 5, DROITE, false, true, 1, { R_VERT, -1, -1 }, 0}; // Door EAST
        //DoorCollection[3] = { 6, 10, BAS, false, false, 0, { -1 -1, -1 }, -1}; // Door SOUTH

        //TreasureCollection[0] = {1,1, T_RED, false, WHITE_FRAGMENT, 100, true };
        //TreasureCollection[1] = {2,1, T_YELLOW, false, BLACK_FRAGMENT, 100, true };
        //TreasureCollection[2] = {3,1, T_GREEN, false, RED_FRAGMENT, 100, true };
        //TreasureCollection[3] = {4,1, T_BLUE, false, GREEN_FRAGMENT, 100, true };
        //TreasureCollection[4] = {5,1, T_BLUE, false, BLUE_FRAGMENT, 100, true };

        //MonstersCollection[0] = { 6, 4, 2, 10, HORIZONTAL, GAUCHE, BLOB };
        //MonstersCollection[1] = { 7, 5, 2, 8, VERTICAL, HAUT, SKELETON };
    }
    else if (numLevel==1) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 1;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 4;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 16;
        NbMonsterLevel = 4;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-1;
        Map[numLevel].y=-1;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=false;

        joueur = { 6, 9, HAUT };
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

        DoorCollection[0] = { 6,0, HAUT, false, true, 2, { R_ROUGE,R_BLEU, -1 }, 4}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 1, { R_BLEU, -1, -1 }, 0}; // Door WEST
        DoorCollection[1] = { 12, 5, DROITE, false, true, 2, { R_VERT, R_NOIR, -1 }, 2}; // Door EAST
        //DoorCollection[3] = { 6, 10, BAS, false, false, 0, { -1 -1, -1 }, -1}; // Door SOUTH

        TreasureCollection[0] = {4,4, T_RED, false, PIERRE_BLANCHE, 100,  true };
        TreasureCollection[1] = {8,4, T_YELLOW, false, PIERRE_BLANCHE, 100,  true };
        TreasureCollection[2] = {4,6, T_GREEN, false, PIERRE_BLANCHE, 100,  true };
        TreasureCollection[3] = {8,6, T_BLUE, false, PIERRE_BLANCHE, 100,  true };

        MonstersCollection[0] = { 6, 3, 1, 11, HORIZONTAL, DROITE, SPIDER };
        MonstersCollection[1] = { 6, 7, 1, 11, HORIZONTAL, GAUCHE, SPIDER };
        MonstersCollection[2] = { 3, 5, 1, 9, VERTICAL, HAUT, SPIDER };
        MonstersCollection[3] = { 9, 5, 1, 9, VERTICAL, BAS, SPIDER };
        //MonstersCollection[1] = { 1, 5, 1, 9, VERTICAL, BAS, SPIDER };
        //MonstersCollection[2] = { 10, 5, 1, 9, VERTICAL, HAUT, GHOST };

    }
    else if (numLevel==2) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 2;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 4;
        NbDoorsLevel = 3;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 24;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=0;
        Map[numLevel].y=-1;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=true;

        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        ObstaclesCollection[0] = {1,4,BLOCK_STONE};
        ObstaclesCollection[1] = {2,4,BLOCK_STONE};
        ObstaclesCollection[2] = {1,6,BLOCK_STONE};
        ObstaclesCollection[3] = {2,6,BLOCK_STONE};
        ObstaclesCollection[4] = {10,4,BLOCK_STONE};
        ObstaclesCollection[5] = {11,4,BLOCK_STONE};
        ObstaclesCollection[6] = {10,6,BLOCK_STONE};
        ObstaclesCollection[7] = {11,6,BLOCK_STONE};
        ObstaclesCollection[8] = {5,8,BLOCK_STONE};
        ObstaclesCollection[9] = {5,9,BLOCK_STONE};
        ObstaclesCollection[10] = {7,8,BLOCK_STONE};
        ObstaclesCollection[11] = {7,9,BLOCK_STONE};
        ObstaclesCollection[12] = {5,2,BLOCK_WATER};
        ObstaclesCollection[13] = {6,2,BLOCK_WATER};
        ObstaclesCollection[14] = {7,2,BLOCK_WATER};
        ObstaclesCollection[15] = {5,3,BLOCK_WATER};
        ObstaclesCollection[16] = {7,3,BLOCK_WATER};
        ObstaclesCollection[17] = {5,4,BLOCK_WATER};
        ObstaclesCollection[18] = {7,4,BLOCK_WATER};
        ObstaclesCollection[19] = {5,5,BLOCK_WATER};
        ObstaclesCollection[20] = {7,5,BLOCK_WATER};
        ObstaclesCollection[21] = {5,6,BLOCK_WATER};
        ObstaclesCollection[22] = {6,6,BLOCK_WATER};
        ObstaclesCollection[23] = {7,6,BLOCK_WATER};

        BlocksCollection[0] = { 2, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 3, 1, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 10, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 9, 1, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        ReceptorCollection[0] = { 0, 3, DROITE, 255, 0, 0, false };  // Receptor RED
        ReceptorCollection[1] = { 2, 0, BAS, 0, 255, 0, false }; // Receptor GREEN
        ReceptorCollection[2] = { 12, 3, GAUCHE, 0, 0, 255, false }; // Receptor BLUE
        ReceptorCollection[3] = { 10, 0, BAS, 0, 0, 0, false }; // Receptor BLACK
        //ReceptorCollection[4] = { 8, 0, BAS, 255, 0, 255, false }; // Receptor PINK
        //ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
        //ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
        //ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

        //DoorCollection[0] = { 6,0, HAUT, false, true, 1, { R_ROUGE,R_BLEU, R_VERT }, 1}; // Door NORTH
        DoorCollection[0] = { 0, 5, GAUCHE, false, true, 2, { R_ROUGE, R_VERT, -1 }, 1}; // Door WEST
        DoorCollection[1] = { 12, 5, DROITE, false, true, 2, { R_BLEU, R_NOIR, -1 }, 3}; // Door EAST
        DoorCollection[2] = { 6, 10, BAS, false, true, 2, { R_ROUGE, R_BLEU, -1 }, 0}; // Door SOUTH

        TreasureCollection[0] = {1,1, T_RED, false, PIERRE_BLANCHE, 100,  true };
        TreasureCollection[1] = {11,1, T_YELLOW, false, PIERRE_BLANCHE, 100,  true };
        TreasureCollection[2] = {1,9, T_GREEN, false, PIERRE_BLANCHE, 100,  true };
        TreasureCollection[3] = {11,9, T_BLUE, false, PIERRE_BLANCHE, 100,  true };

    }
    else if (numLevel==3) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 1;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 4;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 8;
        NbMonsterLevel = 4;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=1;
        Map[numLevel].y=-1;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=false;

        joueur = { 6, 9, HAUT };
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
        ReceptorCollection[3] = { 12, 7, GAUCHE, 0, 0, 0, false }; // Receptor BLACK
        //ReceptorCollection[4] = { 8, 0, BAS, 255, 0, 255, false }; // Receptor PINK
        //ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
        //ReceptorCollection[3] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
        //ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE


        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { R_ROUGE,R_BLEU, R_VERT }, 5}; // Door NORTH
        DoorCollection[1] = { 0, 5, GAUCHE, false, true, 1, { R_BLEU, 3, -1 }, 2}; // Door WEST
        //DoorCollection[2] = { 12, 5, DROITE, false, true, 1, { R_VERT, -1, -1 }, 0}; // Door EAST
        //DoorCollection[3] = { 6, 10, BAS, false, false, 0, { -1 -1, -1 }, -1}; // Door SOUTH

        TreasureCollection[0] = {4,4, T_RED, false, PIERRE_BLANCHE, 100,  true };
        TreasureCollection[1] = {8,4, T_YELLOW, false, PIERRE_BLANCHE, 100,  true };
        TreasureCollection[2] = {4,6, T_GREEN, false, PIERRE_BLANCHE, 100,  true };
        TreasureCollection[3] = {8,6, T_BLUE, false, PIERRE_BLANCHE, 100,  true };

        MonstersCollection[0] = { 6, 2, 1, 11, HORIZONTAL, DROITE, SKELETON };
        MonstersCollection[1] = { 6, 8, 1, 11, HORIZONTAL, GAUCHE, SKELETON };
        MonstersCollection[2] = { 2, 5, 1, 9, VERTICAL, HAUT, SKELETON };
        MonstersCollection[3] = { 10, 5, 1, 9, VERTICAL, BAS, SKELETON };


    }
    else if (numLevel==4) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 1;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 5;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 4;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-1;
        Map[numLevel].y=-2;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;

        joueur = { 6, 9, HAUT };
        lumiere = { 6, 6, 1,1,1,0, false };

        //ObstaclesCollection[0] = {2,2,BLOCK_SUN};
        //ObstaclesCollection[1] = {10,2,BLOCK_SUN};
        //ObstaclesCollection[2] = {2,8,BLOCK_SUN};
        //ObstaclesCollection[3] = {10,8,BLOCK_SUN};
        ObstaclesCollection[0] = {5,6,BLOCK_WATER};
        ObstaclesCollection[1] = {7,6,BLOCK_WATER};
        ObstaclesCollection[2] = {6,5,BLOCK_WATER};
        ObstaclesCollection[3] = {6,7,BLOCK_WATER};

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
        ReceptorCollection[3] = { 12, 8, GAUCHE, 0, 0, 0, false }; // Receptor BLACK
        ReceptorCollection[4] = { 12, 3, GAUCHE, 255, 255, 0, false }; // Receptor PINK
        // ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
        // ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
        // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

        DoorCollection[0] = { 6,0, HAUT, false, true, 2, { 2,4, -1 }, 7}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 1, { R_BLEU, -1, -1 }, 0}; // Door WEST
        //DoorCollection[2] = { 12, 5, DROITE, false, true, 1, { R_VERT, -1, -1 }, 0}; // Door EAST
        DoorCollection[1] = { 6, 10, BAS, false, true, 1, { R_NOIR, -1, -1 }, 1}; // Door SOUTH

        TreasureCollection[0] = {1,1, T_RED, false, PIERRE_BLANCHE, 100, true };
        TreasureCollection[1] = {11,1, T_YELLOW, false, PIERRE_BLANCHE, 100, true };
        TreasureCollection[2] = {11,9, T_GREEN, false, PIERRE_BLANCHE, 100, true };
        TreasureCollection[3] = {1,9, T_BLUE, false, PIERRE_BLANCHE, 100, true };

        //BossCollection[0] = { 6, 2, 2, 10, HORIZONTAL, GAUCHE, B_RED };
        //BossCollection[1] = { 2, 5, 2, 8, VERTICAL, HAUT, B_GREEN };
        //BossCollection[2] = { 6, 9, 2, 10, HORIZONTAL, GAUCHE, B_BLUE };
        //BossCollection[3] = { 10, 5, 2, 8, VERTICAL, HAUT, B_BLACK };

    }
    else if (numLevel==5) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 1;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 4;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 8;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=1;
        Map[numLevel].y=-2;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;

        joueur = { 6, 9, HAUT };
        lumiere = { 6, 6, 1,1,1,0, false };

        ObstaclesCollection[0] = {6,3,BLOCK_SUN};
        ObstaclesCollection[1] = {5,4,BLOCK_WATER};
        ObstaclesCollection[2] = {7,4,BLOCK_WATER};
        ObstaclesCollection[3] = {5,5,BLOCK_SUN};
        ObstaclesCollection[4] = {7,5,BLOCK_SUN};
        ObstaclesCollection[5] = {5,6,BLOCK_WATER};
        ObstaclesCollection[6] = {7,6,BLOCK_WATER};
        ObstaclesCollection[7] = {6,7,BLOCK_SUN};


        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        ReceptorCollection[0] = { 8, 0, BAS, 255, 0, 0, false };  // Receptor RED
        ReceptorCollection[1] = { 4, 10, HAUT, 0, 255, 0, false }; // Receptor GREEN
        ReceptorCollection[2] = { 4, 0, BAS, 0, 0, 255, false }; // Receptor BLUE
        ReceptorCollection[3] = { 8, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK
        // ReceptorCollection[4] = { 12, 3, GAUCHE, 255, 0, 255, false }; // Receptor PINK
        // ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
        // ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
        // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

        DoorCollection[0] = { 6,0, HAUT, false, true, 2, { 0, 2, -1 }, 9}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 1, { R_BLEU, -1, -1 }, 0}; // Door WEST
        //DoorCollection[2] = { 12, 5, DROITE, false, true, 1, { R_VERT, -1, -1 }, 0}; // Door EAST
        DoorCollection[1] = { 6, 10, BAS, false, true, 2, { 1, 3, -1 }, 3}; // Door SOUTH

        TreasureCollection[0] = {1,1, T_RED, false, PIERRE_BLANCHE, 100, true };
        TreasureCollection[1] = {11,1, T_YELLOW, false, PIERRE_BLANCHE, 100, true };
        TreasureCollection[2] = {11,9, T_GREEN, false, PIERRE_BLANCHE, 100, true };
        TreasureCollection[3] = {1,9, T_BLUE, false, PIERRE_BLANCHE, 100, true };

        //BossCollection[0] = { 6, 2, 2, 10, HORIZONTAL, GAUCHE, B_RED };
        //BossCollection[1] = { 2, 5, 2, 8, VERTICAL, HAUT, B_GREEN };
        //BossCollection[2] = { 6, 9, 2, 10, HORIZONTAL, GAUCHE, B_BLUE };
        //BossCollection[3] = { 10, 5, 2, 8, VERTICAL, HAUT, B_BLACK };
    }
    else if (numLevel==6) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 2;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 4;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 8;
        NbMonsterLevel = 8;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-2;
        Map[numLevel].y=-3;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=false;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0,1,2 }, 11}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 0}; // Door WEST
        DoorCollection[1] = { 12, 5, DROITE, false, true, 1, { 3, -1, -1 }, 7}; // Door EAST
        //DoorCollection[1] = { 6, 10, BAS, false, true, 0, { -1, -1, -1 }, 0}; // Door SOUTH

        ObstaclesCollection[0] = {5,4,BLOCK_WATER};
        ObstaclesCollection[1] = {5,5,BLOCK_WATER};
        ObstaclesCollection[2] = {5,6,BLOCK_WATER};
        ObstaclesCollection[3] = {6,6,BLOCK_WATER};
        ObstaclesCollection[4] = {7,6,BLOCK_WATER};
        ObstaclesCollection[5] = {7,5,BLOCK_WATER};
        ObstaclesCollection[6] = {7,4,BLOCK_WATER};
        ObstaclesCollection[7] = {6,4,BLOCK_WATER};

        ReceptorCollection[0] = { 5, 0, BAS, 255, 0, 0, false };  // Receptor RED
        ReceptorCollection[1] = { 0, 5, DROITE, 0, 255, 0, false }; // Receptor GREEN
        ReceptorCollection[2] = { 12, 4, GAUCHE, 0, 0, 255, false }; // Receptor BLUE
        ReceptorCollection[3] = { 6, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK
        // ReceptorCollection[4] = { 12, 3, GAUCHE, 255, 0, 255, false }; // Receptor PINK
        // ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
        // ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
        // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

        MonstersCollection[0] = { 5, 3, 1, 5, HORIZONTAL, GAUCHE, BLOB };
        MonstersCollection[1] = { 5, 2, 1, 5, HORIZONTAL, GAUCHE, BLOB };
        MonstersCollection[2] = { 7, 3, 7, 11, HORIZONTAL, DROITE, BLOB };
        MonstersCollection[3] = { 7, 2, 7, 11, HORIZONTAL, DROITE, BLOB };
        MonstersCollection[4] = { 5, 7, 1, 5, HORIZONTAL, GAUCHE, BLOB };
        MonstersCollection[5] = { 5, 8, 1, 5, HORIZONTAL, GAUCHE, BLOB };
        MonstersCollection[6] = { 7, 7, 7, 11, HORIZONTAL, DROITE, BLOB };
        MonstersCollection[7] = { 7, 8, 7, 11, HORIZONTAL, DROITE, BLOB };


    }
    else if (numLevel==7) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 6;
        NbDoorsLevel = 3;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 7;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-1;
        Map[numLevel].y=-3;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 7, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        ReceptorCollection[0] = { 12, 3, GAUCHE, 255, 0, 0, false };  // Receptor RED
        ReceptorCollection[1] = { 6, 0, BAS, 0, 255, 0, false }; // Receptor GREEN
        ReceptorCollection[2] = { 0, 3, DROITE, 0, 0, 255, false }; // Receptor BLUE
        ReceptorCollection[3] = { 11, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK
        //ReceptorCollection[4] = { 12, 3, GAUCHE, 255, 0, 255, false }; // Receptor PINK
        ReceptorCollection[4] = { 1, 0, BAS, 0, 255, 255, false }; // Receptor CYAN
        ReceptorCollection[5] = { 11, 0, BAS, 255, 255, 0, false }; // Receptor YELLOW
        // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 }, 11}; // Door NORTH
        DoorCollection[0] = { 0, 5, GAUCHE, false, true, 2, { 2, 0, -1 }, 6}; // Door WEST
        DoorCollection[1] = { 12, 5, DROITE, false, true, 3, { 1, 5, 4 }, 8}; // Door EAST
        DoorCollection[2] = { 6, 10, BAS, false, true, 1, { 3, -1, -1 }, 4}; // Door SOUTH

        ObstaclesCollection[0] = {5,6,BLOCK_WATER};
        //ObstaclesCollection[1] = {6,6,BLOCK_WATER};
        ObstaclesCollection[1] = {7,6,BLOCK_WATER};
        ObstaclesCollection[2] = {5,7,BLOCK_WATER};
        ObstaclesCollection[3] = {7,7,BLOCK_WATER};
        ObstaclesCollection[4] = {5,8,BLOCK_WATER};
        ObstaclesCollection[5] = {6,8,BLOCK_WATER};
        ObstaclesCollection[6] = {7,8,BLOCK_WATER};

    }
    else if (numLevel==8) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 2;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 5;
        NbDoorsLevel = 3;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 0;
        NbMonsterLevel = 0;
        NbBossLevel = 1;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=0;
        Map[numLevel].y=-3;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=false;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 2,3,4 },12}; // Door NORTH
        DoorCollection[1] = { 0, 5, GAUCHE, false, true, 3, { 1, 4, 2 }, 7}; // Door WEST
        DoorCollection[2] = { 12, 5, DROITE, false, true, 3, { 0, 3, 2 }, 9}; // Door EAST
        //DoorCollection[2] = { 6, 10, BAS, false, true, 0, { -1, -1, -1 }, 4}; // Door SOUTH


        ReceptorCollection[0] = { 12, 8, GAUCHE, 255, 0, 0, false };  // Receptor RED
        ReceptorCollection[1] = { 0, 8, DROITE, 0, 255, 0, false }; // Receptor GREEN
        //ReceptorCollection[2] = { 0, 6, DROITE, 0, 0, 255, false }; // Receptor BLUE
        ReceptorCollection[2] = { 6, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK
        ReceptorCollection[3] = { 9, 10, HAUT, 255, 0, 255, false }; // Receptor PINK
        ReceptorCollection[4] = { 3, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
        // ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
        // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE



        BossCollection[0] = { 6, 3, 2, 10, HORIZONTAL, GAUCHE, B_RED };

    }
    else if (numLevel==9) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 6;
        NbDoorsLevel = 3;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 7;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=1;
        Map[numLevel].y=-3;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 7, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        ReceptorCollection[0] = { 3, 0, BAS, 255, 0, 255, false };  // Receptor RED
        ReceptorCollection[1] = { 6, 0, BAS, 255, 0, 0, false }; // Receptor GREEN
        ReceptorCollection[2] = { 9, 0, BAS, 255, 255, 0, false }; // Receptor BLUE
        ReceptorCollection[3] = { 11, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK
        //ReceptorCollection[4] = { 12, 3, GAUCHE, 255, 0, 255, false }; // Receptor PINK
        ReceptorCollection[4] = { 1, 0, BAS, 255, 0, 0, false }; // Receptor CYAN
        ReceptorCollection[5] = { 11, 0, BAS, 255, 0, 0, false }; // Receptor YELLOW
        // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 }, 11}; // Door NORTH
        DoorCollection[0] = { 0, 5, GAUCHE, false, true, 3, { 0, 2, 1 }, 8}; // Door WEST
        DoorCollection[1] = { 12, 5, DROITE, false, true, 3, { 1, 4, 5 }, 10}; // Door EAST
        DoorCollection[2] = { 6, 10, BAS, false, true, 1, { 3, -1, -1 }, 5}; // Door SOUTH

        ObstaclesCollection[0] = {5,6,BLOCK_WATER};
        ObstaclesCollection[1] = {7,6,BLOCK_WATER};
        ObstaclesCollection[2] = {5,7,BLOCK_WATER};
        ObstaclesCollection[3] = {7,7,BLOCK_WATER};
        ObstaclesCollection[4] = {5,8,BLOCK_WATER};
        ObstaclesCollection[5] = {6,8,BLOCK_WATER};
        ObstaclesCollection[6] = {7,8,BLOCK_WATER};

    }
    else if (numLevel==10) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 4;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 8;
        NbMonsterLevel = 8;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=2;
        Map[numLevel].y=-3;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=false;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0,1,2 },13}; // Door NORTH
        DoorCollection[1] = { 0, 5, GAUCHE, false, true, 1, { 3, -1, -1 }, 9}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        //DoorCollection[2] = { 6, 10, BAS, false, true, 0, { -1, -1, -1 }, 5}; // Door SOUTH

        ObstaclesCollection[0] = {5,4,BLOCK_WATER};
        ObstaclesCollection[1] = {5,5,BLOCK_WATER};
        ObstaclesCollection[2] = {5,6,BLOCK_WATER};
        ObstaclesCollection[3] = {6,6,BLOCK_WATER};
        ObstaclesCollection[4] = {7,6,BLOCK_WATER};
        ObstaclesCollection[5] = {7,5,BLOCK_WATER};
        ObstaclesCollection[6] = {7,4,BLOCK_WATER};
        ObstaclesCollection[7] = {6,4,BLOCK_WATER};

        ReceptorCollection[0] = { 5, 0, BAS, 255, 0, 0, false };  // Receptor RED
        ReceptorCollection[1] = { 12, 5, GAUCHE, 0, 255, 0, false }; // Receptor GREEN
        ReceptorCollection[2] = { 0, 4, DROITE, 0, 0, 255, false }; // Receptor BLUE
        ReceptorCollection[3] = { 6, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK
        // ReceptorCollection[4] = { 12, 3, GAUCHE, 255, 0, 255, false }; // Receptor PINK
        // ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
        // ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
        // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

        MonstersCollection[0] = { 5, 3, 1, 5, HORIZONTAL, GAUCHE, GHOST };
        MonstersCollection[1] = { 5, 2, 1, 5, HORIZONTAL, GAUCHE, GHOST };
        MonstersCollection[2] = { 7, 3, 7, 11, HORIZONTAL, DROITE, GHOST };
        MonstersCollection[3] = { 7, 2, 7, 11, HORIZONTAL, DROITE, GHOST };
        MonstersCollection[4] = { 5, 7, 1, 5, HORIZONTAL, GAUCHE, GHOST };
        MonstersCollection[5] = { 5, 8, 1, 5, HORIZONTAL, GAUCHE, GHOST };
        MonstersCollection[6] = { 7, 7, 7, 11, HORIZONTAL, DROITE, GHOST };
        MonstersCollection[7] = { 7, 8, 7, 11, HORIZONTAL, DROITE, GHOST };
    }
    else if (numLevel==11) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 4;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 24;
        NbMonsterLevel = 4;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-2;
        Map[numLevel].y=-4;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        ReceptorCollection[0] = { 9, 1, BAS, 255, 0, 0, false };  // Receptor RED
        ReceptorCollection[1] = { 4, 10, HAUT, 0, 255, 0, false }; // Receptor GREEN
        ReceptorCollection[2] = { 3, 3, DROITE, 0, 0, 255, false }; // Receptor BLUE
        ReceptorCollection[3] = { 5, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK
        // ReceptorCollection[4] = { 12, 3, GAUCHE, 255, 0, 255, false }; // Receptor PINK
        // ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
        // ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
        // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE


        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0,1,2 },15}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 9}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        DoorCollection[1] = { 6, 10, BAS, false, true, 1, { 3, -1, -1 }, 6}; // Door SOUTH

        ObstaclesCollection[0] = {5,3,BLOCK_WATER};
        ObstaclesCollection[1] = {5,5,BLOCK_WATER};
        ObstaclesCollection[2] = {5,7,BLOCK_WATER};
        ObstaclesCollection[3] = {6,6,BLOCK_WATER};
        ObstaclesCollection[4] = {7,3,BLOCK_WATER};
        ObstaclesCollection[5] = {7,5,BLOCK_WATER};
        ObstaclesCollection[6] = {7,7,BLOCK_WATER};
        ObstaclesCollection[7] = {6,4,BLOCK_WATER};

        ObstaclesCollection[8] = {1,3,BLOCK_MOON};
        ObstaclesCollection[9] = {3,3,BLOCK_MOON};
        ObstaclesCollection[10] = {1,5,BLOCK_MOON};
        ObstaclesCollection[11] = {3,5,BLOCK_MOON};
        ObstaclesCollection[12] = {1,7,BLOCK_MOON};
        ObstaclesCollection[13] = {3,7,BLOCK_MOON};
        ObstaclesCollection[14] = {3,1,BLOCK_MOON};
        ObstaclesCollection[15] = {3,9,BLOCK_MOON};

        ObstaclesCollection[16] = {11,3,BLOCK_MOON};
        ObstaclesCollection[17] = {9,3,BLOCK_MOON};
        ObstaclesCollection[18] = {11,5,BLOCK_MOON};
        ObstaclesCollection[19] = {9,5,BLOCK_MOON};
        ObstaclesCollection[20] = {11,7,BLOCK_MOON};
        ObstaclesCollection[21] = {9,7,BLOCK_MOON};
        ObstaclesCollection[22] = {9,1,BLOCK_MOON};
        ObstaclesCollection[23] = {9,9,BLOCK_MOON};




        TreasureCollection[0] = {1,1, T_RED, false, HEART_SINGLE, 100, true };
        TreasureCollection[1] = {11,1, T_YELLOW, false, HEART_SINGLE, 100, true };
        TreasureCollection[2] = {11,9, T_GREEN, false, HEART_SINGLE, 100, true };
        TreasureCollection[3] = {1,9, T_BLUE, false, HEART_SINGLE, 100, true };

        MonstersCollection[0] = { 2, 1, 1, 9, VERTICAL, BAS, BAT };
        MonstersCollection[1] = { 10, 1, 1, 9, VERTICAL, HAUT, BAT };
        MonstersCollection[2] = { 1, 2, 1, 11, HORIZONTAL, DROITE, BAT };
        MonstersCollection[3] = { 11, 8, 1, 11, HORIZONTAL, GAUCHE, BAT };


    }
    else if (numLevel==12) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 2;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 1;
        NbDoorsLevel = 1;
        NbBlocksLevel = 4;
        NbTreasureLevel = 3;
        NbObstacleLevel = 0;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=0;
        Map[numLevel].y=-4;
        Map[numLevel].R=255;
        Map[numLevel].G=0;
        Map[numLevel].B=0;
        Map[numLevel].A=255;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        ReceptorCollection[0] = { 0, 5, DROITE, 255, 255, 255, false }; // Receptor WHITE

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 },6}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 9}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        DoorCollection[0] = { 6, 10, BAS, false, true, 1, { 0, -1, -1 }, 8}; // Door SOUTH

        TreasureCollection[0] = {6,2, T_RED, false, RED_FRAGMENT, 1000,  true };
        TreasureCollection[1] = {1,2, T_YELLOW, false, HEART_SINGLE, 100,  true };
        TreasureCollection[2] = {11,2, T_YELLOW, false, HEART_SINGLE, 100,  true };
        //TreasureCollection[3] = {1,1, T_BLUE, true, PIERRE_BLANCHE, 100,  true };

    }
    else if (numLevel==13) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 4;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 24;
        NbMonsterLevel = 4;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=2;
        Map[numLevel].y=-4;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];


        ReceptorCollection[0] = { 9, 1, BAS, 255, 0, 0, false };  // Receptor RED
        ReceptorCollection[1] = { 4, 10, HAUT, 0, 255, 0, false }; // Receptor GREEN
        ReceptorCollection[2] = { 3, 3, DROITE, 0, 0, 255, false }; // Receptor BLUE
        ReceptorCollection[3] = { 5, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK
        // ReceptorCollection[4] = { 12, 3, GAUCHE, 255, 0, 255, false }; // Receptor PINK
        // ReceptorCollection[5] = { 9, 10, HAUT, 0, 255, 255, false }; // Receptor CYAN
        // ReceptorCollection[6] = { 12, 8, GAUCHE, 255, 255, 0, false }; // Receptor YELLOW
        // ReceptorCollection[7] = { 0, 7, DROITE, 255, 255, 255, false }; // Receptor WHITE

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0,1, 2 },19}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 9}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        DoorCollection[1] = { 6, 10, BAS, false, true, 1, { 3, -1, -1 }, 10}; // Door SOUTH

        ObstaclesCollection[0] = {5,3,BLOCK_WATER};
        ObstaclesCollection[1] = {5,5,BLOCK_WATER};
        ObstaclesCollection[2] = {5,7,BLOCK_WATER};
        ObstaclesCollection[3] = {6,6,BLOCK_WATER};
        ObstaclesCollection[4] = {7,3,BLOCK_WATER};
        ObstaclesCollection[5] = {7,5,BLOCK_WATER};
        ObstaclesCollection[6] = {7,7,BLOCK_WATER};
        ObstaclesCollection[7] = {6,4,BLOCK_WATER};

        ObstaclesCollection[8] = {1,3,BLOCK_SUN};
        ObstaclesCollection[9] = {3,3,BLOCK_SUN};
        ObstaclesCollection[10] = {1,5,BLOCK_SUN};
        ObstaclesCollection[11] = {3,5,BLOCK_SUN};
        ObstaclesCollection[12] = {1,7,BLOCK_SUN};
        ObstaclesCollection[13] = {3,7,BLOCK_SUN};
        ObstaclesCollection[14] = {3,1,BLOCK_SUN};
        ObstaclesCollection[15] = {3,9,BLOCK_SUN};

        ObstaclesCollection[16] = {11,3,BLOCK_SUN};
        ObstaclesCollection[17] = {9,3,BLOCK_SUN};
        ObstaclesCollection[18] = {11,5,BLOCK_SUN};
        ObstaclesCollection[19] = {9,5,BLOCK_SUN};
        ObstaclesCollection[20] = {11,7,BLOCK_SUN};
        ObstaclesCollection[21] = {9,7,BLOCK_SUN};
        ObstaclesCollection[22] = {9,1,BLOCK_SUN};
        ObstaclesCollection[23] = {9,9,BLOCK_SUN};

        TreasureCollection[0] = {1,1, T_RED, false, HEART_SINGLE, 100, true };
        TreasureCollection[1] = {11,1, T_YELLOW, false, HEART_SINGLE, 100, true };
        TreasureCollection[2] = {11,9, T_GREEN, false, HEART_SINGLE, 100, true };
        TreasureCollection[3] = {1,9, T_BLUE, false, HEART_SINGLE, 100, true };

        MonstersCollection[0] = { 2, 1, 1, 9, VERTICAL, BAS, GHOST };
        MonstersCollection[1] = { 10, 9, 1, 9, VERTICAL, HAUT, BAT };
        MonstersCollection[2] = { 1, 2, 1, 11, HORIZONTAL, DROITE, SKELETON };
        MonstersCollection[3] = { 11, 8, 1, 11, HORIZONTAL, GAUCHE, SPIDER };
    }
    else if (numLevel==14) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 4;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 2;
        NbObstacleLevel = 20;
        NbMonsterLevel = 3;
        NbBossLevel = 1;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-3;
        Map[numLevel].y=-5;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=false;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0,1,2 }, 21}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 0}; // Door WEST
        DoorCollection[1] = { 12, 5, DROITE, false, true, 1, { 3, -1, -1 }, 15}; // Door EAST
        //DoorCollection[1] = { 6, 10, BAS, false, true, 0, { -1, -1, -1 }, 0}; // Door SOUTH

        ReceptorCollection[0] = { 1, 0, BAS, 255, 0, 0, false };  // Receptor RED
        ReceptorCollection[1] = { 11, 0, BAS, 0, 255, 0, false }; // Receptor GREEN
        ReceptorCollection[2] = { 11, 10, HAUT, 0, 0, 255, false }; // Receptor BLUE
        ReceptorCollection[3] = { 1, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK

        BossCollection[0] = { 6, 2, 2, 10, HORIZONTAL, GAUCHE, B_GREEN };

        ObstaclesCollection[0] = {1,4,BLOCK_WATER};
        ObstaclesCollection[1] = {1,6,BLOCK_WATER};
        ObstaclesCollection[2] = {3,4,BLOCK_WATER};
        ObstaclesCollection[3] = {3,6,BLOCK_WATER};
        ObstaclesCollection[4] = {4,4,BLOCK_WATER};
        ObstaclesCollection[5] = {4,6,BLOCK_WATER};

        ObstaclesCollection[6] = {8,4,BLOCK_WATER};
        ObstaclesCollection[7] = {8,6,BLOCK_WATER};
        ObstaclesCollection[8] = {9,4,BLOCK_WATER};
        ObstaclesCollection[9] = {9,6,BLOCK_WATER};
        ObstaclesCollection[10] = {11,4,BLOCK_WATER};
        ObstaclesCollection[11] = {11,6,BLOCK_WATER};

        ObstaclesCollection[12] = {5,1,BLOCK_WATER};
        ObstaclesCollection[13] = {7,1,BLOCK_WATER};
        ObstaclesCollection[14] = {5,3,BLOCK_WATER};
        ObstaclesCollection[15] = {7,3,BLOCK_WATER};

        ObstaclesCollection[16] = {5,7,BLOCK_WATER};
        ObstaclesCollection[17] = {7,7,BLOCK_WATER};
        ObstaclesCollection[18] = {5,9,BLOCK_WATER};
        ObstaclesCollection[19] = {7,9,BLOCK_WATER};

        TreasureCollection[0] = {1,5, T_GREEN, false, HEART_FULL, 10000,  true };
        TreasureCollection[1] = {6,9, T_YELLOW, false, PIERRE_BLANCHE, 1000,  true };

        //MonstersCollection[0] = { 6, 2, 4, 8, HORIZONTAL, DROITE, SKELETON };
        MonstersCollection[0] = { 6, 8, 4, 8, HORIZONTAL, DROITE, BLOB };

        MonstersCollection[1] = { 2, 5, 3, 7, VERTICAL, HAUT, BAT };
        MonstersCollection[2] = { 10, 5, 3, 7, VERTICAL, BAS, GHOST };

    }
    else if (numLevel==15)  // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 7;
        NbDoorsLevel = 3;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 12;
        NbMonsterLevel = 3;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-2;
        Map[numLevel].y=-5;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 }, 11}; // Door NORTH
        DoorCollection[0] = { 0, 5, GAUCHE, false, true, 3, { 4, 5, 6 }, 14}; // Door WEST
        DoorCollection[1] = { 12, 5, DROITE, false, true, 3, { 0, 1, 2 }, 16}; // Door EAST
        DoorCollection[2] = { 6, 10, BAS, false, true, 1, { 3, -1, -1 }, 11}; // Door SOUTH

        ReceptorCollection[0] = { 10, 0, BAS, 0, 0, 255, false };
        ReceptorCollection[1] = { 12, 2, GAUCHE, 255, 255, 0, false };
        ReceptorCollection[2] = { 11, 0, BAS, 255, 0, 255, false };

        ReceptorCollection[3] = { 1, 10, HAUT, 0, 0, 0, false };

        ReceptorCollection[4] = { 0, 2, DROITE, 255, 0, 255, false };
        ReceptorCollection[5] = { 1, 0, BAS, 0, 255, 255, false };
        ReceptorCollection[6] = { 3, 0, BAS, 0, 255, 0, false };


        ObstaclesCollection[0] = {6,2,BLOCK_WATER};
        ObstaclesCollection[1] = {5,3,BLOCK_WATER};
        ObstaclesCollection[2] = {7,3,BLOCK_WATER};
        ObstaclesCollection[3] = {4,4,BLOCK_WATER};
        ObstaclesCollection[4] = {8,4,BLOCK_WATER};
        ObstaclesCollection[5] = {3,5,BLOCK_WATER};
        ObstaclesCollection[6] = {9,5,BLOCK_WATER};
        ObstaclesCollection[7] = {4,6,BLOCK_WATER};
        ObstaclesCollection[8] = {8,6,BLOCK_WATER};
        ObstaclesCollection[9] = {5,7,BLOCK_WATER};
        ObstaclesCollection[10] = {7,7,BLOCK_WATER};
        ObstaclesCollection[11] = {6,8,BLOCK_WATER};

        MonstersCollection[0] = { 6, 1, 1, 11, HORIZONTAL, DROITE, GHOST };
        MonstersCollection[1] = { 1, 5, 1, 9, VERTICAL, HAUT, GHOST };
        MonstersCollection[2] = { 11, 5, 1, 9, VERTICAL, HAUT, GHOST };

    }
    else if (numLevel==16) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 1;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 9;
        NbDoorsLevel = 3;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 8;
        NbMonsterLevel = 4;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-1;
        Map[numLevel].y=-5;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=false;

        joueur = { 6, 9, HAUT };
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

        ReceptorCollection[0] = { 2, 8, HAUT, 0, 255, 0, false };
        ReceptorCollection[1] = { 9, 10, HAUT, 255, 0, 255, false };
        ReceptorCollection[2] = { 12, 2, GAUCHE, 0, 0, 0, false };

        ReceptorCollection[3] = { 10, 8, HAUT, 0, 0, 255, false };
        ReceptorCollection[4] = { 5, 10, HAUT, 255, 0, 0, false };
        ReceptorCollection[5] = { 8, 0, BAS, 0, 0, 0, false };

        ReceptorCollection[6] = { 2, 10, HAUT, 255, 255, 0, false };
        ReceptorCollection[7] = { 8, 10, HAUT, 255, 0, 255, false };
        ReceptorCollection[8] = { 12, 7, GAUCHE, 0, 0, 255, false };

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0,1, 2 }, 22}; // Door NORTH
        DoorCollection[1] = { 0, 5, GAUCHE, false, true, 3, { 3, 4, 5 }, 15}; // Door WEST
        DoorCollection[2] = { 12, 5, DROITE, false, true, 3, { 6, 7, 8 }, 18}; // Door EAST
        //DoorCollection[3] = { 6, 10, BAS, false, false, 0, { -1 -1, -1 }, -1}; // Door SOUTH

        TreasureCollection[0] = {4,4, T_RED, false, PIERRE_BLANCHE, 1000,  true };
        TreasureCollection[1] = {8,4, T_YELLOW, false, HEART_FULL, 1000,  true };
        TreasureCollection[2] = {4,6, T_GREEN, false, HEART_FULL, 1000,  true };
        TreasureCollection[3] = {8,6, T_BLUE, false, PIERRE_BLANCHE, 1000,  true };

        MonstersCollection[0] = { 6, 2, 1, 11, HORIZONTAL, DROITE, GHOST };
        MonstersCollection[1] = { 6, 8, 1, 11, HORIZONTAL, GAUCHE, GHOST };
        MonstersCollection[2] = { 2, 5, 1, 9, VERTICAL, HAUT, GHOST };
        MonstersCollection[3] = { 10, 5, 1, 9, VERTICAL, BAS, GHOST };

    }
    else if (numLevel==17) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 0;
        NbDoorsLevel = 3;
        NbBlocksLevel = 4;
        NbTreasureLevel = 1;
        NbObstacleLevel = 20;
        NbMonsterLevel = 4;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-0;
        Map[numLevel].y=-5;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=false;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 1, 1, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 11, 1, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 11, 9, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 1, 9, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 }, 23}; // Door NORTH
        DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 16}; // Door WEST
        DoorCollection[2] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 18}; // Door EAST
        //DoorCollection[2] = { 6, 10, BAS, false, true, 0, { -1, -1, -1 }, 4}; // Door SOUTH


        ObstaclesCollection[0] = {1,4,BLOCK_STONE};
        ObstaclesCollection[1] = {1,6,BLOCK_STONE};
        ObstaclesCollection[2] = {3,4,BLOCK_STONE};
        ObstaclesCollection[3] = {3,6,BLOCK_STONE};
        ObstaclesCollection[4] = {4,4,BLOCK_STONE};
        ObstaclesCollection[5] = {4,6,BLOCK_STONE};

        ObstaclesCollection[6] = {8,4,BLOCK_STONE};
        ObstaclesCollection[7] = {8,6,BLOCK_STONE};
        ObstaclesCollection[8] = {9,4,BLOCK_STONE};
        ObstaclesCollection[9] = {9,6,BLOCK_STONE};
        ObstaclesCollection[10] = {11,4,BLOCK_STONE};
        ObstaclesCollection[11] = {11,6,BLOCK_STONE};

        ObstaclesCollection[12] = {5,1,BLOCK_STONE};
        ObstaclesCollection[13] = {7,1,BLOCK_STONE};
        ObstaclesCollection[14] = {5,3,BLOCK_STONE};
        ObstaclesCollection[15] = {7,3,BLOCK_STONE};

        ObstaclesCollection[16] = {5,7,BLOCK_STONE};
        ObstaclesCollection[17] = {7,7,BLOCK_STONE};
        ObstaclesCollection[18] = {5,9,BLOCK_STONE};
        ObstaclesCollection[19] = {7,9,BLOCK_STONE};

        TreasureCollection[0] = {6,9, T_GREEN, false, HEART_FULL, 10000,  true };

        MonstersCollection[0] = { 6, 2, 4, 8, HORIZONTAL, DROITE, SKELETON };
        MonstersCollection[1] = { 6, 8, 4, 8, HORIZONTAL, DROITE, BLOB };

        MonstersCollection[2] = { 2, 5, 3, 7, VERTICAL, HAUT, BAT };
        MonstersCollection[3] = { 10, 5, 3, 7, VERTICAL, BAS, GHOST };


    }
    else if (numLevel==18) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 2;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 9;
        NbDoorsLevel = 3;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 8;
        NbMonsterLevel = 4;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=1;
        Map[numLevel].y=-5;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=false;

        joueur = { 6, 9, HAUT };
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

        ReceptorCollection[0] = { 2, 8, HAUT, 0, 255, 0, false };
        ReceptorCollection[1] = { 9, 10, HAUT, 255, 0, 255, false };
        ReceptorCollection[2] = { 12, 2, GAUCHE, 0, 0, 0, false };

        ReceptorCollection[3] = { 10, 8, HAUT, 0, 0, 255, false };
        ReceptorCollection[4] = { 5, 10, HAUT, 255, 0, 0, false };
        ReceptorCollection[5] = { 8, 0, BAS, 0, 0, 0, false };

        ReceptorCollection[6] = { 2, 10, HAUT, 255, 255, 0, false };
        ReceptorCollection[7] = { 8, 10, HAUT, 255, 0, 255, false };
        ReceptorCollection[8] = { 12, 7, GAUCHE, 0, 0, 255, false };

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0,1, 2 }, 24}; // Door NORTH
        DoorCollection[1] = { 0, 5, GAUCHE, false, true, 3, { 3, 4, 5 }, 17}; // Door WEST
        DoorCollection[2] = { 12, 5, DROITE, false, true, 3, { 6, 7, 8 }, 19}; // Door EAST
        //DoorCollection[3] = { 6, 10, BAS, false, false, 0, { -1 -1, -1 }, -1}; // Door SOUTH

        TreasureCollection[0] = {4,4, T_RED, false, PIERRE_BLANCHE, 1000,  true };
        TreasureCollection[1] = {8,4, T_YELLOW, false, HEART_FULL, 1000,  true };
        TreasureCollection[2] = {4,6, T_GREEN, false, HEART_FULL, 1000,  true };
        TreasureCollection[3] = {8,6, T_BLUE, false, PIERRE_BLANCHE, 1000,  true };

        MonstersCollection[0] = { 6, 2, 1, 11, HORIZONTAL, DROITE, BLOB };
        MonstersCollection[1] = { 6, 8, 1, 11, HORIZONTAL, GAUCHE, BLOB };
        MonstersCollection[2] = { 2, 5, 1, 9, VERTICAL, HAUT, BLOB };
        MonstersCollection[3] = { 10, 5, 1, 9, VERTICAL, BAS, BLOB };

    }
    else if (numLevel==19)  // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 1;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 7;
        NbDoorsLevel = 3;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 12;
        NbMonsterLevel = 3;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=2;
        Map[numLevel].y=-5;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 },12}; // Door NORTH
        DoorCollection[0] = { 0, 5, GAUCHE, false, true, 3, { 4, 5, 6 }, 18}; // Door WEST
        DoorCollection[1] = { 12, 5, DROITE, false, true, 3, { 0, 1, 2 }, 20}; // Door EAST
        DoorCollection[2] = { 6, 10, BAS, false, true, 1, { 3, -1, -1 }, 13}; // Door SOUTH


        ReceptorCollection[0] = { 8, 0, BAS, 255, 0, 0, false };
        ReceptorCollection[1] = { 9, 0, BAS, 255, 255, 0, false };
        ReceptorCollection[2] = { 11, 0, BAS, 0, 255, 0, false };

        ReceptorCollection[3] = { 1, 10, HAUT, 0, 0, 0, false };

        ReceptorCollection[4] = { 0, 2, DROITE, 255, 0, 255, false };
        ReceptorCollection[5] = { 1, 0, BAS, 0, 255, 255, false };
        ReceptorCollection[6] = { 3, 0, BAS, 0, 255, 0, false };


        ObstaclesCollection[0] = {6,2,BLOCK_WATER};
        ObstaclesCollection[1] = {5,3,BLOCK_WATER};
        ObstaclesCollection[2] = {7,3,BLOCK_WATER};
        ObstaclesCollection[3] = {4,4,BLOCK_WATER};
        ObstaclesCollection[4] = {8,4,BLOCK_WATER};
        ObstaclesCollection[5] = {3,5,BLOCK_WATER};
        ObstaclesCollection[6] = {9,5,BLOCK_WATER};
        ObstaclesCollection[7] = {4,6,BLOCK_WATER};
        ObstaclesCollection[8] = {8,6,BLOCK_WATER};
        ObstaclesCollection[9] = {5,7,BLOCK_WATER};
        ObstaclesCollection[10] = {7,7,BLOCK_WATER};
        ObstaclesCollection[11] = {6,8,BLOCK_WATER};

        MonstersCollection[0] = { 6, 1, 1, 11, HORIZONTAL, DROITE, SKELETON };
        MonstersCollection[1] = { 1, 5, 1, 9, VERTICAL, HAUT, SKELETON };
        MonstersCollection[2] = { 11, 5, 1, 9, VERTICAL, HAUT, SKELETON };

    }
    else if (numLevel==20) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 1;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 4;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 2;
        NbObstacleLevel = 20;
        NbMonsterLevel = 3;
        NbBossLevel = 1;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=3;
        Map[numLevel].y=-5;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=false;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0,1,2 },25}; // Door NORTH
        DoorCollection[1] = { 0, 5, GAUCHE, false, true, 1, { 3, -1, -1 }, 19}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        //DoorCollection[2] = { 6, 10, BAS, false, true, 0, { -1, -1, -1 }, 5}; // Door SOUTH


        ReceptorCollection[0] = { 1, 0, BAS, 255, 0, 0, false };  // Receptor RED
        ReceptorCollection[1] = { 11, 0, BAS, 0, 255, 0, false }; // Receptor GREEN
        ReceptorCollection[2] = { 11, 10, HAUT, 0, 0, 255, false }; // Receptor BLUE
        ReceptorCollection[3] = { 1, 10, HAUT, 0, 0, 0, false }; // Receptor BLACK

        BossCollection[0] = { 6, 2, 2, 10, HORIZONTAL, GAUCHE, B_BLUE };

        ObstaclesCollection[0] = {1,4,BLOCK_WATER};
        ObstaclesCollection[1] = {1,6,BLOCK_WATER};
        ObstaclesCollection[2] = {3,4,BLOCK_WATER};
        ObstaclesCollection[3] = {3,6,BLOCK_WATER};
        ObstaclesCollection[4] = {4,4,BLOCK_WATER};
        ObstaclesCollection[5] = {4,6,BLOCK_WATER};

        ObstaclesCollection[6] = {8,4,BLOCK_WATER};
        ObstaclesCollection[7] = {8,6,BLOCK_WATER};
        ObstaclesCollection[8] = {9,4,BLOCK_WATER};
        ObstaclesCollection[9] = {9,6,BLOCK_WATER};
        ObstaclesCollection[10] = {11,4,BLOCK_WATER};
        ObstaclesCollection[11] = {11,6,BLOCK_WATER};

        ObstaclesCollection[12] = {5,1,BLOCK_WATER};
        ObstaclesCollection[13] = {7,1,BLOCK_WATER};
        ObstaclesCollection[14] = {5,3,BLOCK_WATER};
        ObstaclesCollection[15] = {7,3,BLOCK_WATER};

        ObstaclesCollection[16] = {5,7,BLOCK_WATER};
        ObstaclesCollection[17] = {7,7,BLOCK_WATER};
        ObstaclesCollection[18] = {5,9,BLOCK_WATER};
        ObstaclesCollection[19] = {7,9,BLOCK_WATER};

        TreasureCollection[0] = {11,5, T_BLUE, false, HEART_FULL, 10000,  true };
        TreasureCollection[1] = {6,9, T_YELLOW, false, PIERRE_BLANCHE, 1000,  true };

        //MonstersCollection[0] = { 6, 2, 4, 8, HORIZONTAL, DROITE, SKELETON };
        MonstersCollection[0] = { 6, 8, 4, 8, HORIZONTAL, DROITE, SPIDER };

        MonstersCollection[1] = { 2, 5, 3, 7, VERTICAL, HAUT, SKELETON };
        MonstersCollection[2] = { 10, 5, 3, 7, VERTICAL, BAS, GHOST };
    }
    else if (numLevel==21) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 1;
        NbDoorsLevel = 1;
        NbBlocksLevel = 4;
        NbTreasureLevel = 3;
        NbObstacleLevel = 0;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-3;
        Map[numLevel].y=-6;
        Map[numLevel].R=0;
        Map[numLevel].G=255;
        Map[numLevel].B=0;
        Map[numLevel].A=255;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        ReceptorCollection[0] = { 0, 5, DROITE, 255, 255, 255, false }; // Receptor WHITE

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 },6}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 9}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        DoorCollection[0] = { 6, 10, BAS, false, true, 1, { 0, -1, -1 }, 14}; // Door SOUTH

        TreasureCollection[0] = {6,2, T_GREEN, false, GREEN_FRAGMENT, 1000,  true };
        TreasureCollection[1] = {1,2, T_YELLOW, false, HEART_SINGLE, 100,  true };
        TreasureCollection[2] = {11,2, T_YELLOW, false, HEART_SINGLE, 100,  true };
        //TreasureCollection[3] = {1,1, T_BLUE, true, PIERRE_BLANCHE, 100,  true };
    }
    else if (numLevel==22) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 6;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 16;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-1;
        Map[numLevel].y=-6;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0,1,2 },27}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 9}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        DoorCollection[1] = { 6, 10, BAS, false, true, 3, { 3,4,5 }, 16}; // Door SOUTH

        ReceptorCollection[0] = { 2, 6, DROITE, 0, 255, 255, false };
        ReceptorCollection[1] = { 2, 2, DROITE, 0, 0, 255, false };
        ReceptorCollection[2] = { 5, 2, BAS, 0, 0, 0, false };
        ReceptorCollection[3] = { 7, 2, BAS, 0, 0, 0, false };
        ReceptorCollection[4] = { 10, 2, GAUCHE, 0, 0, 255, false };
        ReceptorCollection[5] = { 10, 6, GAUCHE, 255, 0, 255, false };

        ObstaclesCollection[0] = {2,2,BLOCK_SUN};
        ObstaclesCollection[1] = {5,2,BLOCK_SUN};
        ObstaclesCollection[2] = {7,2,BLOCK_SUN};
        ObstaclesCollection[3] = {10,2,BLOCK_SUN};

        ObstaclesCollection[4] = {2,4,BLOCK_SUN};
        ObstaclesCollection[5] = {5,4,BLOCK_WATER};
        ObstaclesCollection[6] = {7,4,BLOCK_WATER};
        ObstaclesCollection[7] = {10,4,BLOCK_SUN};

        ObstaclesCollection[8] = {2,6,BLOCK_SUN};
        ObstaclesCollection[9] = {5,6,BLOCK_WATER};
        ObstaclesCollection[10] = {7,6,BLOCK_WATER};
        ObstaclesCollection[11] = {10,6,BLOCK_SUN};

        ObstaclesCollection[12] = {2,8,BLOCK_SUN};
        ObstaclesCollection[13] = {5,8,BLOCK_SUN};
        ObstaclesCollection[14] = {7,8,BLOCK_SUN};
        ObstaclesCollection[15] = {10,8,BLOCK_SUN};
    }
    else if (numLevel==23) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 6;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 5;
        NbObstacleLevel = 7;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-0;
        Map[numLevel].y=-6;
        Map[numLevel].R=0;
        Map[numLevel].G=0;
        Map[numLevel].B=0;
        Map[numLevel].A=255;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 4, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        ReceptorCollection[0] = { 2, 10, HAUT, 0, 0, 255, false }; // Receptor WHITE
        ReceptorCollection[1] = { 3, 10, HAUT, 0, 255, 255, false }; // Receptor WHITE
        ReceptorCollection[2] = { 5, 7, HAUT, 0, 0, 255, false }; // Receptor WHITE
        ReceptorCollection[3] = { 7, 7, HAUT, 255, 0, 0, false }; // Receptor WHITE
        ReceptorCollection[4] = { 9, 10, HAUT, 0, 0, 0, false }; // Receptor WHITE
        ReceptorCollection[5] = { 10, 10, HAUT, 255, 0, 0, false }; // Receptor WHITE


        ObstaclesCollection[0] = {5,9,BLOCK_STONE};
        ObstaclesCollection[1] = {5,8,BLOCK_WATER};
        ObstaclesCollection[2] = {5,7,BLOCK_STONE};
        ObstaclesCollection[3] = {6,7,BLOCK_WATER};
        ObstaclesCollection[4] = {7,7,BLOCK_STONE};
        ObstaclesCollection[5] = {7,8,BLOCK_WATER};
        ObstaclesCollection[6] = {7,9,BLOCK_STONE};

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0, 1, 5 }, 28}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 9}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        DoorCollection[1] = { 6, 10, BAS, false, true, 2, { 2, 3, -1 }, 17}; // Door SOUTH

        TreasureCollection[0] = {6,6, T_GREEN, false, BLACK_FRAGMENT, 1000,  true };
        TreasureCollection[1] = {4,6, T_YELLOW, false, HEART_SINGLE, 100,  true };
        TreasureCollection[2] = {8,6, T_YELLOW, false, HEART_SINGLE, 100,  true };
        TreasureCollection[3] = {2,6, T_BLUE, false, PIERRE_BLANCHE, 1000,  true };
        TreasureCollection[4] = {10,6, T_BLUE, false, PIERRE_BLANCHE, 1000,  true };

    }
    else if (numLevel==24) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 6;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 16;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=1;
        Map[numLevel].y=-6;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0,1,2 },29}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 9}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        DoorCollection[1] = { 6, 10, BAS, false, true, 3, { 3,4,5 }, 18}; // Door SOUTH

        ReceptorCollection[0] = { 2, 6, DROITE, 255, 255, 0, false };
        ReceptorCollection[1] = { 2, 2, DROITE, 255, 0, 0, false };
        ReceptorCollection[2] = { 5, 2, BAS, 255, 0, 255, false };
        ReceptorCollection[3] = { 7, 2, BAS, 255, 0, 255, false };
        ReceptorCollection[4] = { 10, 2, GAUCHE, 255, 0, 0, false };
        ReceptorCollection[5] = { 10, 6, GAUCHE, 0, 0, 0, false };

        ObstaclesCollection[0] = {2,2,BLOCK_SUN};
        ObstaclesCollection[1] = {5,2,BLOCK_SUN};
        ObstaclesCollection[2] = {7,2,BLOCK_SUN};
        ObstaclesCollection[3] = {10,2,BLOCK_SUN};

        ObstaclesCollection[4] = {2,4,BLOCK_SUN};
        ObstaclesCollection[5] = {5,4,BLOCK_WATER};
        ObstaclesCollection[6] = {7,4,BLOCK_WATER};
        ObstaclesCollection[7] = {10,4,BLOCK_SUN};

        ObstaclesCollection[8] = {2,6,BLOCK_SUN};
        ObstaclesCollection[9] = {5,6,BLOCK_WATER};
        ObstaclesCollection[10] = {7,6,BLOCK_WATER};
        ObstaclesCollection[11] = {10,6,BLOCK_SUN};

        ObstaclesCollection[12] = {2,8,BLOCK_SUN};
        ObstaclesCollection[13] = {5,8,BLOCK_SUN};
        ObstaclesCollection[14] = {7,8,BLOCK_SUN};
        ObstaclesCollection[15] = {10,8,BLOCK_SUN};
    }
    else if (numLevel==25) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 2;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 1;
        NbDoorsLevel = 1;
        NbBlocksLevel = 4;
        NbTreasureLevel = 3;
        NbObstacleLevel = 0;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=3;
        Map[numLevel].y=-6;
        Map[numLevel].R=0;
        Map[numLevel].G=0;
        Map[numLevel].B=255;
        Map[numLevel].A=255;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        ReceptorCollection[0] = { 0, 5, DROITE, 255, 255, 255, false }; // Receptor WHITE

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 },6}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 9}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        DoorCollection[0] = { 6, 10, BAS, false, true, 1, { 0, -1, -1 }, 20}; // Door SOUTH

        TreasureCollection[0] = {6,2, T_BLUE, false, BLUE_FRAGMENT, 1000,  true };
        TreasureCollection[1] = {1,2, T_YELLOW, false, HEART_SINGLE, 100,  true };
        TreasureCollection[2] = {11,2, T_YELLOW, false, HEART_SINGLE, 100,  true };
        //TreasureCollection[3] = {1,1, T_BLUE, true, PIERRE_BLANCHE, 100,  true };
    }
    else if (numLevel==26) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 1;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 3;
        NbDoorsLevel = 1;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 12;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-2;
        Map[numLevel].y=-7;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=false;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 },6}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 9}; // Door WEST
        DoorCollection[0] = { 12, 5, DROITE, false, true, 3, { 0, 1, 2 }, 27}; // Door EAST
        //DoorCollection[0] = { 6, 10, BAS, false, true, 0, { -1, -1, -1 }, 14}; // Door SOUTH

        ReceptorCollection[0] = { 0, 5, DROITE, 255, 255, 255, false };
        ReceptorCollection[1] = { 2, 1, BAS, 0, 255, 255, false };
        ReceptorCollection[2] = { 2, 9, DROITE, 0, 0, 0, false };

        TreasureCollection[0] = {2, 2, T_BLUE, false, HEART_SINGLE, 100,  true };
        TreasureCollection[1] = {10,2, T_BLUE, false, HEART_SINGLE, 100,  true };
        TreasureCollection[2] = {2, 8, T_BLUE, false, HEART_SINGLE, 100,  true };
        TreasureCollection[3] = {10,8, T_BLUE, false, HEART_SINGLE, 100,  true };

        ObstaclesCollection[0] = {1,1,BLOCK_STONE};
        ObstaclesCollection[1] = {2,1,BLOCK_STONE};
        ObstaclesCollection[2] = {1,2,BLOCK_STONE};

        ObstaclesCollection[3] = {11,1,BLOCK_STONE};
        ObstaclesCollection[4] = {10,1,BLOCK_STONE};
        ObstaclesCollection[5] = {11,2,BLOCK_STONE};

        ObstaclesCollection[6] = {1,9,BLOCK_STONE};
        ObstaclesCollection[7] = {2,9,BLOCK_STONE};
        ObstaclesCollection[8] = {1,8,BLOCK_STONE};

        ObstaclesCollection[9] = {11,9,BLOCK_STONE};
        ObstaclesCollection[10] = {10,9,BLOCK_STONE};
        ObstaclesCollection[11] = {11,8,BLOCK_STONE};

    }
    else if (numLevel==27)  // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 7;
        NbDoorsLevel = 3;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 12;
        NbMonsterLevel = 3;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-1;
        Map[numLevel].y=-7;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 }, 11}; // Door NORTH
        DoorCollection[0] = { 0, 5, GAUCHE, false, true, 3, { 4, 5, 6 }, 26}; // Door WEST
        DoorCollection[1] = { 12, 5, DROITE, false, true, 3, { 0, 1, 2 }, 28}; // Door EAST
        DoorCollection[2] = { 6, 10, BAS, false, true, 1, { 3, -1, -1 }, 22}; // Door SOUTH



        ReceptorCollection[0] = { 10, 0, BAS, 0, 0, 255, false };
        ReceptorCollection[1] = { 12, 2, GAUCHE, 255, 255, 0, false };
        ReceptorCollection[2] = { 11, 0, BAS, 255, 0, 255, false };

        ReceptorCollection[3] = { 1, 10, HAUT, 0, 0, 0, false };

        ReceptorCollection[4] = { 0, 2, DROITE, 255, 0, 255, false };
        ReceptorCollection[5] = { 1, 0, BAS, 0, 255, 255, false };
        ReceptorCollection[6] = { 3, 0, BAS, 0, 255, 0, false };


        ObstaclesCollection[0] = {6,2,BLOCK_WATER};
        ObstaclesCollection[1] = {5,3,BLOCK_WATER};
        ObstaclesCollection[2] = {7,3,BLOCK_WATER};
        ObstaclesCollection[3] = {4,4,BLOCK_WATER};
        ObstaclesCollection[4] = {8,4,BLOCK_WATER};
        ObstaclesCollection[5] = {3,5,BLOCK_WATER};
        ObstaclesCollection[6] = {9,5,BLOCK_WATER};
        ObstaclesCollection[7] = {4,6,BLOCK_WATER};
        ObstaclesCollection[8] = {8,6,BLOCK_WATER};
        ObstaclesCollection[9] = {5,7,BLOCK_WATER};
        ObstaclesCollection[10] = {7,7,BLOCK_WATER};
        ObstaclesCollection[11] = {6,8,BLOCK_WATER};

        MonstersCollection[0] = { 6, 1, 1, 11, HORIZONTAL, DROITE, GHOST };
        MonstersCollection[1] = { 1, 5, 1, 9, VERTICAL, HAUT, GHOST };
        MonstersCollection[2] = { 11, 5, 1, 9, VERTICAL, HAUT, GHOST };

    }
    else if (numLevel==28)  // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 1;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 12;
        NbDoorsLevel = 4;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 0;
        NbMonsterLevel = 4;
        NbBossLevel = 1;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=0;
        Map[numLevel].y=-7;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 6, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 0,7,2 },31}; // Door NORTH
        DoorCollection[1] = { 0, 5, GAUCHE, false, true, 3, { 10, 1, 4 }, 27}; // Door WEST
        DoorCollection[2] = { 12, 5, DROITE, false, true, 3, { 6, 9, 11 }, 29}; // Door EAST
        DoorCollection[3] = { 6, 10, BAS, false, true, 3, { 8, 3, 5 }, 23}; // Door SOUTH


        BossCollection[0] = { 6, 5, 2, 10, HORIZONTAL, GAUCHE, B_BLACK };

        MonstersCollection[0] = { 3, 5, 1, 9, VERTICAL, HAUT, SKELETON };
        MonstersCollection[1] = { 5, 5, 1, 9, VERTICAL, BAS, SKELETON };
        MonstersCollection[2] = { 7, 5, 1, 9, VERTICAL, BAS, SKELETON };
        MonstersCollection[3] = { 9, 5, 1, 9, VERTICAL, HAUT, SKELETON };

        ReceptorCollection[0] = { 1, 0, BAS, 255, 255, 0, false };
        ReceptorCollection[1] = { 0, 6, DROITE, 255, 255, 0, false };
        ReceptorCollection[2] = { 0, 7, DROITE, 255, 255, 255, false };
        ReceptorCollection[3] = { 0, 9, DROITE, 255, 255, 255, false };
        ReceptorCollection[4] = { 2, 10, HAUT, 255, 0, 255, false };
        ReceptorCollection[5] = { 4, 10, HAUT, 255, 0, 255, false };
        ReceptorCollection[6] = { 12, 1, GAUCHE, 255, 0, 0, false };
        ReceptorCollection[7] = { 5, 0, BAS, 0, 0, 0, false };
        ReceptorCollection[8] = { 2, 0, BAS, 255, 0, 0, false };
        ReceptorCollection[9] = { 7, 0, BAS, 0, 0, 0, false };
        ReceptorCollection[10] = { 11, 0, BAS, 255, 0, 0, false };
        ReceptorCollection[11] = { 12, 7, GAUCHE, 255, 0, 0, false };

    }
    else if (numLevel==29)  // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 1;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 7;
        NbDoorsLevel = 3;
        NbBlocksLevel = 4;
        NbTreasureLevel = 0;
        NbObstacleLevel = 12;
        NbMonsterLevel = 3;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=1;
        Map[numLevel].y=-7;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=true;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 },12}; // Door NORTH
        DoorCollection[0] = { 0, 5, GAUCHE, false, true, 3, { 4, 5, 6 }, 28}; // Door WEST
        DoorCollection[1] = { 12, 5, DROITE, false, true, 3, { 0, 1, 2 }, 30}; // Door EAST
        DoorCollection[2] = { 6, 10, BAS, false, true, 1, { 3, -1, -1 }, 24}; // Door SOUTH


        ReceptorCollection[0] = { 8, 0, BAS, 255, 0, 0, false };
        ReceptorCollection[1] = { 9, 0, BAS, 255, 255, 0, false };
        ReceptorCollection[2] = { 11, 0, BAS, 0, 255, 0, false };

        ReceptorCollection[3] = { 1, 10, HAUT, 0, 0, 0, false };

        ReceptorCollection[4] = { 0, 2, DROITE, 255, 0, 255, false };
        ReceptorCollection[5] = { 1, 0, BAS, 0, 255, 255, false };
        ReceptorCollection[6] = { 3, 0, BAS, 0, 255, 0, false };


        ObstaclesCollection[0] = {6,2,BLOCK_WATER};
        ObstaclesCollection[1] = {5,3,BLOCK_WATER};
        ObstaclesCollection[2] = {7,3,BLOCK_WATER};
        ObstaclesCollection[3] = {4,4,BLOCK_WATER};
        ObstaclesCollection[4] = {8,4,BLOCK_WATER};
        ObstaclesCollection[5] = {3,5,BLOCK_WATER};
        ObstaclesCollection[6] = {9,5,BLOCK_WATER};
        ObstaclesCollection[7] = {4,6,BLOCK_WATER};
        ObstaclesCollection[8] = {8,6,BLOCK_WATER};
        ObstaclesCollection[9] = {5,7,BLOCK_WATER};
        ObstaclesCollection[10] = {7,7,BLOCK_WATER};
        ObstaclesCollection[11] = {6,8,BLOCK_WATER};

        MonstersCollection[0] = { 6, 1, 1, 11, HORIZONTAL, DROITE, SKELETON };
        MonstersCollection[1] = { 1, 5, 1, 9, VERTICAL, HAUT, SKELETON };
        MonstersCollection[2] = { 11, 5, 1, 9, VERTICAL, HAUT, SKELETON };

    }
    else if (numLevel==30) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 3;
        NbDoorsLevel = 1;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 12;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=2;
        Map[numLevel].y=-7;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=true;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=false;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 },12}; // Door NORTH
        DoorCollection[0] = { 0, 5, GAUCHE, false, true, 3, { 0, 1, 2 }, 29}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 30}; // Door EAST
        //DoorCollection[2] = { 6, 10, BAS, false, true, 0, { -1, -1, -1 }, 24}; // Door SOUTH

        ReceptorCollection[0] = { 12, 5, GAUCHE, 255, 255, 255, false };
        ReceptorCollection[1] = { 10, 1, BAS, 255, 0, 255, false };
        ReceptorCollection[2] = { 2, 9, DROITE, 0, 0, 0, false };

        TreasureCollection[0] = {2, 2, T_BLUE, false, HEART_SINGLE, 100,  true };
        TreasureCollection[1] = {10,2, T_BLUE, false, HEART_SINGLE, 100,  true };
        TreasureCollection[2] = {2, 8, T_BLUE, false, HEART_SINGLE, 100,  true };
        TreasureCollection[3] = {10,8, T_BLUE, false, HEART_SINGLE, 100,  true };

        ObstaclesCollection[0] = {1,1,BLOCK_STONE};
        ObstaclesCollection[1] = {2,1,BLOCK_STONE};
        ObstaclesCollection[2] = {1,2,BLOCK_STONE};

        ObstaclesCollection[3] = {11,1,BLOCK_STONE};
        ObstaclesCollection[4] = {10,1,BLOCK_STONE};
        ObstaclesCollection[5] = {11,2,BLOCK_STONE};

        ObstaclesCollection[6] = {1,9,BLOCK_STONE};
        ObstaclesCollection[7] = {2,9,BLOCK_STONE};
        ObstaclesCollection[8] = {1,8,BLOCK_STONE};

        ObstaclesCollection[9] = {11,9,BLOCK_STONE};
        ObstaclesCollection[10] = {10,9,BLOCK_STONE};
        ObstaclesCollection[11] = {11,8,BLOCK_STONE};
    }
    else if (numLevel==31)  // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 5;
        NbDoorsLevel = 2;
        NbBlocksLevel = 4;
        NbTreasureLevel = 4;
        NbObstacleLevel = 13;
        NbMonsterLevel = 0;
        NbBossLevel = 1;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=-0;
        Map[numLevel].y=-8;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=0;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=true;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 6, 7, 1,1,1,0, false };

        BlocksCollection[0] = { 8, 3, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 4, 7, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 4, 3, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 8, 7, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        DoorCollection[0] = { 6,0, HAUT, false, true, 3, { 2,3,4 },32}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 9}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        DoorCollection[1] = { 6, 10, BAS, false, true, 2, { 0,1, -1 }, 28}; // Door SOUTH


        ReceptorCollection[0] = { 1, 1, DROITE, 255, 0, 0, false };
        ReceptorCollection[1] = { 11, 1, GAUCHE, 255, 0, 0, false };
        ReceptorCollection[2] = { 6, 3, BAS, 0, 0, 0, false }; // Receptor WHITE
        ReceptorCollection[3] = { 3, 1, BAS, 255, 0, 255, false }; // Receptor WHITE
        ReceptorCollection[4] = { 9, 1, BAS, 255, 255, 0, false }; // Receptor WHITE


        TreasureCollection[0] = {1,5, T_RED, false, PIERRE_BLANCHE, 100, true };
        TreasureCollection[1] = {11,5, T_YELLOW, false, PIERRE_BLANCHE, 100, true };
        TreasureCollection[2] = {11,7, T_GREEN, false, PIERRE_BLANCHE, 100, true };
        TreasureCollection[3] = {1,7, T_BLUE, false, PIERRE_BLANCHE, 100, true };

        ObstaclesCollection[0] = {1,1,BLOCK_STONE};
        ObstaclesCollection[1] = {3,1,BLOCK_STONE};
        ObstaclesCollection[2] = {5,1,BLOCK_STONE};

        ObstaclesCollection[3] = {7,1,BLOCK_STONE};
        ObstaclesCollection[4] = {9,1,BLOCK_STONE};
        ObstaclesCollection[5] = {11,1,BLOCK_STONE};

        ObstaclesCollection[6] = {1,9,BLOCK_STONE};
        ObstaclesCollection[7] = {3,9,BLOCK_STONE};
        ObstaclesCollection[8] = {5,9,BLOCK_STONE};

        ObstaclesCollection[9] = {7,9,BLOCK_STONE};
        ObstaclesCollection[10] = {9,9,BLOCK_STONE};
        ObstaclesCollection[11] = {11,9,BLOCK_STONE};

        ObstaclesCollection[12] = {6,3,BLOCK_STONE};


        BossCollection[0] = { 6, 4, 1, 11, HORIZONTAL, GAUCHE, BIGBOSS };
    }
    else if (numLevel==32) // VALIDATED [OK]
    {
        currentLevel = numLevel;

        LEVELCOLOR = 0;
        SIZE_MAP_X=13;
        SIZE_MAP_Y=11;

        NbReceptorsLevel = 0;
        NbDoorsLevel = 1;
        NbBlocksLevel = 4;
        NbTreasureLevel = 1;
        NbObstacleLevel = 32;
        NbMonsterLevel = 0;
        NbBossLevel = 0;

        BlocksCollection = (Blocks*) malloc( NbBlocksLevel * sizeof( Blocks) );
        ReceptorCollection = (Receptor*) malloc( NbReceptorsLevel * sizeof( Receptor) );
        DoorCollection = (Door*) malloc( NbDoorsLevel * sizeof( Door) );
        TreasureCollection = (Treasure*) malloc( NbTreasureLevel * sizeof( Treasure) );
        ObstaclesCollection = (Obstacle*) malloc( NbObstacleLevel * sizeof( Obstacle) );
        MonstersCollection = (Monster*) malloc( NbMonsterLevel * sizeof( Monster) );
        BossCollection = (Boss*) malloc( NbBossLevel * sizeof( Boss ) );

        Map[numLevel].x=0;
        Map[numLevel].y=-9;
        Map[numLevel].R=255;
        Map[numLevel].G=255;
        Map[numLevel].B=255;
        Map[numLevel].A=255;
        Map[numLevel].visited=true;
        Map[numLevel].doorWest=false;
        Map[numLevel].doorNorth=false;
        Map[numLevel].doorEast=false;
        Map[numLevel].doorSouth=true;


        joueur = { 6, 9, HAUT };
        lumiere = { 3, 5, 1,1,1,0, false };

        BlocksCollection[0] = { 2, 4, 255, 0, 0, 127, TRANSPARENT };
        BlocksCollection[1] = { 2, 5, 0, 255, 0, 127, TRANSPARENT };
        BlocksCollection[2] = { 2, 6, 0, 0, 255, 127, TRANSPARENT };
        BlocksCollection[3] = { 10, 5, 0, 0, 0, 255, OPAQUE };

        BlocRouge = &BlocksCollection[0];
        BlocVert = &BlocksCollection[1];
        BlocBleu = &BlocksCollection[2];
        BlocNoir = &BlocksCollection[3];

        //DoorCollection[0] = { 6,0, HAUT, false, true, 0, { -1,-1, -1 },6}; // Door NORTH
        //DoorCollection[1] = { 0, 5, GAUCHE, false, true, 0, { -1, -1, -1 }, 9}; // Door WEST
        //DoorCollection[1] = { 12, 5, DROITE, false, true, 0, { -1, -1, -1 }, 10}; // Door EAST
        DoorCollection[0] = { 6, 10, BAS, false, true, 0, { -1, -1, -1 }, 31}; // Door SOUTH

        TreasureCollection[0] = {6,2, T_BLUE, false, WHITE_FRAGMENT, 10000,  true };

    ObstaclesCollection[0] = {1,1,BLOCK_STONE};
    ObstaclesCollection[1] = {2,1,BLOCK_STONE};
    ObstaclesCollection[2] = {1,2,BLOCK_STONE};

    ObstaclesCollection[3] = {11,1,BLOCK_STONE};
    ObstaclesCollection[4] = {10,1,BLOCK_STONE};
    ObstaclesCollection[5] = {11,2,BLOCK_STONE};

    ObstaclesCollection[6] = {1,9,BLOCK_STONE};
    ObstaclesCollection[7] = {2,9,BLOCK_STONE};
    ObstaclesCollection[8] = {1,8,BLOCK_STONE};

    ObstaclesCollection[9] = {11,9,BLOCK_STONE};
    ObstaclesCollection[10] = {10,9,BLOCK_STONE};
    ObstaclesCollection[11] = {11,8,BLOCK_STONE};

    ObstaclesCollection[12] = {4,1,BLOCK_WATER};
    ObstaclesCollection[13] = {4,2,BLOCK_WATER};
    ObstaclesCollection[14] = {4,3,BLOCK_WATER};
    ObstaclesCollection[15] = {4,4,BLOCK_WATER};
    ObstaclesCollection[16] = {5,4,BLOCK_WATER};
    ObstaclesCollection[17] = {5,5,BLOCK_WATER};
    ObstaclesCollection[18] = {5,6,BLOCK_WATER};
    ObstaclesCollection[19] = {5,7,BLOCK_WATER};
    ObstaclesCollection[20] = {5,8,BLOCK_WATER};
    ObstaclesCollection[21] = {5,9,BLOCK_WATER};


    ObstaclesCollection[22] = {8,1,BLOCK_WATER};
    ObstaclesCollection[23] = {8,2,BLOCK_WATER};
    ObstaclesCollection[24] = {8,3,BLOCK_WATER};
    ObstaclesCollection[25] = {8,4,BLOCK_WATER};
    ObstaclesCollection[26] = {7,4,BLOCK_WATER};
    ObstaclesCollection[27] = {7,5,BLOCK_WATER};
    ObstaclesCollection[28] = {7,6,BLOCK_WATER};
    ObstaclesCollection[29] = {7,7,BLOCK_WATER};
    ObstaclesCollection[30] = {7,8,BLOCK_WATER};
    ObstaclesCollection[31] = {7,9,BLOCK_WATER};


    }





    if (needcorrection==true)
        adjustLevel( numLevel );
}


void synchroniseGame( void )
{
    score = sauvegarde.currentScore[0]*256+sauvegarde.currentScore[1];
    currentLevel = sauvegarde.currentLevel;
    difficulty = sauvegarde.currentDifficulty;

    life = sauvegarde.currentLife;

    RedFrag = sauvegarde.redFrag==1 ? true : false;
    GreenFrag = sauvegarde.greenFrag==1 ? true : false;
    BlueFrag = sauvegarde.blueFrag==1 ? true : false;
    BlackFrag = sauvegarde.blackFrag==1 ? true : false;
    WhiteFrag = sauvegarde.whiteFrag==1 ? true : false;

    for( unsigned char k =0; k<33; k++)
    {
        if (sauvegarde.visitedLevels[k]==0x01)
        {
            loadLevel( k );
        }
        else Map[k].visited= false;
    }
}


void exitAndFree( void )
{
    free(BlocksCollection);
    free(ReceptorCollection);
    free(DoorCollection);
    free(TreasureCollection);
    free(ObstaclesCollection);
    free(MonstersCollection);
    free(BossCollection);
    free(BulletCollection);
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
        bool Vis = TreasureCollection[k].isvisible;

        unsigned int lX = SIZE*Xb+OFFSET_X;
        unsigned int lY = SIZE*Yb+OFFSET_Y;

        if (Vis==true)
        {

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
    for( unsigned char k=0; k<=NB_LEVELS; k++ )
    {
        if (Map[k].visited==true)
        {
            unsigned char R= Map[k].R;
            unsigned char G= Map[k].G;
            unsigned char B= Map[k].B;
            unsigned char A= Map[k].A;

            unsigned int lX = OFFSET_X_MINIMAP + Map[k].x*(SCALE_X_MINIMAP+2);
            unsigned int lY = OFFSET_Y_MINIMAP + Map[k].y*(SCALE_Y_MINIMAP+2);

            _boxRGBA( lX-4, lY-3, lX+4, lY+3, R, G, B, A>>1 );
            _rectangleRGBA( lX-4, lY-3, lX+4, lY+3, R, G, B, 255);

            if (Map[k].doorWest==true) _lineRGBA( lX-5, lY-1, lX-5, lY+1, 255, 255, 255, 255);
            if (Map[k].doorEast==true) _lineRGBA( lX+5, lY-1, lX+5, lY+1, 255, 255, 255, 255);
            if (Map[k].doorNorth==true) _lineRGBA( lX-1, lY-4, lX+1, lY-4, 255, 255, 255, 255);
            if (Map[k].doorSouth==true) _lineRGBA( lX-1, lY+4, lX+1, lY+4, 255, 255, 255, 255);

            if (k==currentLevel)
            {
                _rectangleRGBA( lX-4, lY-3, lX+4, lY+3, 255, 0, 255, 255);
                _rectangleRGBA( lX-3, lY-2, lX+3, lY+2, 255, 0, 255, 255);
            }
        }
    }
}

void drawInterface( void )
{
    unsigned int midX, midY;

    dsubimage( 232, 0, &parchemin, 0,0, 164, 210,  DIMAGE_NONE);
    dfont( &font_fantasy );

    dprint( 256, 36, C_RGB(150,150,150), "MAGIC Light %s", RevShort);
    dprint( 255, 35, C_BLACK, "MAGIC Light %s", RevShort);

    dfont( &font_tiny );

    dprint( 251, 56, C_RGB(150,150,150), "Score : %d", score );
    dprint( 250, 55, C_BLACK, "Score : %d", score );

    dprint( 251, 66, C_RGB(150,150,150), "Level : %d", currentLevel );
    dprint( 250, 65, C_BLACK, "Level : %d",  currentLevel );

    dprint( 316, 56, C_RGB(150,150,150), "Life :");
    dprint( 315, 55, C_BLACK, "Life :" );

    for( unsigned char k = 0; k< life; k++)
    {
        dsubimage( 340 + k*8, 57, &hearts, 8, 0, 8, 8,  DIMAGE_NONE);
    }

    for( unsigned char k = life; k< lifeMax; k++)
    {
        dsubimage( 340 + k*8, 57, &hearts, 0, 0, 8, 8,  DIMAGE_NONE);
    }


    if (WhiteFrag==true) dsubimage( 340, 66, &fragments, 0, 0, 32, 32,  DIMAGE_NONE);
    if (BlackFrag==true) dsubimage( 340, 66, &fragments, 32, 0, 32, 32,  DIMAGE_NONE);
    if (RedFrag==true) dsubimage( 340, 66, &fragments, 64, 0, 32, 32,  DIMAGE_NONE);
    if (GreenFrag==true) dsubimage( 340, 66, &fragments, 96, 0, 32, 32,  DIMAGE_NONE);
    if (BlueFrag==true) dsubimage( 340, 66, &fragments, 128, 0, 32, 32,  DIMAGE_NONE);


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
    if (selected == JOUEUR)
        for(unsigned char k=0; k<NbDoorsLevel; k++)
        {
            if ( x==DoorCollection[k].x && y==DoorCollection[k].y)
            {
                if (DoorCollection[k].isdoor && DoorCollection[k].isopen)
                {
                    updateCurrentState( );
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

bool checkNextPositionMonster( unsigned int Xtarget, unsigned int Ytarget, unsigned int direction )
{

    for( unsigned char k=0; k<NbObstacleLevel; k++)
    {
        if (ObstaclesCollection[k].x==Xtarget  && ObstaclesCollection[k].y==Ytarget)
            return false;
    }

    //if (Xtarget>=lumiere.x-1 && Xtarget<=lumiere.x+1 && Ytarget>=lumiere.y-1 && Ytarget<=lumiere.y+1)
    if (Xtarget==lumiere.x && Ytarget==lumiere.y)
    {
        return false;
    }
    else if (Xtarget==joueur.x && Ytarget==joueur.y)
    {
        if (life>0) life--;
        return false;
    }
    else if (Xtarget==BlocRouge->x && Ytarget==BlocRouge->y)
    {
        if (direction==HAUT)
        {
            if (isValidMove( ROUGE, BlocRouge->x, BlocRouge->y-1, HAUT))
            {
                BlocRouge->y-=1;
                return true;
            }
            else return false;
        }
        else if (direction==BAS)
        {
            if (isValidMove( ROUGE, BlocRouge->x, BlocRouge->y+1, BAS))
            {
                BlocRouge->y+=1;
                return true;
            }
            else return false;
        }
        else if (direction==GAUCHE)
        {
            if (isValidMove( ROUGE, BlocRouge->x-1, BlocRouge->y, GAUCHE))
            {
                BlocRouge->x-=1;
                return true;
            }
            else return false;
        }
        else if (direction==DROITE)
        {
            if (isValidMove( ROUGE, BlocRouge->x+1, BlocRouge->y, DROITE))
            {
                BlocRouge->x+=1;
                return true;
            }
            else return false;
        }
    }
    else if (Xtarget==BlocVert->x && Ytarget==BlocVert->y)
    {
        if (direction==HAUT)
        {
            if (isValidMove( VERT, BlocVert->x, BlocVert->y-1, HAUT))
            {
                BlocVert->y-=1;
                return true;
            }
            else return false;
        }
        else if (direction==BAS)
        {
            if (isValidMove( VERT, BlocVert->x, BlocVert->y+1, BAS))
            {
                BlocVert->y+=1;
                return true;
            }
            else return false;
        }
        else if (direction==GAUCHE)
        {
            if (isValidMove( VERT, BlocVert->x-1, BlocVert->y, GAUCHE))
            {
                BlocVert->x-=1;
                return true;
            }
            else return false;
        }
        else if (direction==DROITE)
        {
            if (isValidMove( VERT, BlocVert->x+1, BlocVert->y, DROITE))
            {
                BlocVert->x+=1;
                return true;
            }
            else return false;
        }
    }
    else if (Xtarget==BlocBleu->x && Ytarget==BlocBleu->y)
    {
        if (direction==HAUT)
        {
            if (isValidMove( BLEU, BlocBleu->x, BlocBleu->y-1, HAUT))
            {
                BlocBleu->y-=1;
                return true;
            }
            else return false;
        }
        else if (direction==BAS)
        {
            if (isValidMove( BLEU, BlocBleu->x, BlocBleu->y+1, BAS))
            {
                BlocBleu->y+=1;
                return true;
            }
            else return false;
        }
        else if (direction==GAUCHE)
        {
            if (isValidMove( BLEU, BlocBleu->x-1, BlocBleu->y, GAUCHE))
            {
                BlocBleu->x-=1;
                return true;
            }
            else return false;
        }
        else if (direction==DROITE)
        {
            if (isValidMove( BLEU, BlocBleu->x+1, BlocBleu->y, DROITE))
            {
                BlocBleu->x+=1;
                return true;
            }
            else return false;
        }
    }
    else if (Xtarget==BlocNoir->x && Ytarget==BlocNoir->y)
    {
        if (direction==HAUT)
        {
            if (isValidMove( NOIR, BlocNoir->x, BlocNoir->y-1, HAUT))
            {
                BlocNoir->y-=1;
                return true;
            }
            else return false;
        }
        else if (direction==BAS)
        {
            if (isValidMove( NOIR, BlocNoir->x, BlocNoir->y+1, BAS))
            {
                BlocNoir->y+=1;
                return true;
            }
            else return false;
        }
        else if (direction==GAUCHE)
        {
            if (isValidMove( NOIR, BlocNoir->x-1, BlocNoir->y, GAUCHE))
            {
                BlocNoir->x-=1;
                return true;
            }
            else return false;
        }
        else if (direction==DROITE)
        {
            if (isValidMove( NOIR, BlocNoir->x+1, BlocNoir->y, DROITE))
            {
                BlocNoir->x+=1;
                return true;
            }
            else return false;
        }
    }

    return true;
}

void updateMonsters( void )
{
    for( unsigned char k=0; k<NbMonsterLevel; k++ )
    {
        unsigned int X = MonstersCollection[k].xcur;
        unsigned int Y = MonstersCollection[k].ycur;
        unsigned char mini = MonstersCollection[k].mini;
        unsigned char maxi = MonstersCollection[k].maxi;
        unsigned int direction = MonstersCollection[k].direction;
        unsigned int sens = MonstersCollection[k].sens;

        if (direction==VERTICAL)
        {
            if (sens==BAS)
            {
                if (Y<maxi && checkNextPositionMonster( X, Y+1, BAS)==true)
                {
                    MonstersCollection[k].ycur++;
                    MonstersCollection[k].sens=BAS;
                }
                else
                {
                    MonstersCollection[k].sens=HAUT;
                }
            }
            else if (sens==HAUT)
            {
                if (Y>mini && checkNextPositionMonster( X, Y-1, HAUT)==true)
                {
                    MonstersCollection[k].ycur--;
                    MonstersCollection[k].sens=HAUT;
                }
                else
                {
                    MonstersCollection[k].sens=BAS;
                }
            }
        }
        else if (direction==HORIZONTAL)
        {

            if (sens==DROITE)
            {
                if (X<maxi && checkNextPositionMonster( X+1, Y, DROITE)==true)
                {
                    MonstersCollection[k].xcur++;
                    MonstersCollection[k].sens=DROITE;
                }
                else
                {
                    MonstersCollection[k].sens=GAUCHE;
                }
            }
            else if (sens==GAUCHE)
            {
                if (X>mini && checkNextPositionMonster( X-1, Y, GAUCHE)==true)
                {
                    MonstersCollection[k].xcur--;
                    MonstersCollection[k].sens=GAUCHE;
                }
                else
                {
                    MonstersCollection[k].sens=DROITE;
                }
            }
        }

    }
}


void renderMonsters( void )
{
    for( unsigned char k=0; k<NbMonsterLevel; k++ )
    {
        unsigned int lX = SIZE*MonstersCollection[k].xcur+OFFSET_X;
        unsigned int lY = SIZE*MonstersCollection[k].ycur+OFFSET_Y;
        unsigned char mini = MonstersCollection[k].mini;
        unsigned char maxi = MonstersCollection[k].maxi;
        unsigned int direction = MonstersCollection[k].direction;
        unsigned int sens = MonstersCollection[k].sens;
        unsigned int type = MonstersCollection[k].kind;



        unsigned int OFFSET_X_MONSTER = 48*type;

        if (direction==VERTICAL)
        {

            /*
                              if (true)
                             {
                                _lineRGBA( lX+SIZE/2, mini*SIZE+OFFSET_Y+SIZE/2, lX+SIZE/2, maxi*SIZE+OFFSET_Y+SIZE/2, 255, 0, 0, 255 );
                                _lineRGBA( lX+1+SIZE/2, mini*SIZE+OFFSET_Y+SIZE/2, lX+1+SIZE/2, maxi*SIZE+OFFSET_Y+SIZE/2, 255, 0, 0, 255 );
                                _lineRGBA( lX-1+SIZE/2, mini*SIZE+OFFSET_Y+SIZE/2, lX-1+SIZE/2, maxi*SIZE+OFFSET_Y+SIZE/2, 255, 0, 0, 255 );
                             }
            */
            if (sens==BAS && MonstersCollection[k].ycur<maxi)
            {
                if (frame_monster==0) dsubimage( lX, lY, &monstres, OFFSET_X_MONSTER,0,16,16,  DIMAGE_NONE);
                else if (frame_monster==1) dsubimage( lX, lY+3, &monstres, OFFSET_X_MONSTER+16,0,16,16,  DIMAGE_NONE);
                else if (frame_monster==2) dsubimage( lX, lY+5, &monstres, OFFSET_X_MONSTER+32,0,16,16,  DIMAGE_NONE);
                else if (frame_monster==3) dsubimage( lX, lY+8, &monstres, OFFSET_X_MONSTER,0,16,16,  DIMAGE_NONE);
                else if (frame_monster==4) dsubimage( lX, lY+11, &monstres, OFFSET_X_MONSTER+16,0,16,16,  DIMAGE_NONE);
                else if (frame_monster==5) dsubimage( lX, lY+13, &monstres, OFFSET_X_MONSTER+32,0,16,16,  DIMAGE_NONE);
            }
            else if (sens==BAS && MonstersCollection[k].ycur==maxi)
            {
                dsubimage( lX, lY, &monstres, OFFSET_X_MONSTER,0,16,16,  DIMAGE_NONE);
            }
            else if (sens==HAUT && MonstersCollection[k].ycur>mini)
            {
                if (frame_monster==0) dsubimage( lX, lY, &monstres, OFFSET_X_MONSTER,32,16,16,  DIMAGE_NONE);
                else if (frame_monster==1) dsubimage( lX, lY-3, &monstres, OFFSET_X_MONSTER+16,32,16,16,  DIMAGE_NONE);
                else if (frame_monster==2) dsubimage( lX, lY-5, &monstres, OFFSET_X_MONSTER+32,32,16,16,  DIMAGE_NONE);
                else if (frame_monster==3) dsubimage( lX, lY-8, &monstres, OFFSET_X_MONSTER,32,16,16,  DIMAGE_NONE);
                else if (frame_monster==4) dsubimage( lX, lY-11, &monstres, OFFSET_X_MONSTER+16,32,16,16,  DIMAGE_NONE);
                else if (frame_monster==5) dsubimage( lX, lY-13, &monstres, OFFSET_X_MONSTER+32,32,16,16,  DIMAGE_NONE);
            }
            else if (sens==HAUT && MonstersCollection[k].ycur==mini)
            {
                dsubimage( lX, lY, &monstres, OFFSET_X_MONSTER,32,16,16,  DIMAGE_NONE);
            }
        }

        else if (direction==HORIZONTAL)
        {

            /*
                             if (true)
                             {
                                _lineRGBA( mini*SIZE+OFFSET_X+SIZE/2, lY+SIZE/2, maxi*SIZE+OFFSET_X+SIZE/2, lY+SIZE/2, 255, 0, 0, 255 );
                                _lineRGBA( mini*SIZE+OFFSET_X+SIZE/2, lY+1+SIZE/2, maxi*SIZE+OFFSET_X+SIZE/2, lY+1+SIZE/2, 255, 0, 0, 255 );
                                _lineRGBA( mini*SIZE+OFFSET_X+SIZE/2, lY-1+SIZE/2, maxi*SIZE+OFFSET_X+SIZE/2, lY-1+SIZE/2, 255, 0, 0, 255 );
                             }
            */

            if (sens==GAUCHE && MonstersCollection[k].xcur>mini)
            {
                if (frame_monster==0) dsubimage( lX, lY, &monstres, OFFSET_X_MONSTER,48,16,16,  DIMAGE_NONE);
                else if (frame_monster==1) dsubimage( lX-3, lY, &monstres, OFFSET_X_MONSTER+16,48,16,16,  DIMAGE_NONE);
                else if (frame_monster==2) dsubimage( lX-5, lY, &monstres, OFFSET_X_MONSTER+32,48,16,16,  DIMAGE_NONE);
                else if (frame_monster==3) dsubimage( lX-8, lY, &monstres, OFFSET_X_MONSTER,48,16,16,  DIMAGE_NONE);
                else if (frame_monster==4) dsubimage( lX-11, lY, &monstres, OFFSET_X_MONSTER+16,48,16,16,  DIMAGE_NONE);
                else if (frame_monster==5) dsubimage( lX-13, lY, &monstres, OFFSET_X_MONSTER+32,48,16,16,  DIMAGE_NONE);
            }
            else if (sens==GAUCHE && MonstersCollection[k].xcur==mini)
            {
                dsubimage( lX, lY, &monstres, OFFSET_X_MONSTER,48,16,16,  DIMAGE_NONE);
            }
            else if (sens==DROITE && MonstersCollection[k].xcur<maxi)
            {
                if (frame_monster==0) dsubimage( lX, lY, &monstres, OFFSET_X_MONSTER,16,16,16,  DIMAGE_NONE);
                else if (frame_monster==1) dsubimage( lX+3, lY, &monstres, OFFSET_X_MONSTER+16,16,16,16,  DIMAGE_NONE);
                else if (frame_monster==2) dsubimage( lX+5, lY, &monstres, OFFSET_X_MONSTER+32,16,16,16,  DIMAGE_NONE);
                else if (frame_monster==3) dsubimage( lX+8, lY, &monstres, OFFSET_X_MONSTER,16,16,16,  DIMAGE_NONE);
                else if (frame_monster==4) dsubimage( lX+11, lY, &monstres, OFFSET_X_MONSTER+16,16,16,16,  DIMAGE_NONE);
                else if (frame_monster==5) dsubimage( lX+13, lY, &monstres, OFFSET_X_MONSTER+32,16,16,16,  DIMAGE_NONE);
            }
            else if (sens==DROITE && MonstersCollection[k].xcur==maxi)
            {
                dsubimage( lX, lY, &monstres, OFFSET_X_MONSTER,16,16,16,  DIMAGE_NONE);
            }
        }

    }

    frame_monster++;
    if (frame_monster==6)
    {
        updateMonsters();
        frame_monster=0;
    }

}



bool checkNextPositionBoss( unsigned int Xtarget, unsigned int Ytarget, unsigned int direction )
{

    for( unsigned char k=0; k<NbObstacleLevel; k++)
    {
        if (ObstaclesCollection[k].x==Xtarget  && ObstaclesCollection[k].y==Ytarget)
            return false;
    }

    //if (Xtarget>=lumiere.x-1 && Xtarget<=lumiere.x+1 && Ytarget>=lumiere.y-1 && Ytarget<=lumiere.y+1)
    if (Xtarget==lumiere.x && Ytarget==lumiere.y)
    {
        return false;
    }
    else if (Xtarget==joueur.x && Ytarget==joueur.y)
    {
        if (life>0) life--;
        return false;
    }
    else if (Xtarget==BlocRouge->x && Ytarget==BlocRouge->y)
    {
        if (direction==HAUT)
        {
            if (isValidMove( ROUGE, BlocRouge->x, BlocRouge->y-1, HAUT))
            {
                BlocRouge->y-=1;
                return true;
            }
            else return false;
        }
        else if (direction==BAS)
        {
            if (isValidMove( ROUGE, BlocRouge->x, BlocRouge->y+1, BAS))
            {
                BlocRouge->y+=1;
                return true;
            }
            else return false;
        }
        else if (direction==GAUCHE)
        {
            if (isValidMove( ROUGE, BlocRouge->x-1, BlocRouge->y, GAUCHE))
            {
                BlocRouge->x-=1;
                return true;
            }
            else return false;
        }
        else if (direction==DROITE)
        {
            if (isValidMove( ROUGE, BlocRouge->x+1, BlocRouge->y, DROITE))
            {
                BlocRouge->x+=1;
                return true;
            }
            else return false;
        }
    }
    else if (Xtarget==BlocVert->x && Ytarget==BlocVert->y)
    {
        if (direction==HAUT)
        {
            if (isValidMove( VERT, BlocVert->x, BlocVert->y-1, HAUT))
            {
                BlocVert->y-=1;
                return true;
            }
            else return false;
        }
        else if (direction==BAS)
        {
            if (isValidMove( VERT, BlocVert->x, BlocVert->y+1, BAS))
            {
                BlocVert->y+=1;
                return true;
            }
            else return false;
        }
        else if (direction==GAUCHE)
        {
            if (isValidMove( VERT, BlocVert->x-1, BlocVert->y, GAUCHE))
            {
                BlocVert->x-=1;
                return true;
            }
            else return false;
        }
        else if (direction==DROITE)
        {
            if (isValidMove( VERT, BlocVert->x+1, BlocVert->y, DROITE))
            {
                BlocVert->x+=1;
                return true;
            }
            else return false;
        }
    }
    else if (Xtarget==BlocBleu->x && Ytarget==BlocBleu->y)
    {
        if (direction==HAUT)
        {
            if (isValidMove( BLEU, BlocBleu->x, BlocBleu->y-1, HAUT))
            {
                BlocBleu->y-=1;
                return true;
            }
            else return false;
        }
        else if (direction==BAS)
        {
            if (isValidMove( BLEU, BlocBleu->x, BlocBleu->y+1, BAS))
            {
                BlocBleu->y+=1;
                return true;
            }
            else return false;
        }
        else if (direction==GAUCHE)
        {
            if (isValidMove( BLEU, BlocBleu->x-1, BlocBleu->y, GAUCHE))
            {
                BlocBleu->x-=1;
                return true;
            }
            else return false;
        }
        else if (direction==DROITE)
        {
            if (isValidMove( BLEU, BlocBleu->x+1, BlocBleu->y, DROITE))
            {
                BlocBleu->x+=1;
                return true;
            }
            else return false;
        }
    }
    else if (Xtarget==BlocNoir->x && Ytarget==BlocNoir->y)
    {
        if (direction==HAUT)
        {
            if (isValidMove( NOIR, BlocNoir->x, BlocNoir->y-1, HAUT))
            {
                BlocNoir->y-=1;
                return true;
            }
            else return false;
        }
        else if (direction==BAS)
        {
            if (isValidMove( NOIR, BlocNoir->x, BlocNoir->y+1, BAS))
            {
                BlocNoir->y+=1;
                return true;
            }
            else return false;
        }
        else if (direction==GAUCHE)
        {
            if (isValidMove( NOIR, BlocNoir->x-1, BlocNoir->y, GAUCHE))
            {
                BlocNoir->x-=1;
                return true;
            }
            else return false;
        }
        else if (direction==DROITE)
        {
            if (isValidMove( NOIR, BlocNoir->x+1, BlocNoir->y, DROITE))
            {
                BlocNoir->x+=1;
                return true;
            }
            else return false;
        }
    }

    return true;
}


void updateBoss( void )
{
    for( unsigned char k=0; k<NbBossLevel; k++ )
    {
        unsigned int X = BossCollection[k].xcur;
        unsigned int Y = BossCollection[k].ycur;
        unsigned char mini = BossCollection[k].mini;
        unsigned char maxi = BossCollection[k].maxi;
        unsigned int direction = BossCollection[k].direction;
        unsigned int sens = BossCollection[k].sens;

        if (direction==VERTICAL)
        {
            if (sens==BAS)
            {
                if (Y<maxi && checkNextPositionMonster( X, Y+1, BAS)==true)
                {
                    BossCollection[k].ycur++;
                    BossCollection[k].sens=BAS;
                }
                else
                {
                    BossCollection[k].sens=HAUT;
                }
            }
            else if (sens==HAUT)
            {
                if (Y>mini && checkNextPositionMonster( X, Y-1, HAUT)==true)
                {
                    BossCollection[k].ycur--;
                    BossCollection[k].sens=HAUT;
                }
                else
                {
                    BossCollection[k].sens=BAS;
                }
            }
        }
        else if (direction==HORIZONTAL)
        {

            if (sens==DROITE)
            {
                if (X<maxi && checkNextPositionMonster( X+1, Y, DROITE)==true)
                {
                    BossCollection[k].xcur++;
                    BossCollection[k].sens=DROITE;
                }
                else
                {
                    BossCollection[k].sens=GAUCHE;
                }
            }
            else if (sens==GAUCHE)
            {
                if (X>mini && checkNextPositionMonster( X-1, Y, GAUCHE)==true)
                {
                    BossCollection[k].xcur--;
                    BossCollection[k].sens=GAUCHE;
                }
                else
                {
                    BossCollection[k].sens=DROITE;
                }
            }
        }

    }
}



void renderBoss( void )
{

    for( unsigned char k=0; k<NbBossLevel; k++ )
    {
        unsigned int lX = SIZE*BossCollection[k].xcur+OFFSET_X;
        unsigned int lY = SIZE*BossCollection[k].ycur+OFFSET_Y - 2; //Boss are 18 pixel high instead of 16
        unsigned char mini = BossCollection[k].mini;
        unsigned char maxi = BossCollection[k].maxi;
        unsigned int direction = BossCollection[k].direction;
        unsigned int sens = BossCollection[k].sens;
        unsigned int type = BossCollection[k].color;

        if (type ==BIGBOSS)
        {
            lX -= 8;
            lY -= 14;

            unsigned int OFFSET_X_MONSTER = 0;

            if (direction==VERTICAL)
            {

                /*
                                  if (true)
                                 {
                                    _lineRGBA( lX+SIZE/2, mini*SIZE+OFFSET_Y+SIZE/2, lX+SIZE/2, maxi*SIZE+OFFSET_Y+SIZE/2, 255, 0, 0, 255 );
                                    _lineRGBA( lX+1+SIZE/2, mini*SIZE+OFFSET_Y+SIZE/2, lX+1+SIZE/2, maxi*SIZE+OFFSET_Y+SIZE/2, 255, 0, 0, 255 );
                                    _lineRGBA( lX-1+SIZE/2, mini*SIZE+OFFSET_Y+SIZE/2, lX-1+SIZE/2, maxi*SIZE+OFFSET_Y+SIZE/2, 255, 0, 0, 255 );
                                 }
                */
                if (sens==BAS && BossCollection[k].ycur<maxi)
                {
                    if (frame_boss==0) dsubimage( lX, lY, &bigboss, OFFSET_X_MONSTER,64,32,32,  DIMAGE_NONE);
                    else if (frame_boss==1) dsubimage( lX, lY+3, &bigboss, OFFSET_X_MONSTER+32,64,32,32,  DIMAGE_NONE);
                    else if (frame_boss==2) dsubimage( lX, lY+5, &bigboss, OFFSET_X_MONSTER+64,64,32,32,  DIMAGE_NONE);
                    else if (frame_boss==3) dsubimage( lX, lY+8, &bigboss, OFFSET_X_MONSTER,64,32,32,  DIMAGE_NONE);
                    else if (frame_boss==4) dsubimage( lX, lY+11, &bigboss, OFFSET_X_MONSTER+32,64,32,32,  DIMAGE_NONE);
                    else if (frame_boss==5) dsubimage( lX, lY+13, &bigboss, OFFSET_X_MONSTER+64,64,32,32,  DIMAGE_NONE);
                }
                else if (sens==BAS && BossCollection[k].ycur==maxi)
                {
                    dsubimage( lX, lY, &bigboss, OFFSET_X_MONSTER,64,32,32,  DIMAGE_NONE);
                }
                else if (sens==HAUT && BossCollection[k].ycur>mini)
                {
                    if (frame_boss==0) dsubimage( lX, lY, &bigboss, OFFSET_X_MONSTER,0,32,32,  DIMAGE_NONE);
                    else if (frame_boss==1) dsubimage( lX, lY-3, &bigboss, OFFSET_X_MONSTER+32,0,32,32,  DIMAGE_NONE);
                    else if (frame_boss==2) dsubimage( lX, lY-5, &bigboss, OFFSET_X_MONSTER+64,0,32,32,  DIMAGE_NONE);
                    else if (frame_boss==3) dsubimage( lX, lY-8, &bigboss, OFFSET_X_MONSTER,32,0,32,  DIMAGE_NONE);
                    else if (frame_boss==4) dsubimage( lX, lY-11, &bigboss, OFFSET_X_MONSTER+32,0,32,32,  DIMAGE_NONE);
                    else if (frame_boss==5) dsubimage( lX, lY-13, &bigboss, OFFSET_X_MONSTER+64,0,32,32,  DIMAGE_NONE);
                }
                else if (sens==HAUT && BossCollection[k].ycur==mini)
                {
                    dsubimage( lX, lY, &bigboss, OFFSET_X_MONSTER,0,32,32,  DIMAGE_NONE);
                }
            }

            else if (direction==HORIZONTAL)
            {

                /*
                                 if (true)
                                 {
                                    _lineRGBA( mini*SIZE+OFFSET_X+SIZE/2, lY+SIZE/2, maxi*SIZE+OFFSET_X+SIZE/2, lY+SIZE/2, 255, 0, 0, 255 );
                                    _lineRGBA( mini*SIZE+OFFSET_X+SIZE/2, lY+1+SIZE/2, maxi*SIZE+OFFSET_X+SIZE/2, lY+1+SIZE/2, 255, 0, 0, 255 );
                                    _lineRGBA( mini*SIZE+OFFSET_X+SIZE/2, lY-1+SIZE/2, maxi*SIZE+OFFSET_X+SIZE/2, lY-1+SIZE/2, 255, 0, 0, 255 );
                                 }
                */

                if (sens==GAUCHE && BossCollection[k].xcur>mini)
                {
                    if (frame_boss==0) dsubimage( lX, lY, &bigboss, OFFSET_X_MONSTER,96,32,32,  DIMAGE_NONE);
                    else if (frame_boss==1) dsubimage( lX-3, lY, &bigboss, OFFSET_X_MONSTER+32,96,32,32,  DIMAGE_NONE);
                    else if (frame_boss==2) dsubimage( lX-5, lY, &bigboss, OFFSET_X_MONSTER+64,96,32,32,  DIMAGE_NONE);
                    else if (frame_boss==3) dsubimage( lX-8, lY, &bigboss, OFFSET_X_MONSTER,96,32,32,  DIMAGE_NONE);
                    else if (frame_boss==4) dsubimage( lX-11, lY, &bigboss, OFFSET_X_MONSTER+32,96,32,32,  DIMAGE_NONE);
                    else if (frame_boss==5) dsubimage( lX-13, lY, &bigboss, OFFSET_X_MONSTER+64,96,32,32,  DIMAGE_NONE);
                }
                else if (sens==GAUCHE && BossCollection[k].xcur==mini)
                {
                    dsubimage( lX, lY, &bigboss, OFFSET_X_MONSTER,96,32,32,  DIMAGE_NONE);
                }
                else if (sens==DROITE && BossCollection[k].xcur<maxi)
                {
                    if (frame_boss==0) dsubimage( lX, lY, &bigboss, OFFSET_X_MONSTER,32,32,32,  DIMAGE_NONE);
                    else if (frame_boss==1) dsubimage( lX+3, lY, &bigboss, OFFSET_X_MONSTER+32,32,32,32,  DIMAGE_NONE);
                    else if (frame_boss==2) dsubimage( lX+5, lY, &bigboss, OFFSET_X_MONSTER+64,32,32,32,  DIMAGE_NONE);
                    else if (frame_boss==3) dsubimage( lX+8, lY, &bigboss, OFFSET_X_MONSTER,32,32,32,  DIMAGE_NONE);
                    else if (frame_boss==4) dsubimage( lX+11, lY, &bigboss, OFFSET_X_MONSTER+32,32,32,32,  DIMAGE_NONE);
                    else if (frame_boss==5) dsubimage( lX+13, lY, &bigboss, OFFSET_X_MONSTER+64,32,32,32,  DIMAGE_NONE);
                }
                else if (sens==DROITE && BossCollection[k].xcur==maxi)
                {
                    dsubimage( lX, lY, &bigboss, OFFSET_X_MONSTER,32,32,32,  DIMAGE_NONE);
                }
            }

        }
        else
        {

            unsigned int OFFSET_X_MONSTER = 48*type;


            if (direction==VERTICAL)
            {

                /*
                                  if (true)
                                 {
                                    _lineRGBA( lX+SIZE/2, mini*SIZE+OFFSET_Y+SIZE/2, lX+SIZE/2, maxi*SIZE+OFFSET_Y+SIZE/2, 255, 0, 0, 255 );
                                    _lineRGBA( lX+1+SIZE/2, mini*SIZE+OFFSET_Y+SIZE/2, lX+1+SIZE/2, maxi*SIZE+OFFSET_Y+SIZE/2, 255, 0, 0, 255 );
                                    _lineRGBA( lX-1+SIZE/2, mini*SIZE+OFFSET_Y+SIZE/2, lX-1+SIZE/2, maxi*SIZE+OFFSET_Y+SIZE/2, 255, 0, 0, 255 );
                                 }
                */
                if (sens==BAS && BossCollection[k].ycur<maxi)
                {
                    if (frame_boss==0) dsubimage( lX, lY, &bossmages, OFFSET_X_MONSTER,36,16,18,  DIMAGE_NONE);
                    else if (frame_boss==1) dsubimage( lX, lY+3, &bossmages, OFFSET_X_MONSTER+16,36,16,18,  DIMAGE_NONE);
                    else if (frame_boss==2) dsubimage( lX, lY+5, &bossmages, OFFSET_X_MONSTER+32,36,16,18,  DIMAGE_NONE);
                    else if (frame_boss==3) dsubimage( lX, lY+8, &bossmages, OFFSET_X_MONSTER,36,16,18,  DIMAGE_NONE);
                    else if (frame_boss==4) dsubimage( lX, lY+11, &bossmages, OFFSET_X_MONSTER+16,36,16,18,  DIMAGE_NONE);
                    else if (frame_boss==5) dsubimage( lX, lY+13, &bossmages, OFFSET_X_MONSTER+32,36,16,18,  DIMAGE_NONE);
                }
                else if (sens==BAS && BossCollection[k].ycur==maxi)
                {
                    dsubimage( lX, lY, &bossmages, OFFSET_X_MONSTER,36,16,18,  DIMAGE_NONE);
                }
                else if (sens==HAUT && BossCollection[k].ycur>mini)
                {
                    if (frame_boss==0) dsubimage( lX, lY, &bossmages, OFFSET_X_MONSTER,0,16,18,  DIMAGE_NONE);
                    else if (frame_boss==1) dsubimage( lX, lY-3, &bossmages, OFFSET_X_MONSTER+16,0,16,18,  DIMAGE_NONE);
                    else if (frame_boss==2) dsubimage( lX, lY-5, &bossmages, OFFSET_X_MONSTER+32,0,16,18,  DIMAGE_NONE);
                    else if (frame_boss==3) dsubimage( lX, lY-8, &bossmages, OFFSET_X_MONSTER,0,16,18,  DIMAGE_NONE);
                    else if (frame_boss==4) dsubimage( lX, lY-11, &bossmages, OFFSET_X_MONSTER+16,0,16,18,  DIMAGE_NONE);
                    else if (frame_boss==5) dsubimage( lX, lY-13, &bossmages, OFFSET_X_MONSTER+32,0,16,18,  DIMAGE_NONE);
                }
                else if (sens==HAUT && BossCollection[k].ycur==mini)
                {
                    dsubimage( lX, lY, &bossmages, OFFSET_X_MONSTER,0,16,18,  DIMAGE_NONE);
                }
            }

            else if (direction==HORIZONTAL)
            {

                /*
                                 if (true)
                                 {
                                    _lineRGBA( mini*SIZE+OFFSET_X+SIZE/2, lY+SIZE/2, maxi*SIZE+OFFSET_X+SIZE/2, lY+SIZE/2, 255, 0, 0, 255 );
                                    _lineRGBA( mini*SIZE+OFFSET_X+SIZE/2, lY+1+SIZE/2, maxi*SIZE+OFFSET_X+SIZE/2, lY+1+SIZE/2, 255, 0, 0, 255 );
                                    _lineRGBA( mini*SIZE+OFFSET_X+SIZE/2, lY-1+SIZE/2, maxi*SIZE+OFFSET_X+SIZE/2, lY-1+SIZE/2, 255, 0, 0, 255 );
                                 }
                */

                if (sens==GAUCHE && BossCollection[k].xcur>mini)
                {
                    if (frame_boss==0) dsubimage( lX, lY, &bossmages, OFFSET_X_MONSTER,54,16,18,  DIMAGE_NONE);
                    else if (frame_boss==1) dsubimage( lX-3, lY, &bossmages, OFFSET_X_MONSTER+16,54,16,18,  DIMAGE_NONE);
                    else if (frame_boss==2) dsubimage( lX-5, lY, &bossmages, OFFSET_X_MONSTER+32,54,16,18,  DIMAGE_NONE);
                    else if (frame_boss==3) dsubimage( lX-8, lY, &bossmages, OFFSET_X_MONSTER,54,16,18,  DIMAGE_NONE);
                    else if (frame_boss==4) dsubimage( lX-11, lY, &bossmages, OFFSET_X_MONSTER+16,54,16,18,  DIMAGE_NONE);
                    else if (frame_boss==5) dsubimage( lX-13, lY, &bossmages, OFFSET_X_MONSTER+32,54,16,18,  DIMAGE_NONE);
                }
                else if (sens==GAUCHE && BossCollection[k].xcur==mini)
                {
                    dsubimage( lX, lY, &bossmages, OFFSET_X_MONSTER,54,16,18,  DIMAGE_NONE);
                }
                else if (sens==DROITE && BossCollection[k].xcur<maxi)
                {
                    if (frame_boss==0) dsubimage( lX, lY, &bossmages, OFFSET_X_MONSTER,18,16,18,  DIMAGE_NONE);
                    else if (frame_boss==1) dsubimage( lX+3, lY, &bossmages, OFFSET_X_MONSTER+16,18,16,18,  DIMAGE_NONE);
                    else if (frame_boss==2) dsubimage( lX+5, lY, &bossmages, OFFSET_X_MONSTER+32,18,16,18,  DIMAGE_NONE);
                    else if (frame_boss==3) dsubimage( lX+8, lY, &bossmages, OFFSET_X_MONSTER,18,16,18,  DIMAGE_NONE);
                    else if (frame_boss==4) dsubimage( lX+11, lY, &bossmages, OFFSET_X_MONSTER+16,18,16,18,  DIMAGE_NONE);
                    else if (frame_boss==5) dsubimage( lX+13, lY, &bossmages, OFFSET_X_MONSTER+32,18,16,18,  DIMAGE_NONE);
                }
                else if (sens==DROITE && BossCollection[k].xcur==maxi)
                {
                    dsubimage( lX, lY, &bossmages, OFFSET_X_MONSTER,18,16,18,  DIMAGE_NONE);
                }
            }
        }
    }

    frame_boss++;
    if (frame_boss==6)
    {
        updateBoss();
        frame_boss=0;
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
                            _boxRGBA( X+SIZE/4*(j+1)-1, Y-1, X+SIZE/4*(j+1)+1, Y+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                            _rectangleRGBA( X+SIZE/4*(j+1)-2, Y-2, X+SIZE/4*(j+1)+2, Y+2, 150, 150, 150, 255);
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
                            _boxRGBA( X+SIZE/4*(j+1)-1, Y-1, X+SIZE/4*(j+1)+1, Y+1, tempRecept->R, tempRecept->G, tempRecept->B, 255);
                            _rectangleRGBA( X+SIZE/4*(j+1)-2, Y-2, X+SIZE/4*(j+1)+2, Y+2, 150, 150, 150, 255);
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

        dimage( 0, 10, &bigparch);
        dimage( 88, 42, &maintitle);

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
        if (key==KEY_DOWN && selectOptionPause<2)
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

        dimage( 0, 10, &bigparch);
        dimage( 88, 42, &maintitle);

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

        dfont( &font_fantasy );
        dprint( 90, 155, C_RGB(150,150,150), "Please, Send Me Back to Main Menu");
        dprint( 89, 154, C_BLACK,  "Please, Send Me Back to Main Menu");


        dfont( &font_tiny );

        if (selectOptionPause == 0)
        {
            lX = 70;
            lY = 119;
            GotoMainMenu=false;
        }
        else if (selectOptionPause == 1)
        {
            lX = 70;
            lY = 134;
            GotoMainMenu=false;
        }
        else if (selectOptionPause == 2)
        {
            lX = 70;
            lY = 149;
            GotoMainMenu=true;
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

static int get_inputs_loose(void)
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
            doneLoose = true;
            selectOptionLoose = 1;
        }
        if (key==KEY_UP && selectOptionLoose>0)
        {
            selectOptionLoose--;
        }
        if (key==KEY_DOWN && selectOptionLoose<1)
        {
            selectOptionLoose++;
        }
        if (key==KEY_EXE)
        {
            doneLoose = true;
        }
    }
}

void drawLoose( void )
{
    doneLoose = false;

    unsigned int lX = 0;
    unsigned int lY = 0;

    frame_light = 0;

    while (!doneLoose)
    {
        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);
        dimage( 88, 42, &maintitle);

        dfont( &font_fantasy );
        dprint( 90, 95, C_RGB(150,150,150), "You Loose : Be Careful Of the Monsters" );
        dprint( 89, 94, C_BLACK, "You Loose : Be Careful Of the Monsters" );

        dfont( &font_fantasy );
        dprint( 90, 105, C_RGB(150,150,150), "RetryThis Level ?" );
        dprint( 89, 104, C_BLACK, "RetryThis Level ?" );

        dfont( &font_fantasy );
        dprint( 90, 125, C_RGB(150,150,150), "Yes - I Never Give Up !!!");
        dprint( 89, 124, C_BLACK, "Yes - I Never Give Up !!!");

        dfont( &font_fantasy );
        dprint( 90, 140, C_RGB(150,150,150),  "No - Better If I Quit !!! ");
        dprint( 89, 139, C_BLACK,  "No - Better If I Quit !!! ");

        dfont( &font_tiny );

        if (selectOptionLoose == 0)
        {
            lX = 70;
            lY = 119;
        }
        else if (selectOptionLoose == 1)
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

        get_inputs_loose();

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

        dimage( 0, 10, &bigparch);
        dimage( 88, 42, &maintitle);

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
        dprint( 110, 156, C_RGB(150,150,150), "Welcome to Magic Light %s", Revision);
        dprint( 109, 155, C_BLACK, "Welcome to Magic Light %s", Revision);
        dprint( 110, 166, C_RGB(150,150,150), "                    12/2021 - by SlyVTT                     ");
        dprint( 109, 165, C_BLACK, "                    12/2021 - by SlyVTT                     ");


        flickers++;
        if (flickers==40) flickers=0;

        dupdate();

        get_inputs_title();
    }
}

static int get_inputs_difficultymenu(void)
{
    int opt = GETKEY_DEFAULT & ~GETKEY_REP_ARROWS;
    int timeout = 1;

    while(1)
    {
        key_event_t ev = getkey_opt(opt, &timeout);
        if(ev.type == KEYEV_NONE) return -1;

        int key = ev.key;

        if (key==KEY_EXE)
            doneDifficulty = true;

        if (key==KEY_UP && selectStartMenu>0)
            selectDifficultyMenu--;

        if (key==KEY_DOWN && selectStartMenu<2)
            selectDifficultyMenu++;
    }
}

unsigned char drawDifficultyMenu( void )
{
    doneDifficulty = false;

    unsigned int lX = 0;
    unsigned int lY = 0;

    frame_light = 0;

    while (!doneDifficulty)
    {

        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);
        dimage( 88, 42, &maintitle);

        dfont( &font_fantasy );
        dprint( 90, 100, C_RGB(150,150,150), "Sweet Like a Cherry Cake");
        dprint( 89, 99, C_BLACK, "Sweet Like a Cherry Cake");

        for( unsigned char k = 0; k< 5; k++)
        {
            dsubimage( 250 + k*8, 100, &hearts, 8, 0, 8, 8,  DIMAGE_NONE);
        }
        for( unsigned char k = 5; k< 5; k++)
        {
            dsubimage( 250 + k*8, 100, &hearts, 0, 0, 8, 8,  DIMAGE_NONE);
        }

        dfont( &font_fantasy );
        dprint( 90, 120, C_RGB(150,150,150), "Acid Like an Orange Juice");
        dprint( 89, 119, C_BLACK, "Acid Like an Orange Juice");

        for( unsigned char k = 0; k< 3; k++)
        {
            dsubimage( 250 + k*8, 120, &hearts, 8, 0, 8, 8,  DIMAGE_NONE);
        }
        for( unsigned char k = 3; k< 5; k++)
        {
            dsubimage( 250 + k*8, 120, &hearts, 0, 0, 8, 8,  DIMAGE_NONE);
        }

        dfont( &font_fantasy );
        dprint( 90, 140, C_RGB(150,150,150), "Bitter Like a Dark Beer");
        dprint( 89, 139, C_BLACK, "Bitter Like a Dark Beer");

        for( unsigned char k = 0; k< 1; k++)
        {
            dsubimage( 250 + k*8, 140, &hearts, 8, 0, 8, 8,  DIMAGE_NONE);
        }
        for( unsigned char k = 1; k< 5; k++)
        {
            dsubimage( 250 + k*8, 140, &hearts, 0, 0, 8, 8,  DIMAGE_NONE);
        }


        if (selectDifficultyMenu == 0)
        {
            lX = 70;
            lY = 94;
        }
        else if (selectDifficultyMenu == 1)
        {
            lX = 70;
            lY = 114;
        }
        else if (selectDifficultyMenu == 2)
        {
            lX = 70;
            lY = 134;
        }

        frame_light%=6;
        dsubimage( lX, lY, &light, 16*frame_light,0,16,16,  DIMAGE_NONE);
        frame_light++;


        dfont( &font_tiny );
        dprint( 90, 166, C_RGB(150,150,150), "[UP] [DOWN] : select / [EXE] : validate");
        dprint( 89, 165, C_BLACK,"[UP] [DOWN] : select / [EXE] : validate");

        dupdate();

        get_inputs_difficultymenu();
    }

    return selectDifficultyMenu;
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

        if (saveexist==false && key==KEY_UP && selectStartMenu>1)
            selectStartMenu--;

        if (saveexist==true && key==KEY_UP && selectStartMenu>0)
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

    saveexist = false;

    gint_world_switch( GINT_CALL( is_save_existing ) );

    if (saveexist==false) selectStartMenu = 1;
    else selectStartMenu = 0;

    while (!doneStart)
    {
        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);
        dimage( 88, 42, &maintitle);

        if (saveexist==false)
        {
            dfont( &font_fantasy );
            dprint( 80, 100, C_RGB(150,150,150), "Continue Previous Game");
            dprint( 79, 99, C_RGB(240,240,240), "Continue Previous Game");

            dfont( &font_fantasy );
            dprint( 80, 120, C_RGB(150,150,150), "Start New Game");
            dprint( 79, 119, C_BLACK, "Start New Game");
        }
        else
        {
            dfont( &font_fantasy );
            dprint( 80, 100, C_RGB(150,150,150), "Continue Previous Game");
            dprint( 79, 99, C_BLACK, "Continue Previous Game");

            dfont( &font_fantasy );
            dprint( 80, 120, C_RGB(150,150,150), "Start New Game (Will Trash Previous Save)");
            dprint( 79, 119, C_RED, "Start New Game (Will Trash Previous Save)");
        }


        dfont( &font_fantasy );
        dprint( 80, 140, C_RGB(150,150,150), "Quit And Back To OS");
        dprint( 79, 139, C_BLACK, "Quit And Back To OS");

        if (selectStartMenu == 0)
        {
            lX = 60;
            lY = 94;
        }
        else if (selectStartMenu == 1)
        {
            lX = 60;
            lY = 114;
        }
        else if (selectStartMenu == 2)
        {
            lX = 60;
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



static int get_inputs_story(void)
{
    int opt = GETKEY_DEFAULT & ~GETKEY_REP_ARROWS;
    int timeout = 1;

    while(1)
    {
        key_event_t ev = getkey_opt(opt, &timeout);
        if(ev.type == KEYEV_NONE) return -1;

        int key = ev.key;

        if (key==KEY_EXE || key==KEY_EXIT)
            doneStory = true;

        if (key==KEY_UP && selectStoryMenu>1)
            selectStoryMenu--;

        if (key==KEY_DOWN && selectStoryMenu<2)
            selectStoryMenu++;
    }
}

void drawStoryMenu( void )
{

    doneStory = false;

    selectStoryMenu = 1;

    while (!doneStory)
    {
        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);
        dimage( 63, 41, &village);


        dfont( &font_tiny );
        dprint( 60, 170, C_RGB(150,150,150), "[EXE/EXIT]    >>>");
        dprint( 59, 169, C_BLACK,"[EXE/EXIT]    >>>");

        dupdate();

        get_inputs_story();
    }

    doneStory = false;
    selectStoryMenu = 1;

    while (!doneStory)
    {
        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);

        dfont( &font_fantasy );
        dprint( 70, 55, C_RGB(150,150,150), "A long time ago, in a peaceful and colorful ");
        dprint( 69, 54, C_BLACK, "A long time ago, in a peaceful and colorful");

        dfont( &font_fantasy );
        dprint( 55, 75, C_RGB(150,150,150), "village came a Dark Sorcerer with the terrible");
        dprint( 54, 74, C_BLACK, "village came a Dark Sorcerer with the terrible");

        dfont( &font_fantasy );
        dprint( 55, 95, C_RGB(150,150,150), "ambition of stoling all lights and colors. Making" );
        dprint( 54, 94, C_BLACK, "ambition of stoling all lights and colors. Making " );

        dfont( &font_fantasy );
        dprint( 60, 115, C_RGB(150,150,150), "darkness and fear the only remaining things " );
        dprint( 59, 114, C_BLACK,  "darkness and fear the only remaining things" );

        dfont( &font_fantasy );
        dprint( 60, 135, C_RGB(150,150,150), "here ..." );
        dprint( 59, 134, C_BLACK,  "here ..." );



        dfont( &font_tiny );
        dprint( 70, 166, C_RGB(150,150,150), "[EXE/EXIT]    >>>");
        dprint( 69, 165, C_BLACK,"[EXE/EXIT]    >>>");

        dupdate();

        get_inputs_story();
    }

    doneStory = false;

    selectStoryMenu = 1;

/*    while (!doneStory)
    {
        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);
        dimage( 63, 41, &village);


        dfont( &font_tiny );
        dprint( 60, 170, C_RGB(150,150,150), "[EXE/EXIT]    >>>");
        dprint( 59, 169, C_BLACK,"[EXE/EXIT]    >>>");

        dupdate();

        get_inputs_story();
    }
*/

    int nbstep = 128;

    memcpy( palette, village.data, 512 );
    uint16_t paletteNB[256];
    uint16_t paletteTemp[256];

    uint16_t paletteStart[256][3];
    uint16_t paletteEnd[256][3];
    uint16_t paletteNow[256][3];
    int16_t paletteDelta[256][3];

    for( int k=0; k<255; k++)
    {
        unsigned char B=(unsigned char) ((palette[k] & 0b0000000000011111)<<3);
        unsigned char G=(unsigned char) (((palette[k] & 0b0000011111100000) >> 5)<<2);
        unsigned char R=(unsigned char) (((palette[k] & 0b1111100000000000) >> 11)<<3);

        paletteStart[k][0] = R<<8;
        paletteStart[k][1] = G<<8;
        paletteStart[k][2] = B<<8;

        uint16_t Moy = (0.299*R+0.587*G+0.114*B);

        paletteEnd[k][0] = Moy<<8;
        paletteEnd[k][1] = Moy<<8;
        paletteEnd[k][2] = Moy<<8;

        paletteDelta[k][0] = (paletteEnd[k][0] - paletteStart[k][0])/nbstep;
        paletteDelta[k][1] = (paletteEnd[k][1] - paletteStart[k][1])/nbstep;
        paletteDelta[k][2] = (paletteEnd[k][2] - paletteStart[k][2])/nbstep;

        paletteNB[k] = (uint16_t) (((Moy>>3) << 11) | ((Moy>>2) << 5 ) | (Moy>>3));
    }
    //memcpy( village.data, paletteNB, 512 );


    int framevillage = 0;
    doneStory = false;


    while (!doneStory)
    {
        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);

        if (framevillage<=nbstep)
        {
            for( int k=0; k<255; k++)
            {
                paletteNow[k][0] = (paletteStart[k][0]+framevillage*paletteDelta[k][0])>>8;
                paletteNow[k][1] = (paletteStart[k][1]+framevillage*paletteDelta[k][1])>>8;
                paletteNow[k][2] = (paletteStart[k][2]+framevillage*paletteDelta[k][2])>>8;
                paletteTemp[k] = (uint16_t) (((paletteNow[k][0]>>3) << 11) | ((paletteNow[k][1]>>2) << 5 ) | (paletteNow[k][2]>>3));
            }
            memcpy( village.data, paletteTemp, 512 );
        }
        else
        {
            memcpy( village.data, paletteNB, 512 );
        }

        framevillage++;
        if (framevillage>=nbstep) framevillage=nbstep;

        dimage( 63, 41, &village);


        dfont( &font_tiny );
        dprint( 60, 170, C_RGB(150,150,150), "[EXE/EXIT]    >>>");
        dprint( 59, 169, C_BLACK,"[EXE/EXIT]    >>>");

        dupdate();

        get_inputs_story();
    }

        doneStory = false;
    selectStoryMenu = 1;

    while (!doneStory)
    {
        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);

        dfont( &font_fantasy );
        dprint( 60, 55, C_RGB(150,150,150), "The Sorcerer returned to his Castle" );
        dprint( 59, 54, C_BLACK,  "The Sorcerer returned to his Castle" );

        dfont( &font_fantasy );
        dprint( 60, 75, C_RGB(150,150,150), "and ruled his Daughters to protect his loot..." );
        dprint( 59, 74, C_BLACK,  "and ruled his Daughters to protect his loot..." );

        dfont( &font_fantasy );
        dprint( 65, 95, C_RGB(150,150,150), "Guided only by courage, you entered the" );
        dprint( 64, 94, C_BLACK, "Guided only by courage, you entered the" );

        dfont( &font_fantasy );
        dprint( 65, 115, C_RGB(150,150,150), "castle to defeat all dangers and help the" );
        dprint( 64, 114, C_BLACK,  "castle to defeat all dangers and help the" );

        dfont( &font_fantasy );
        dprint( 65, 135, C_RGB(150,150,150), "villagers ... Good luck my Friend !!!" );
        dprint( 64, 134, C_BLACK,  "villagers ... Good luck my Friend !!!" );


        dfont( &font_tiny );
        dprint( 60, 170, C_RGB(150,150,150), "[EXE/EXIT]    >>>");
        dprint( 59, 169, C_BLACK,"[EXE/EXIT]    >>>");

        dupdate();

        get_inputs_story();
    }

    doneStory = false;

    selectStoryMenu = 1;


    while (!doneStory)
    {
        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);
        dimage( 63, 41, &castletitle);


        dfont( &font_tiny );
        dprint( 60, 170, C_RGB(150,150,150), "[EXE/EXIT]    Play");
        dprint( 59, 169, C_BLACK,"[EXE/EXIT]    Play");

        dupdate();

        get_inputs_story();
    }

    memcpy( village.data, palette, 512 );
}



void updateTreasures( selection what )
{
    if (what==JOUEUR)
    {
        unsigned char X = joueur.x;
        unsigned char Y= joueur.y;

        for (unsigned char k=0; k<NbTreasureLevel; k++)
        {
            if (X==TreasureCollection[k].x && Y==TreasureCollection[k].y)
            {
                if (TreasureCollection[k].isopen==false)
                {
                    score+=TreasureCollection[k].scoreboost;
                    TreasureCollection[k].isopen=true;

                    if (TreasureCollection[k].inside == BLACK_FRAGMENT)  BlackFrag=true;
                    if (TreasureCollection[k].inside == WHITE_FRAGMENT)  WhiteFrag=true;
                    if (TreasureCollection[k].inside == RED_FRAGMENT)  RedFrag=true;
                    if (TreasureCollection[k].inside == GREEN_FRAGMENT)  GreenFrag=true;
                    if (TreasureCollection[k].inside == BLUE_FRAGMENT)  BlueFrag=true;

                    if (TreasureCollection[k].inside == HEART_SINGLE)
                    {
                        if (difficulty==0 && life<5) life++;
                        else if (difficulty==1 && life<3) life++;
                        else if (difficulty==2 && life<1) life++;    // this is just theorical as Life < 1 means that you loose !!
                    }

                    if (TreasureCollection[k].inside == HEART_FULL)
                    {
                        if (difficulty==0) life=5;
                        else if (difficulty==1) life=3;
                        else if (difficulty==2) life=1;    // this is just theorical as Life < 1 means that you loose !!
                    }

                }
                else
                {
                    TreasureCollection[k].isvisible=false;
                }
            }
        }
    }
}


void launch_Boss_Attack( void )
{

    if (frame_boss_attack==0)
    {
        attackboss=true;
        xbossattack=BossCollection[0].xcur*SIZE+OFFSET_X+SIZE/2;
        ybossattack=BossCollection[0].ycur*SIZE+OFFSET_Y+SIZE/2;

        srand( rtc_ticks() );
        attacktype = rand() % 8;
        attackpattern = rand() % 2;

        if (attacktype==0) NbBulletLevel=5;
        else if (attacktype==1) NbBulletLevel=10;
        else if (attacktype==2) NbBulletLevel=20;
        else if (attacktype==3) NbBulletLevel=10;
        else if (attacktype==4) NbBulletLevel=20;
        else if (attacktype==5) NbBulletLevel=10;
        else if (attacktype==6) NbBulletLevel=20;
        else NbBulletLevel=40;

        free( BulletCollection );
        BulletCollection = (Bullet*) malloc( NbBulletLevel * sizeof( Bullet ) );

        for( unsigned char k = 0; k<NbBulletLevel; k++)
        {
            BulletCollection[k].visible = true;
        }

    }

    if ( attackpattern == 0)
    {
        for( unsigned char k=0; k<NbBulletLevel; k++)
        {
            if ( BulletCollection[k].visible == true )
            {
                int xpart = xbossattack + 2*frame_boss_attack*cos( k*360/NbBulletLevel*3.1415927/180.0 );
                int ypart = ybossattack + 2*frame_boss_attack*sin( k*360/NbBulletLevel*3.1415927/180.0 );

                unsigned char cellX = (xpart-OFFSET_X)/SIZE;
                unsigned char cellY = (ypart-OFFSET_Y)/SIZE;

                for (unsigned char j = 0; j<NbObstacleLevel; j++)
                {
                    if (ObstaclesCollection[j].x==cellX && ObstaclesCollection[j].y==cellY && ObstaclesCollection[j].type!=BLOCK_WATER)
                    {
                        BulletCollection[k].visible = false;
                    }
                }
                for (unsigned char j = 0; j<NbBlocksLevel; j++)
                {
                    if (BlocksCollection[j].x==cellX && BlocksCollection[j].y==cellY)
                    {
                        BulletCollection[k].visible = false;
                    }
                }
                if (cellX==joueur.x && cellY==joueur.y)
                {
                    if (life>0) life--;
                    BulletCollection[k].visible = false;
                }

                if (xpart<OFFSET_X+SIZE || xpart>OFFSET_X+SIZE*(SIZE_MAP_X-1) || ypart <OFFSET_Y+SIZE || ypart>OFFSET_Y+SIZE*(SIZE_MAP_Y-1))
                {
                    BulletCollection[k].visible = false;
                }


                if ( BulletCollection[k].visible == true ) dsubimage( xpart-8, ypart-8, &bulletsbicolor, frame_bullet*16,attacktype*16,16,16,  DIMAGE_NONE);
            }

        }
    }
    else if ( attackpattern == 1)
    {
        for( unsigned char k=0; k<NbBulletLevel; k++)
        {
            if ( BulletCollection[k].visible == true )
            {
                int xpart = xbossattack + 2*frame_boss_attack*cos( (frame_boss_attack*2+k*360/NbBulletLevel)*3.1415927/180.0 );
                int ypart = ybossattack + 2*frame_boss_attack*sin( (frame_boss_attack*2+k*360/NbBulletLevel)*3.1415927/180.0 );

                unsigned char cellX = (xpart-OFFSET_X)/SIZE;
                unsigned char cellY = (ypart-OFFSET_Y)/SIZE;

                for (unsigned char j = 0; j<NbObstacleLevel; j++)
                {
                    if (ObstaclesCollection[j].x==cellX && ObstaclesCollection[j].y==cellY && ObstaclesCollection[j].type!=BLOCK_WATER)
                    {
                        BulletCollection[k].visible = false;
                    }
                }
                for (unsigned char j = 0; j<NbBlocksLevel; j++)
                {
                    if (BlocksCollection[j].x==cellX && BlocksCollection[j].y==cellY)
                    {
                        BulletCollection[k].visible = false;
                    }
                }
                if (cellX==joueur.x && cellY==joueur.y)
                {
                    if (life>0) life--;
                    BulletCollection[k].visible = false;
                }

                if (xpart<OFFSET_X+SIZE || xpart>OFFSET_X+SIZE*(SIZE_MAP_X-1) || ypart <OFFSET_Y+SIZE || ypart>OFFSET_Y+SIZE*(SIZE_MAP_Y-1))
                {
                    BulletCollection[k].visible = false;
                }


                if ( BulletCollection[k].visible == true ) dsubimage( xpart-8, ypart-8, &bulletsbicolor, frame_bullet*16,attacktype*16,16,16,  DIMAGE_NONE);
            }

        }
    }




    frame_bullet++;
    if (frame_bullet==8) frame_bullet=0;

    frame_boss_attack++;
    if (frame_boss_attack==100)
    {
        frame_boss_attack=0;
        attackboss=false;
        free( BulletCollection );
        BulletCollection=NULL;
    }
}

static int get_inputs(void)
{
    int opt = GETKEY_DEFAULT & ~GETKEY_REP_ARROWS & ~GETKEY_MOD_SHIFT & ~GETKEY_MOD_ALPHA;
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

        if (key==KEY_SHIFT)
        {
            updateTreasures( selected );
        }

#if DEBUG==1
        if (key==KEY_1)
        {
            if (currentLevel>0)
                loadLevel(currentLevel-1);
        }

        if (key==KEY_7)
        {
            if (currentLevel<NB_LEVELS)
                loadLevel(currentLevel+1);
        }

        if (key==KEY_9)
        {
            BlackFrag=true;
            WhiteFrag=true;
            RedFrag=true;
            GreenFrag=true;
            BlueFrag=true;
        }

        if (key==KEY_3)
        {
            BlackFrag=false;
            WhiteFrag=false;
            RedFrag=false;
            GreenFrag=false;
            BlueFrag=false;
        }

        if (key==KEY_8 && NbBossLevel!=0)
        {
            attackboss = true;
        }
#endif

    }
}



static int get_inputs_end(void)
{
    int opt = GETKEY_DEFAULT & ~GETKEY_REP_ARROWS;
    int timeout = 1;

    while(1)
    {
        key_event_t ev = getkey_opt(opt, &timeout);
        if(ev.type == KEYEV_NONE) return -1;

        int key = ev.key;

        if (key==KEY_EXE || key==KEY_EXIT)
        {
            doneEnd = true;
            GotoMainMenu = true ;
        }

        /*
                      if (key==KEY_UP && selectEndMenu>1)
                             selectEndMenu--;

                      if (key==KEY_DOWN && selectEndMenu<2)
                             selectEndMenu++;
        */

    }
}

void drawEndMenu( void )
{
    doneEnd = false;

    selectEndMenu = 0;

    unsigned int positionGuy = 75;
    int dir = 1;

    frame = 0;

    while (!doneEnd)
    {
        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);

        //dfont( &font_fantasy );
        //dprint( 70, 45, C_RGB(150,150,150), "The END ... ");
        //dprint( 69, 44, C_BLACK, "The END ...");


        dfont( &font_fantasy );
        dprint( 60, 57, C_RGB(150,150,150), "Congratulation my Dear Friend !!!");
        dprint( 59, 56, C_BLACK, "Congratulation my Dear Friend !!!");

        dfont( &font_fantasy );
        dprint( 55, 69, C_RGB(150,150,150), " " );
        dprint( 54, 68, C_BLACK, " " );

        dfont( &font_fantasy );
        dprint( 60, 81, C_RGB(150,150,150), "You defeated the Sorcerer and you brought" );
        dprint( 59, 80, C_BLACK,  "You defeated the Sorcerer and you brought" );

        dfont( &font_fantasy );
        dprint( 60, 93, C_RGB(150,150,150), "back the Jewel of Colors to the village. " );
        dprint( 59, 92, C_BLACK,  "back the Jewel of Colors to the village. " );

        dfont( &font_fantasy );
        dprint( 60, 105, C_RGB(150,150,150), "You are now a Hero !!!" );
        dprint( 59, 104, C_BLACK,  "You are now a Hero !!!" );

        dfont( &font_fantasy );
        dprint( 65, 117, C_RGB(150,150,150), " " );
        dprint( 64, 116, C_BLACK, " " );

        dfont( &font_fantasy );
        dprint( 65, 129, C_RGB(150,150,150), " " );
        dprint( 64, 128, C_BLACK,  "" );

        dfont( &font_fantasy );
        dprint( 65, 141, C_RGB(150,150,150), "The End ... At least for Now !!" );
        dprint( 64, 140, C_BLACK,  "The End ... At least for Now !!" );

        if (dir==1) // the guy is going to the right
        {

            if (positionGuy>=285) dir=-1;

            if (frame==0) dsubimage( positionGuy, 117, &player, 0,16,16,16,  DIMAGE_NONE);
            else if (frame==1) dsubimage( positionGuy, 117, &player, 16,16,16,16,  DIMAGE_NONE);
            else if (frame==2) dsubimage( positionGuy, 117, &player, 32,16,16,16,  DIMAGE_NONE);

            frame++;
            positionGuy+=1;
            if  (frame==3) frame=0;
        }
        else if (dir==-1) // the guy is going to the left
        {
            if (positionGuy<=75) dir=1;

            if (frame==0) dsubimage( positionGuy, 117, &player, 0,48,16,16,  DIMAGE_NONE);
            else if (frame==1) dsubimage( positionGuy, 117, &player, 16,48,16,16,  DIMAGE_NONE);
            else if (frame==2) dsubimage( positionGuy, 117, &player, 32,48,16,16,  DIMAGE_NONE);

            frame++;
            positionGuy-=1;
            if  (frame==3) frame=0;
        }


        dfont( &font_tiny );
        dprint( 60, 170, C_RGB(150,150,150), "[EXE/EXIT]    >>>");
        dprint( 59, 169, C_BLACK,"[EXE/EXIT]    >>>");

        dupdate();

        get_inputs_end();
    }


    doneEnd = false;
    int framevillage = 0;

    int nbstep = 128;

    memcpy( palette, village.data, 512 );
    uint16_t paletteNB[256];
    uint16_t paletteTemp[256];

    uint16_t paletteStart[256][3];
    uint16_t paletteEnd[256][3];
    uint16_t paletteNow[256][3];
    int16_t paletteDelta[256][3];

    for( int k=0; k<255; k++)
    {
        unsigned char B=(unsigned char) ((palette[k] & 0b0000000000011111)<<3);
        unsigned char G=(unsigned char) (((palette[k] & 0b0000011111100000) >> 5)<<2);
        unsigned char R=(unsigned char) (((palette[k] & 0b1111100000000000) >> 11)<<3);

        paletteEnd[k][0] = R<<8;
        paletteEnd[k][1] = G<<8;
        paletteEnd[k][2] = B<<8;

        uint16_t Moy = (0.299*R+0.587*G+0.114*B);

        paletteStart[k][0] = Moy<<8;
        paletteStart[k][1] = Moy<<8;
        paletteStart[k][2] = Moy<<8;

        paletteDelta[k][0] = (paletteEnd[k][0] - paletteStart[k][0])/nbstep;
        paletteDelta[k][1] = (paletteEnd[k][1] - paletteStart[k][1])/nbstep;
        paletteDelta[k][2] = (paletteEnd[k][2] - paletteStart[k][2])/nbstep;

        paletteNB[k]= (uint16_t) (((Moy>>3) << 11) | ((Moy>>2) << 5 ) | (Moy>>3));
    }
    memcpy( village.data, paletteNB, 512 );


    doneEnd = false;

    while (!doneEnd)
    {
        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);

        if (framevillage<=nbstep)
        {
            for( int k=0; k<255; k++)
            {
                paletteNow[k][0] = (paletteStart[k][0]+framevillage*paletteDelta[k][0])>>8;
                paletteNow[k][1] = (paletteStart[k][1]+framevillage*paletteDelta[k][1])>>8;
                paletteNow[k][2] = (paletteStart[k][2]+framevillage*paletteDelta[k][2])>>8;
                paletteTemp[k] = (uint16_t) (((paletteNow[k][0]>>3) << 11) | ((paletteNow[k][1]>>2) << 5 ) | (paletteNow[k][2]>>3));
            }
            memcpy( village.data, paletteTemp, 512 );
        }
        else
        {
            memcpy( village.data, paletteNB, 512 );
        }

        framevillage++;
        if (framevillage>=nbstep) framevillage=nbstep;

        dimage( 63, 41, &village);


        dfont( &font_tiny );
        dprint( 60, 170, C_RGB(150,150,150), "[EXE/EXIT]    >>>");
        dprint( 59, 169, C_BLACK,"[EXE/EXIT]    >>>");

        dupdate();

        get_inputs_end();
    }


    doneEnd = false;

    while (!doneEnd)
    {
        dclear(C_RGB(0,0,0));

        dimage( 0, 10, &bigparch);
        dimage( 88, 42, &maintitle);
        dimage( 108, 105, &theend);

        dfont( &font_tiny );
        dprint( 118, 170, C_RGB(150,150,150), "[EXE/EXIT]   Back to Main Menu");
        dprint( 117, 169, C_BLACK,"[EXE/EXIT]    Back to Main Menu");

        dupdate();

        get_inputs_end();
    }

}




void the_end( void )
{
    drawEndMenu( );
}


int main ( int argc, char** argv )
{

    do
    {

        selectStartMenu = 0;
        selectDifficultyMenu = 0;
        selectOptionMenu = 0;
        selectOptionPause = 0;
        selectOptionLoose = 0;
        selectStoryMenu = 0;
        selectEndMenu = 0;
        difficulty = 0;

        doneGame = false;
        doneMain = false;
        doneOption = false;
        donePause = false;
        doneStart = false;
        doneTitle = false;
        doneStory = false;
        doneEnd = false;
        GotoMainMenu=false;

        drawTitle();

        unsigned char choice = drawStartMenu();

        if (choice==0)
        {
            initMap();
            loadprogress();

            synchroniseGame( );
            loadLevel( currentLevel );
            adjustLevel( currentLevel );

        }
        else if (choice==1)
        {
            // We start a new game, so we load Level #000
            difficulty= drawDifficultyMenu();

            BlackFrag = false;
            WhiteFrag = false;
            RedFrag = false;
            GreenFrag = false;
            BlueFrag = false;
            score = 0;

            if (difficulty==0) life=5;
            else  if (difficulty==1) life=3;
            else if (difficulty==2) life=1;

            drawStoryMenu();

            initMap();

            loadLevel( 0 );
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

            renderMonsters(  );
            renderBoss( );

            if (attackboss==false && NbBossLevel!=0 && frame_boss_attack == 0 )
            {
                srand( rtc_ticks() );
                unsigned char randomattack = rand() % 101;

                if (randomattack>=45 && randomattack<=65) launch_Boss_Attack();
            }
            if (attackboss==true) launch_Boss_Attack();


            renderPlayer(  );

            dupdate();

            get_inputs();


            updateCurrentState( );


            if (life==0) drawLoose();

            if (selectOptionPause==1 || selectOptionPause==2 || selectOptionLoose==1) doneGame=true;
            if (life==0 && selectOptionLoose==0)
            {
                if (difficulty==0) life=5;
                else  if (difficulty==1) life=3;
                else if (difficulty==2) life=1;

                loadLevel( currentLevel);
            }

            if (WhiteFrag && BlackFrag && RedFrag && GreenFrag && BlueFrag )
            {
                the_end();
                doneGame=true;
                gint_world_switch( GINT_CALL( delete_save )  );
                initMap();
            }



        }

        //updateCurrentState( );

        if (doneEnd==false) gint_world_switch( GINT_CALL( saveprogress )  );

    }
    while (GotoMainMenu);

    exitAndFree( );

    return 0;
}
