#define ENABLE_GLFW


#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"
#include "src/testris.h"


static Testris _g_testris_state;
static Testris *testris;


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

    for (s32 y = 4; y < grid->grid_h; ++y) {
        for (s32 x = 0; x < grid->grid_w; ++x) {
            Block *b = grid->GetBlock(y, x);
            if (b->solid == true) {

                Widget *g = UI_Plain();
                g->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
                g->features_flg |= WF_ABSREL_POSITION;
                g->w = grid_unit_sz;
                g->h = grid_unit_sz;
                g->x0 = x * grid_unit_sz;
                g->y0 = (y - 4) * grid_unit_sz;
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
            default: assert(1 == 0 && "switch default"); break; }

            block->solid = RandMinMaxI(0, 1) == 1;
            if (block->solid == true) {
                block->falling = true;
            }
        }
    }
}

void FillGridBottomRandomly() {
    for (s32 y = 16; y < grid->grid_h; ++y) {
        for (s32 x = 0; x < grid->grid_w; ++x) {

            Block *block = grid->GetBlock(y, x);

            s32 color_selector = RandMinMaxI(0, 3);
            switch (color_selector) {
            case 0: block->color = COLOR_RED; break;
            case 1: block->color = COLOR_GREEN; break;
            case 2: block->color = COLOR_YELLOW; break;
            case 3: block->color = COLOR_BLUE; break;
            default: assert(1 == 0 && "switch default"); break; }

            block->solid = RandMinMaxI(0, 1) == 1;
            if (block->solid == true) {
                block->falling = false;
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
    //FillGridBottomRandomly();

    //SpawnFallingBlocks();


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
