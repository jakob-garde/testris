#ifndef __TESTRIS_H__
#define __TESTRIS_H__


#include "../lib/jg_baselayer.h"
#include "../../cbui/cbui_includes.h"


enum BlockType {
    BT_UNINITIALIZED,

    BT_LONG,
    BT_STEP,
    BT_TEE,
    BT_BOX,
    BT_ELL,
 
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
    f32 animate;
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


bool BlockGridCollides(Block block) {
    for (s32 y = 0; y < 4; ++y) {
        for (s32 x = 0; x < 4; ++x) {

            if (block.data[y][x]) {
                s32 yy = y + block.grid_y;
                s32 xx = x + block.grid_x;

                bool in_range = yy >= 0 && yy < grid->grid_h && xx >= 0 && x < grid->grid_w;
                bool collides = !in_range || grid->GetBlock(yy, xx)->solid;

                if (collides) {
                    return true;
                }
            }
        }
    }

    return false;
}

Block BlockRotate(Block b) {
    Block r = b;

    if (b.tpe == BT_LONG) {
        r.data[0][0] = b.data[3][0];
        r.data[0][1] = b.data[2][0];
        r.data[0][2] = b.data[1][0];
        r.data[0][3] = b.data[0][0];

        r.data[1][0] = b.data[3][1];
        r.data[1][1] = b.data[2][1];
        r.data[1][2] = b.data[1][1];
        r.data[1][3] = b.data[0][1];

        r.data[2][0] = b.data[3][2];
        r.data[2][1] = b.data[2][2];
        r.data[2][2] = b.data[1][2];
        r.data[2][3] = b.data[0][2];

        r.data[3][0] = b.data[3][3];
        r.data[3][1] = b.data[2][3];
        r.data[3][2] = b.data[1][3];
        r.data[3][3] = b.data[0][3];
    }
    else if (b.tpe == BT_BOX) {
        // we don't need to rotate it
    }
    else {
        r.data[0][0] = b.data[2][0];
        r.data[0][1] = b.data[1][0];
        r.data[0][2] = b.data[0][0];

        r.data[1][0] = b.data[2][1];
        r.data[1][1] = b.data[1][1];
        r.data[1][2] = b.data[0][1];

        r.data[2][0] = b.data[2][2];
        r.data[2][1] = b.data[1][2];
        r.data[2][2] = b.data[0][2];

        r.data[3][0] = b.data[2][3];
        r.data[3][1] = b.data[1][3];
        r.data[3][2] = b.data[0][3];
    }

    return r;
}

Block BlockMirrorX(Block b) {
    Block m = b;
    // only mirror the first 3 columns
    for (s32 y = 0; y < 4; ++y) {
        m.data[y][0] = b.data[y][2];
        m.data[y][2] = b.data[y][0];
    }
    return m;
}

void BlockRotateIfAble() {
    Block rot = BlockRotate(grid->falling);
    if (BlockGridCollides(rot) == false) {
        grid->falling = rot;
    }
}

void BlockLeftIfAble() {
    Block left = grid->falling;
    left.grid_x -= 1;
    if (BlockGridCollides(left) == false) {
        grid->falling = left;
    }
}

void BlockRightIfAble() {
    Block right = grid->falling;
    right.grid_x += 1;
    if (BlockGridCollides(right) == false) {
        grid->falling = right;
    }
}

bool BlockCanFall(Block block) {
    bool can_fall = true;

    for (s32 y = 0; y < 4; ++y) {
        for (s32 x = 0; x < 4; ++x) {
            bool is_block = block.data[y][x];
            s32 yy = y + block.grid_y;
            s32 xx = x + block.grid_x;

            if (is_block) {
                if (yy >= 0 && yy < grid->grid_h && xx >= 0 && x < grid->grid_w) {


                    // TODO: refactor using the BlockCollides() function


                    GridSlot *slot = grid->GetBlock(yy + 1, xx);
                    if (slot->solid) {
                        can_fall = false;
                    }
                }
                else {
                    can_fall = false;
                }
            }
        }
    }

    return can_fall;
}

void BlockFall() {

    bool can_fall = BlockCanFall(grid->falling);
    if (can_fall) {
        grid->falling.grid_y += 1;
    }
    else {
        GridSlot slot = {};
        slot.color = grid->falling.color;
        slot.solid = true;

        // solidify as grid slots
        for (s32 y = 0; y < 4; ++y) {
            for (s32 x = 0; x < 4; ++x) {
                if (grid->falling.data[y][x]) {
                    s32 yy = y + grid->falling.grid_y;
                    s32 xx = x + grid->falling.grid_x;
                    grid->SetBlock(yy, xx, slot);
                }
            }
        }
        grid->falling = {};
    }
}

void BlockSpawn() {

    s32 color_selector = RandMinMaxI(0, 3);
    Color blocks_color;
    switch (color_selector) {
    case 0: blocks_color = COLOR_RED; break;
    case 1: blocks_color = COLOR_GREEN; break;
    case 2: blocks_color = COLOR_YELLOW; break;
    case 3: blocks_color = COLOR_BLUE; break;
    default: assert(1 == 0 && "switch default"); break; }

    Block block = {};
    block.tpe = (BlockType) RandMinMaxI(1, 5);
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
        block.data[1][1] = true;
        block.data[2][0] = true;
        block.data[2][1] = true;
        block.data[2][2] = true;
    } break;
    case BT_BOX: {
        block.data[1][1] = true;
        block.data[1][2] = true;
        block.data[2][1] = true;
        block.data[2][2] = true;
    } break;
    case BT_ELL: {
        block.data[0][0] = true;
        block.data[1][0] = true;
        block.data[2][0] = true;
        block.data[2][1] = true;
    } break;
    default: assert(1 == 0 && "switch default"); break; }

    printf("falling type: %d\n", block.tpe);

    // randomly mirror the blocks (one of two mirrors)
    if (RandMinMaxI(0, 1) == 1) {
        block = BlockMirrorX(block);
    }

    // randomly rotate the blocks (one of four rotations)
    s32 rotations = RandMinMaxI(0, 3);
    for (s32 i = 0; i < rotations; ++i) {
        block = BlockRotate(block);
    }

    grid->falling = block;
}

void GridLineEliminateStartAnimation() {

    for (s32 y = grid->grid_h - 1; y >= 0; --y) {
        bool row_full = true;

        // detect empty row
        for (s32 x = 0; x < grid->grid_w; ++x) {
            row_full = row_full && grid->GetBlock(y, x)->solid;
        }

        // eliminate empty row
        if (row_full) {
            for (s32 x = 0; x < grid->grid_w; ++x) {
                GridSlot *b = grid->GetBlock(y, x);
                if (b->animate == 0) {
                    b->animate = 1;
                }
            }
        }
    }
}

void GridLineEliminate(s32 line) {

    for (s32 y = line - 1; y >= 0; --y) {
        for (s32 x = 0; x < grid->grid_w; ++x) {

            GridSlot *b = grid->GetBlock(y, x);

            if (b->solid == true) {
                grid->SetBlock(y + 1, x, *b);
                grid->ClearBlock(y, x);
            }

        }
    }
}


#endif
