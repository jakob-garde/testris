#define ENABLE_GLFW


#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"


struct Block {
    Color color;
    bool empty;
    bool falling;
};
static Block g_block_null; // ZII default return value

struct Grid {
    s32 grid_w = 10;
    s32 grid_h = 20;
    Block data[24][10];

    Block *GetBlock(s32 y, s32 x) {
        if (y >= -3 && y < grid_h && x >= 0 && x < grid_w) {
            return &data[y + 4][x];
        }
        else {
            g_block_null = {};
            return &g_block_null;
        }
    }

    void SetBlock(s32 y, s32 x, Block b) {
        if (y >= -3 && y < grid_h && x >= 0 && x < grid_w) {
            data[y + 4][x] = b;
        }
        else {
            assert(1 == 0 && "SetBlock: out of scope");
        }
    }

    void ClearBlock(s32 y, s32 x) {
        if (y >= -3 && y < grid_h && x >= 0 && x < grid_w) {
            data[y + 4][x] = {};
            data[y + 4][x].empty = true;
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

static Testris _g_testris_state;
static Testris *testris;


void FillGridDataRandomly() {
    for (s32 y = 0; y < grid->grid_h; ++y) {
        for (s32 x = 0; x < grid->grid_w; ++x) {

            Block *block = grid->GetBlock(y, x);

            s32 color_selector = RandMinMaxI(0, 3);
            switch (color_selector) {
            case 0: block->color = COLOR_RED; break;
            case 1: block->color = COLOR_GREEN; break;
            case 2: block->color = COLOR_YELLOW; break;
            case 3: block->color = COLOR_BLUE; break;
            default: break; }

            block->empty = RandMinMaxI(0, 1) == 1;
            if (block->empty == false) {
                //block->falling = RandMinMaxI(0, 5) < 5;
                block->falling = true;
            }
        }
    }
}


void UpdateBlocks() {

    // fall blocks
    for (s32 y = grid->grid_h - 1; y >= 0; --y) {
        for (s32 x = 0; x < grid->grid_w; ++x) {

            Block *b = grid->GetBlock(y, x);
            if (b->empty == false && b->falling == true) {

                Block *below = grid->GetBlock(y + 1, x);
                if (below->empty == true && y < (grid->grid_h - 1)) {

                    grid->SetBlock(y+1, x, *b);
                    grid->ClearBlock(y, x);
                }
                else {
                    b->falling = false;
                }
            }

            if (b->falling == false) {
                b->color = COLOR_GRAY;
            }
        }
    }
}


void DoMainScreen() {
    f32 grid_unit_sz = cbui->plf->height / 20.0f;


    if (testris->main_timeout >= 500) {
        testris->main_timeout = 0;
    }
    if (testris->main_timeout == 0) {
        UpdateBlocks();
    }
    testris->main_timeout += cbui->dt;


    // render
    UI_LayoutExpandCenter();
    Widget *w = UI_LayoutVertical();
    w->features_flg |= WF_EXPAND_VERTICAL;
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->h = cbui->plf->height;
    w->w = grid_unit_sz * 10;
    w->col_bckgrnd = COLOR_WHITE;

    for (s32 y = 0; y < grid->grid_h; ++y) {
        for (s32 x = 0; x < grid->grid_w; ++x) {
            Block *b = grid->GetBlock(y, x);
            if (b->empty == false) {

                Widget *g = UI_Plain();
                g->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
                g->features_flg |= WF_ABSREL_POSITION;
                g->w = grid_unit_sz;
                g->h = grid_unit_sz;
                g->x0 = x * grid_unit_sz;
                g->y0 = y * grid_unit_sz;
                g->col_border = COLOR_WHITE;
                g->sz_border = 1;
                g->col_bckgrnd = b->color;

                // TODO: These widget should not be nested by default, necessitating UI_Pop() !
                //      They should have been siblings, s.t. their abs-rel position is calculated
                //      wrt. their common parent.

                UI_Pop();
            }
        }
    }
}


void DoHelpMenu() {
    UI_LayoutExpandCenter();

    bool close;
    UI_CoolPopUp(470, 165, 20, &close);
    if (close) testris->SetModePrev();

    UI_Label("Rotate Block   Up");
    UI_Label("Move Block     Left/Right/Down");
    UI_Label("");
    UI_Label("Help           F1");
    UI_Label("Fullscreen     F10");
    UI_Label("Exit           Esc");
}

void DoTitleScreen() {
    UI_LayoutExpandCenter();
    Widget *vert = UI_LayoutVertical(true);

    SetFontSize(FS_48);
    Widget *lbl_1 = UI_Label("Welcome to Testris");
    SetFontSize(FS_24);
    Widget *lbl_2 = UI_Label("(press space to start)");

    if (GetSpace()) {
        testris->SetMode(TM_MAIN);
    }
}

void RunTestris() {
    cbui = CbuiInit();
    testris = &_g_testris_state;
    grid = &_grid;


    // setup the test state
    testris->mode = TM_MAIN;
    testris->main_timeout = 1;
    FillGridDataRandomly();


    while (cbui->running) {
        CbuiFrameStart();

        switch (testris->mode) {
            case TM_TITLE : {
                DoTitleScreen();
            } break;

            case TM_MAIN : {
                DoMainScreen();
            } break;

            case TM_HELPS : {
                DoHelpMenu();
            } break;

            case 3 : {
                //
            } break;

            case 4 : {
                //
            } break;

            default: break;
        }

        if (GetFKey(1)) {
            printf("toggling ...\n");
            testris->ToggleMode(TM_HELPS);
        }

        CbuiFrameEnd();
    }
    CbuiExit();
}


int main (int argc, char **argv) {
    TimeProgram;
    BaselayerAssertVersion(0, 2, 3);
    CbuiAssertVersion(0, 2, 1);

    RunTestris();
}
