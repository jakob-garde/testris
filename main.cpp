#define ENABLE_GLFW


#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"


enum TestrisMode {
    TM_TITLE,
    TM_MAIN,
    TM_HELPS,

    TM_CNT
};

struct Testris {
    TestrisMode mode;
    TestrisMode mode_prev;

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


void DoMainScreen() {
    UI_LayoutExpandCenter();
    //UI_Label("Main Game ...");

    s32 grid_unit_sz = cbui->plf->height / 20;

    //Widget *w = UI_LayoutVertical();
    Widget *w = WidgetGetCached("testris_grid_area");
    TreeBranch(w);
    w->frame_touched = cbui->frameno;
    w->features_flg |= WF_EXPAND_VERTICAL;
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->h = cbui->plf->height;
    w->w = grid_unit_sz * 10;

    w->col_bckgrnd = COLOR_WHITE;
    w->col_border = COLOR_BLACK;
    w->sz_border = 1;


    s32 grid_h = 20;
    s32 grid_w = 10;
    for (s32 i = 0; i < grid_h; ++i) {
        for (s32 j = 0; j < grid_w; ++j) {
            Widget *g = UI_Plain();
            g->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
            g->features_flg |= WF_ABSREL_POSITION;
            g->w = grid_unit_sz;
            g->h = grid_unit_sz;
            g->x0 = j * grid_unit_sz;
            g->y0 = i * grid_unit_sz;

            s32 color_selector = RandMinMaxI(0, 5);
            switch (color_selector) {
            case 0: g->col_bckgrnd = COLOR_RED; break;
            case 1: g->col_bckgrnd = COLOR_GREEN; break;
            case 2: g->col_bckgrnd = COLOR_YELLOW; break;
            case 3: g->col_bckgrnd = COLOR_BLUE; break;
            case 4: g->col_bckgrnd = COLOR_BLACK; break;
            case 5: g->col_bckgrnd = COLOR_WHITE; break;
            default: break; }

            // TODO: store the colours in a persistent grid structure:
            //      We don't need to store it on persistent widgets, although we could
            //      as an exercise. Reason why: The grid holds model data - which we want
            //      to operate on using game logics.

            // TODO: These widget should not be nested by default, necessitating UI_Pop() !
            //      They should have been siblings, s.t. their abs-rel position is calculated
            //      wrt. their common parent.
            UI_Pop();
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
