#ifndef __TESTRIS_H__
#define __TESTRIS_H__


#include "../lib/jg_baselayer.h"
#include "../../cbui/cbui_includes.h"


enum BlockType {
    BT_LONG,
    BT_STEP,
    BT_TEE,
    BT_BOX,

    BT_CNT
};

struct Block {
    BlockType tpe;
    Color color;

    bool data[4][4]; // 4x4 indices 0..3 x 0..3
    s32 grid_y;
    s32 grid_x;
};


struct GridSlot {
    Color color;
    bool solid;
    bool falling;
};

static GridSlot g_block_null; // ZII default return value

struct Grid {
    s32 grid_w = 10;
    s32 grid_h = 24;
    GridSlot data[24][10];

    Block falling;

    GridSlot *GetBlock(s32 y, s32 x) {
        if (y >= 0 && y < grid_h && x >= 0 && x < grid_w) {
            return &data[y][x];
        }
        else {
            g_block_null = {};
            g_block_null.solid = true;
            return &g_block_null;
        }
    }

    void SetBlock(s32 y, s32 x, GridSlot b) {
        if (y >= 0 && y < grid_h && x >= 0 && x < grid_w) {
            data[y][x] = b;
        }
        else {
            assert(1 == 0 && "SetBlock: out of scope");
        }
    }

    void ClearBlock(s32 y, s32 x) {
        if (y >= 0 && y < grid_h && x >= 0 && x < grid_w) {
            data[y][x] = {};
        }
        else {
            assert(1 == 0 && "SetBlock: out of scope");
        }
    }
};

static Grid _grid;
static Grid *grid;

enum TestrisMode {
    TM_TITLE,
    TM_MAIN,
    TM_HELPS,

    TM_CNT
};

struct Testris {
    TestrisMode mode;
    TestrisMode mode_prev;
    f32 main_timeout;

    void SetMode(TestrisMode mode_new) {
        mode_prev = mode;
        mode = mode_new;
    }

    void SetModePrev() {
        TestrisMode swap = mode_prev;
        mode_prev = mode;
        mode = swap;
    }

    void ToggleMode(TestrisMode mode_toggle) {
        if (mode == mode_toggle) {
            SetModePrev();
        }
        else {
            SetMode(mode_toggle);
        }
    }
};

void Update() {

    // TODO: fall the block
    // TODO: check for collisions with the grid

    if (true) {
        grid->falling.grid_y += 1;
    }

    // TODO: eliminate line
    // TODO: fall the blocks above once elimination animation is done
}

void FallGridCells() {
    for (s32 y = grid->grid_h - 1; y >= 0; --y) {
        for (s32 x = 0; x < grid->grid_w; ++x) {

            GridSlot *b = grid->GetBlock(y, x);
            if (b->solid == true && b->falling == true) {

                GridSlot *below = grid->GetBlock(y + 1, x);
                if (below->solid == false && y < (grid->grid_h - 1)) {

                    grid->SetBlock(y + 1, x, *b);
                    grid->ClearBlock(y, x);
                }
                else if (below->solid == true) {
                    b->falling = false;
                }
            }
        }
    }
}

void SpawnFallingBlock() {

    s32 color_selector = RandMinMaxI(0, 3);
    Color blocks_color;
    switch (color_selector) {
    case 0: blocks_color = COLOR_RED; break;
    case 1: blocks_color = COLOR_GREEN; break;
    case 2: blocks_color = COLOR_YELLOW; break;
    case 3: blocks_color = COLOR_BLUE; break;
    default: assert(1 == 0 && "switch default"); break; }

    Block block = {};
    block.tpe = (BlockType) RandMinMaxI(0, 3);
    block.grid_y = 0;
    block.grid_x = 3;
    block.color = blocks_color;

    switch (block.tpe) {
    case BT_LONG: {
        block.data[0][1] = true;
        block.data[1][1] = true;
        block.data[2][1] = true;
        block.data[3][1] = true;
    } break;
    case BT_STEP: {
        block.data[1][0] = true;
        block.data[1][1] = true;
        block.data[2][1] = true;
        block.data[2][2] = true;
    } break;
    case BT_TEE: {
        block.data[0][1] = true;
        block.data[1][1] = true;
        block.data[2][1] = true;
        block.data[3][1] = true;
        block.data[3][2] = true;
    } break;
    case BT_BOX: {
        block.data[1][1] = true;
        block.data[1][2] = true;
        block.data[2][1] = true;
        block.data[2][2] = true;
    } break;
    default: assert(1 == 0 && "switch default"); break; }

    printf("falling type: %d\n", block.tpe);

    // TODO: randomly mirror the blocks (one of two mirrors)
    // TODO: randomly rotate the blocks (one of four rotations)

    grid->falling = block;
}


#endif
