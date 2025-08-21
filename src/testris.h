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
GridSlot block_zero;

struct Grid {
    s32 width = 10;
    s32 height = 24;
    s32 visible_height = 20;
    GridSlot data[24][10];

    Block falling;
    Block next;
    bool pause_falling;

    GridSlot *GetSlot(s32 row, s32 col) {
        if (row >= 0 && row < height && col >= 0 && col < width) {
            return &data[row][col];
        }
        else {
            block_zero = {};
            block_zero.solid = true;

            return &block_zero;
        }
    }

    void SetSlot(s32 row, s32 col, GridSlot b) {
        if (row >= 0 && row < height && col >= 0 && col < width) {
            data[row][col] = b;
        }
        else {
            assert(1 == 0 && "SetBlock: out of scope");
        }
    }

    void ClearSlot(s32 row, s32 col) {
        if (row >= 0 && row < height && col >= 0 && col < width) {
            data[row][col] = {};
        }
        else {
            assert(1 == 0 && "ClearBlock: out of scope");
        }
    }
};

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


// globals
static Testris testris;
static Grid grid;


f32 TimeSinceModeStart_ms() {
    f32 t_delta_ms = (cbui->t_framestart - testris.t_mode_start) / 1000;
    return t_delta_ms;
}

void UpdateGridState() {
    // find the blinking row, or eliminate a blinking row that has timed out
    for (s32 row = 0; row < grid.height; ++row) {

        for (s32 col = 0; col < grid.width; ++col) {
            GridSlot *b = grid.GetSlot(row, col);

            if (b->blinking > (testris.t_animate_interval * 3)) {

                // eliminate this row
                for (s32 z = row - 1; z >= 0; --z) {
                    for (s32 x = 0; x < grid.width; ++x) {

                        grid.data[z + 1][x] = grid.data[z][x];
                        grid.data[z][x] = {};
                    }
                }
                grid.pause_falling = false;

                return;
            }
            else if (b->blinking > 0 && (b->blinking > testris.t_animate_interval * 2)) {
                b->color = COLOR_BLACK;
            }
            else if (b->blinking > 0 && (b->blinking > testris.t_animate_interval)) {
                b->color = COLOR_GRAY;
            }
            else if (b->blinking > 0) {
                b->color = COLOR_BLACK;
            }

            if (b->blinking > 0) {
                b->blinking += cbui->dt;
            }
        }
    }
}

bool BlockCollides(Block block) {
    for (s32 row = 0; row < 4; ++row) {
        for (s32 col = 0; col < 4; ++col) {

            if (block.data[row][col]) {
                s32 y = row + block.grid_y;
                s32 x = col + block.grid_x;

                bool in_range = y >= 0 && y < grid.height && x >= 0 && col < grid.width;
                bool collides = !in_range || grid.GetSlot(y, x)->solid;

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
    // NOTE: Only mirror the first 3 columns: Otherwise the blocks spawn at inconsistent column position

    Block m = b;
    for (s32 y = 0; y < 4; ++y) {
        m.data[y][0] = b.data[y][2];
        m.data[y][2] = b.data[y][0];
    }
    return m;
}

void BlockRotateIfAble() {
    Block rot = BlockRotate(grid.falling);
    if (BlockCollides(rot) == false) {
        grid.falling = rot;
    }
}

void BlockLeftIfAble() {
    Block left = grid.falling;
    left.grid_x -= 1;
    if (BlockCollides(left) == false) {
        grid.falling = left;
    }
}

void BlockRightIfAble() {
    Block right = grid.falling;
    right.grid_x += 1;
    if (BlockCollides(right) == false) {
        grid.falling = right;
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
        default: assert(1 == 0 && "switch default"); break;
    }

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
        default: assert(1 == 0 && "switch default"); break;
    }

    // randomly mirror
    if (RandMinMaxI(0, 1) == 1) {
        block = BlockMirrorX(block);
    }

    // randomly rotate
    s32 rotations = RandMinMaxI(0, 3);
    for (s32 i = 0; i < rotations; ++i) {
        block = BlockRotate(block);
    }

    return block;
}

bool BlockFallOrFreeze() {
    Block test = grid.falling;
    test.grid_y += 1;
    bool can_fall = ! BlockCollides(test);

    if (can_fall) {
        grid.falling.grid_y += 1;
    }

    else {
        GridSlot slot = {};
        slot.color = grid.falling.color;
        slot.solid = true;

        // solidify into the grid
        for (s32 row = 0; row < 4; ++row) {
            for (s32 col = 0; col < 4; ++col) {
                if (grid.falling.data[row][col]) {
                    s32 y = row + grid.falling.grid_y;
                    s32 x = col + grid.falling.grid_x;

                    grid.SetSlot(y, x, slot);
                }
            }
        }

        if (grid.falling.grid_y < 4) {
            testris.SetMode(TM_GAMEOVER, cbui->t_framestart);
        }

        // spawn
        if (grid.next.tpe == BT_UNINITIALIZED) {
            grid.falling = BlockCreate();
        }
        else {
            grid.falling = grid.next;
        }

        grid.falling = grid.next;
        grid.next = BlockCreate();
    }

    // find full rows
    for (s32 row = grid.height - 1; row >= 0; --row) {
        bool row_full = true;
        for (s32 col = 0; col < grid.width; ++col) {
            row_full = row_full && grid.GetSlot(row, col)->solid;
        }

        if (row_full) {
            grid.pause_falling = true;

            for (s32 col = 0; col < grid.width; ++col) {
                GridSlot *b = grid.GetSlot(row, col);

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
    for (s32 row = 0; row < grid.height; ++row) {
        for (s32 col = 0; col < grid.width; ++col) {

            GridSlot *block = grid.GetSlot(row, col);

            s32 color_selector = RandMinMaxI(0, 3);
            switch (color_selector) {
                case 0: block->color = COLOR_RED; break;
                case 1: block->color = COLOR_GREEN; break;
                case 2: block->color = COLOR_YELLOW; break;
                case 3: block->color = COLOR_BLUE; break;
                default: assert(1 == 0 && "switch default"); break;
            }

            block->solid = RandMinMaxI(0, 1) == 1;
        }
    }
}

void FillGridBottomRandomly() {
    for (s32 row = grid.visible_height; row < grid.height; ++row) {
        for (s32 col = 0; col < grid.width; ++col) {

            GridSlot *block = grid.GetSlot(row, col);

            s32 color_selector = RandMinMaxI(0, 3);
            switch (color_selector) {
                case 0: block->color = COLOR_RED; break;
                case 1: block->color = COLOR_GREEN; break;
                case 2: block->color = COLOR_YELLOW2; break;
                case 3: block->color = COLOR_BLUE; break;
                default: assert(1 == 0 && "switch default"); break;
            }

            block->solid = RandMinMaxI(0, 1) == 1;
        }
    }
}

void ClearGridTopAndMiddle() {
    for (s32 row = 0; row < grid.visible_height; ++row) {
        for (s32 col = 0; col < grid.width; ++col) {

            GridSlot *block = grid.GetSlot(row, col);
            *block = {};
        }
    }
}


#endif
