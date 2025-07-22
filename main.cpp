#define ENABLE_GLFW


#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"


struct Block {
    Color color;
    bool solid;
    bool falling;
};

struct Grid {
    s32 grid_w = 10;
    s32 grid_h = 20;
    Block data[24][10];

    Block *GetBlock(s32 y, s32 x) {
        assert(y >= 0 && y < grid_h);
        assert(x >= 0 && x < grid_w);

        return &data[y + 4][x];
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
    for (s32 i = 0; i < grid->grid_h; ++i) {
        for (s32 j = 0; j < grid->grid_w; ++j) {

            Block *block = grid->GetBlock(i, j);

            s32 color_selector = RandMinMaxI(0, 5);
            switch (color_selector) {
            case 0: block->color = COLOR_RED; break;
            case 1: block->color = COLOR_GREEN; break;
            case 2: block->color = COLOR_YELLOW; break;
            case 3: block->color = COLOR_BLUE; break;
            case 4: block->color = COLOR_BLACK; break;
            default: break; }

            block->falling = false;
            block->solid = RandMinMaxI(0, 1) == 1;
        }
    }
}

void DoMainScreen() {
    f32 grid_unit_sz = cbui->plf->height / 20.0f;


    if (testris->main_timeout >= 500) {
        testris->main_timeout = 0;
    }
    if (testris->main_timeout == 0) {
        FillGridDataRandomly();
    }
    testris->main_timeout += cbui->dt;


    UI_LayoutExpandCenter();
    Widget *w = UI_LayoutVertical();
    w->features_flg |= WF_EXPAND_VERTICAL;
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->h = cbui->plf->height;
    w->w = grid_unit_sz * 10;
    w->col_bckgrnd = COLOR_WHITE;

    for (s32 i = 0; i < grid->grid_h; ++i) {
        for (s32 j = 0; j < grid->grid_w; ++j) {
            Block *b = grid->GetBlock(i, j);
            if (b->solid) {
                Widget *g = UI_Plain();
                g->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
                g->features_flg |= WF_ABSREL_POSITION;
                g->w = grid_unit_sz;
                g->h = grid_unit_sz;
                g->x0 = j * grid_unit_sz;
                g->y0 = i * grid_unit_sz;
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

    testris->mode = TM_MAIN;

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
