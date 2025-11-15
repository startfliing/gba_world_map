#include "bg_consts.hpp"

vu16* BGCNT[4] = {
    &REG_BG0CNT,
    &REG_BG1CNT,
    &REG_BG2CNT,
    &REG_BG3CNT
};

vu16* BGHOFS[4] = {
    &REG_BG0HOFS,
    &REG_BG1HOFS,
    &REG_BG2HOFS,
    &REG_BG3HOFS
};

vu16* BGVOFS[4] = {
    &REG_BG0VOFS,
    &REG_BG1VOFS,
    &REG_BG2VOFS,
    &REG_BG3VOFS
};

u16 BGDCNT[4] = {
    DCNT_BG0,
    DCNT_BG1,
    DCNT_BG2,
    DCNT_BG3
};