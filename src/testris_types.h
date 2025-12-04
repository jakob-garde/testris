#ifndef __TESTRIS_TYPES__
#define __TESTRIS_TYPES__


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
    f32 t_mode_start;
    f32 t_lr_down;

    void SetMode(TestrisMode mode_new, f32 t_start) {
        t_mode_start = t_start;
        mode_prev = mode;
        mode = mode_new;
    }
};


#endif
