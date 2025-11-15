#include "tonc.h"
#include "bg_consts.hpp"

#include "starting_location.h"

#define WIDTH 10
#define HEIGHT 5

#define BG_NUM 2
#define CBB 2
#define SBB 20

#define MOVE_SPEED 2

//update x and y pos and update VOFS and HOFS registers
void updateCoords(int* x, int* y, int vx, int vy){
    int tempX = *x + (vx * MOVE_SPEED);
    int tempY = *y + (vy * MOVE_SPEED);

    *y = clamp(tempY, 0, (256*HEIGHT) - 160);
    *x = clamp(tempX, 0, (256*WIDTH) - 240);

    BGHOFS[BG_NUM][0] = *x & 511;
    BGVOFS[BG_NUM][0] = *y & 511;
}

//decide if new maps need to be drawn
void updateBG(int x, int y, int* currMapInd){
    //which map to load
    //0x7FFF == MAX s16 val
    s16 mapX = clamp((x-8)>>8, 0, 0x7FFF);
    s16 mapY = clamp((y-8)>>8, 0, 0x7FFF);
    int mapInd = clamp((mapY * WIDTH) + mapX, 0, HEIGHT*WIDTH);

    //save some computation if player hasn't moved far enough
    if(*currMapInd == mapInd)return;
    *currMapInd = mapInd;

    //which SE the map is currently viewing 0-3
    u16 dseX = mapX & 1;
    u16 dseY = mapY & 1;
    u16 dseInd = (dseY * 2) + dseX;

    //bad coding practice :(
    SCREENBLOCK* maps = (SCREENBLOCK*)starting_locationMap;
    
    //pick accurate bg's for all 4 squares
    SE* newMaps[4];
    switch(dseInd){
        case 0:
            newMaps[0] = (SE*)(maps[mapInd]);
            newMaps[1] = (SE*)(maps[mapInd + 1]);
            newMaps[2] = (SE*)(maps[mapInd + WIDTH]);
            newMaps[3] = (SE*)(maps[mapInd + WIDTH + 1]);
            break;
        case 1:
            newMaps[0] = (SE*)(maps[mapInd + 1]);
            newMaps[1] = (SE*)(maps[mapInd]);
            newMaps[2] = (SE*)(maps[mapInd + WIDTH + 1]);
            newMaps[3] = (SE*)(maps[mapInd + WIDTH]);
            break;
        case 2:
            newMaps[0] = (SE*)(maps[mapInd + WIDTH]);
            newMaps[1] = (SE*)(maps[mapInd + WIDTH + 1]);
            newMaps[2] = (SE*)(maps[mapInd]);
            newMaps[3] = (SE*)(maps[mapInd + 1]);
            break;
        case 3:
            newMaps[0] = (SE*)(maps[mapInd + WIDTH + 1]);
            newMaps[1] = (SE*)(maps[mapInd + WIDTH]);
            newMaps[2] = (SE*)(maps[mapInd + 1]);
            newMaps[3] = (SE*)(maps[mapInd]);
            break;
    }

    //draw the 4 bgs in the right position
    for(int i = 0; i < 4; i++){
        SE* se = (SE*)se_mem[SBB+i];
        for(int j = 0; j < 32*32; j++){
            se[j] = newMaps[i][j];
        }
    }
};

int main(){
    irq_init(0);
	irq_add(II_VBLANK, 0);

    //load palette and tiles, not map :(
    LZ77UnCompVram(starting_locationPal, pal_bg_mem);
    LZ77UnCompVram(starting_locationTiles, tile_mem[CBB]);

    //set registers
    BGCNT[BG_NUM][0] = BG_BUILD(2, 20, 3, 0, 0, 0, 0);
    REG_DISPCNT = DCNT_MODE0 | BGDCNT[BG_NUM];

    //set initial variables
    int x = 0, y = 0, currMapInd = -1;
    updateBG(x, y, &currMapInd);

    while(1){
        VBlankIntrWait();

        if(key_is_down(KEY_DIR)){   //if player is inputting direction...
            updateCoords(&x, &y, key_tri_horz(), key_tri_vert());
            updateBG(x, y, &currMapInd);
        }

        key_poll();
    }
}