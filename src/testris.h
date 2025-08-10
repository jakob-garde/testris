#ifndef __TESTRIS_H__
#define __TESTRIS_H__


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
    f32 blinking;
};

static GridSlot g_block_null; // ZII default return value

struct Grid {
    s32 grid_w = 10;
    s32 grid_h = 24;
    GridSlot data[24][10];

    Block falling;
    Block next;
    bool pause_falling;

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
    TM_GAMEOVER,

    TM_CNT
};

struct Testris {
    TestrisMode mode;
    TestrisMode mode_prev;
    f32 t_fall;
    f32 t_fall_interval;
    f32 t_animate_interval = 100;
    f32 t_mode_start;

    void SetMode(TestrisMode mode_new, f32 t_start) {
        t_mode_start = t_start;
        mode_prev = mode;
        mode = mode_new;
    }
};


static Testris _g_testris_state;
static Testris *testris;


f32 TimeSinceModeStart_ms() {
    f32 t_delta_ms = (cbui->t_framestart - testris->t_mode_start) / 1000;
    return t_delta_ms;
}

void UpdateGridState() {

    for (s32 y = 0; y < grid->grid_h; ++y) {
        bool line_eliminated = true;

        for (s32 x = 0; x < grid->grid_w; ++x) {
            GridSlot *b = grid->GetBlock(y, x);
            line_eliminated = line_eliminated && b->blinking > testris->t_animate_interval * 3;

            if (b->blinking > 0 && b->blinking < testris->t_animate_interval) {
                b->color = COLOR_BLACK;
            }
            else if (b->blinking > 0 && b->blinking < testris->t_animate_interval * 2) {
                b->color = COLOR_WHITE;
            }
            else if (b->blinking > 0 && b->blinking < testris->t_animate_interval * 3) {
                b->color = COLOR_BLACK;
            }
            else if (b->blinking > 0.6f) {
                b->solid = false;
                b->blinking = 0;
            }

            if (b->blinking > 0) {
                b->blinking += cbui->dt;
            }
        }

        if (line_eliminated) {
            for (s32 z = y - 1; z >= 0; --z) {
                for (s32 x = 0; x < grid->grid_w; ++x) {
                    GridSlot *b = grid->GetBlock(z, x);

                    grid->SetBlock(z + 1, x, *b);
                    grid->ClearBlock(z, x);
                }
            }

            grid->pause_falling = false;
        }
    }
}

bool BlockCollides(Block block) {
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
    if (BlockCollides(rot) == false) {
        grid->falling = rot;
    }
}

void BlockLeftIfAble() {
    Block left = grid->falling;
    left.grid_x -= 1;
    if (BlockCollides(left) == false) {
        grid->falling = left;
    }
}

void BlockRightIfAble() {
    Block right = grid->falling;
    right.grid_x += 1;
    if (BlockCollides(right) == false) {
        grid->falling = right;
    }
}

Block BlockCreate() {

    s32 color_selector = RandMinMaxI(0, 3);
    Color blocks_color;
    switch (color_selector) {
    case 0: blocks_color = COLOR_RED; break;
    case 1: blocks_color = COLOR_GREEN; break;
    case 2: blocks_color = COLOR_YELLOW2; break;
    case 3: blocks_color = COLOR_BLUE; break;
    default: assert(1 == 0 && "switch default"); break; }

    Block block = {};
    block.tpe = (BlockType) RandMinMaxI(1, 5);
    block.grid_y = 1;
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
        block.data[0][0] = true;
        block.data[0][1] = true;
        block.data[1][1] = true;
        block.data[1][2] = true;
    } break;
    case BT_TEE: {
        block.data[0][1] = true;
        block.data[1][0] = true;
        block.data[1][1] = true;
        block.data[1][2] = true;
    } break;
    case BT_BOX: {
        block.data[0][0] = true;
        block.data[0][1] = true;
        block.data[1][0] = true;
        block.data[1][1] = true;
    } break;
    case BT_ELL: {
        block.data[0][0] = true;
        block.data[1][0] = true;
        block.data[2][0] = true;
        block.data[2][1] = true;
    } break;
    default: assert(1 == 0 && "switch default"); break; }

    // randomly mirror the blocks (one of two mirrors)
    if (RandMinMaxI(0, 1) == 1) {
        block = BlockMirrorX(block);
    }

    // randomly rotate the blocks (one of four rotations)
    s32 rotations = RandMinMaxI(0, 3);
    for (s32 i = 0; i < rotations; ++i) {
        block = BlockRotate(block);
    }

    return block;
}

bool BlockFallOrFreeze() {
    Block test = grid->falling;
    test.grid_y += 1;
    bool can_fall = ! BlockCollides(test);

    if (can_fall) {
        grid->falling.grid_y += 1;
    }

    else {
        GridSlot slot = {};
        slot.color = grid->falling.color;
        slot.solid = true;

        // solidify into the grid
        for (s32 y = 0; y < 4; ++y) {
            for (s32 x = 0; x < 4; ++x) {
                if (grid->falling.data[y][x]) {
                    s32 yy = y + grid->falling.grid_y;
                    s32 xx = x + grid->falling.grid_x;

                    grid->SetBlock(yy, xx, slot);
                }
            }
        }

        if (grid->falling.grid_y < 4) {
            testris->SetMode(TM_GAMEOVER, cbui->t_framestart);
        }

        // spawn
        if (grid->next.tpe == BT_UNINITIALIZED) {
            grid->falling = BlockCreate();
        }
        else {
            grid->falling = grid->next;
        }

        grid->falling = grid->next;
        grid->next = BlockCreate();
    }

    // find full rows
    for (s32 y = grid->grid_h - 1; y >= 0; --y) {
        bool row_full = true;
        for (s32 x = 0; x < grid->grid_w; ++x) {
            row_full = row_full && grid->GetBlock(y, x)->solid;
        }

        if (row_full) {
            grid->pause_falling = true;

            for (s32 x = 0; x < grid->grid_w; ++x) {
                GridSlot *b = grid->GetBlock(y, x);

                // start blinking sequence
                if (b->blinking == 0) {
                    b->blinking = 1;
                }
            }
        }
    }

    return can_fall;
}

void FillGridRandomly() {
    for (s32 y = 0; y < grid->grid_h; ++y) {
        for (s32 x = 0; x < grid->grid_w; ++x) {

            GridSlot *block = grid->GetBlock(y, x);

            s32 color_selector = RandMinMaxI(0, 3);
            switch (color_selector) {
            case 0: block->color = COLOR_RED; break;
            case 1: block->color = COLOR_GREEN; break;
            case 2: block->color = COLOR_YELLOW; break;
            case 3: block->color = COLOR_BLUE; break;
            default: assert(1 == 0 && "switch default"); break; }

            block->solid = RandMinMaxI(0, 1) == 1;
        }
    }
}

void FillGridBottomRandomly() {
    for (s32 y = 16 + 4; y < grid->grid_h; ++y) {
        for (s32 x = 0; x < grid->grid_w; ++x) {

            GridSlot *block = grid->GetBlock(y, x);

            s32 color_selector = RandMinMaxI(0, 3);
            switch (color_selector) {
            case 0: block->color = COLOR_RED; break;
            case 1: block->color = COLOR_GREEN; break;
            case 2: block->color = COLOR_YELLOW2; break;
            case 3: block->color = COLOR_BLUE; break;
            default: assert(1 == 0 && "switch default"); break; }

            block->solid = RandMinMaxI(0, 1) == 1;
        }
    }
}

void ClearGridTopAndMiddle() {
    for (s32 y = 0; y < 20; ++y) {
        for (s32 x = 0; x < grid->grid_w; ++x) {

            GridSlot *block = grid->GetBlock(y, x);
            *block = {};
        }
    }
}




#endif
