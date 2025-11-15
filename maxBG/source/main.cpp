#include "tonc.h"
#include "bg_consts.hpp"

#include "starting_location.h"

#define BG_NUM 2
#define CBB 2
#define SBB 20

#define MOVE_SPEED 2

//update x and y pos and update VOFS and HOFS registers
void updateCoords(int* x, int* y, int vx, int vy){
    *x = clamp(*x + (vx * MOVE_SPEED), 0, 512-240);
    *y = clamp(*y + (vy * MOVE_SPEED), 0, 512-160);

    BGHOFS[BG_NUM][0] = *x;
    BGVOFS[BG_NUM][0] = *y;
}

//decide if new maps need to be drawn
void drawBG(){
    //bad coding practice :(
    SCREENBLOCK* maps = (SCREENBLOCK*)starting_locationMap;

    //draw the 4 bgs in the right position
    for(int i = 0; i < 4; i++){
        SE* se = (SE*)se_mem[SBB+i];
        for(int j = 0; j < 32*32; j++){
            se[j] = maps[i][j];
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
    BGCNT[BG_NUM][0] = BG_BUILD(2, 20, 3, 0, 0, 0, 1);
    REG_DISPCNT = DCNT_MODE0 | BGDCNT[BG_NUM];

    //set initial variables
    int x = 0, y = 0;
    drawBG();

    while(1){
        VBlankIntrWait();

        if(key_is_down(KEY_DIR)){   //if player is inputting direction...
            updateCoords(&x, &y, key_tri_horz(), key_tri_vert());
        }

        key_poll();
    }
}