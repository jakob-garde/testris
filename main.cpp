#define ENABLE_GLFW


#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"
#include "src/testris.h"


static Testris _g_testris_state;
static Testris *testris;


void GridUpdate() {
    s32 y_eliminated = -1;

    for (s32 y = 0; y < grid->grid_h; ++y) {

        bool line_eliminated = true;

        for (s32 x = 0; x < grid->grid_w; ++x) {
            GridSlot *b = grid->GetBlock(y, x);
            line_eliminated = line_eliminated && b->animate > 200.0f;

            if (b->animate > 0 && b->animate < 60.0f) {
                b->color = COLOR_BLACK;
            }
            else if (b->animate > 0 && b->animate < 120.0f) {
                b->color = COLOR_WHITE;
            }
            else if (b->animate > 0 && b->animate < 200.0f) {
                b->color = COLOR_BLACK;
            }
            else if (b->animate > 0.6f) {
                b->solid = false;
                b->animate = 0;

            }

            if (b->animate > 0) {
                b->animate += cbui->dt;
            }
        }

        if (line_eliminated) {
            GridLineEliminate(y);
        }
    }
}

void RenderMainScreen() {
    // render the grid
    f32 grid_unit_sz = cbui->plf->height / 20.0f;

    UI_LayoutExpandCenter();
    Widget *w_grid  = WidgetGetCached("testris_grid");
    TreeBranch(w_grid);
    w_grid->frame_touched = cbui->frameno;
    w_grid->features_flg |= WF_LAYOUT_VERTICAL;
    w_grid->features_flg |= WF_EXPAND_VERTICAL;
    w_grid->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w_grid->h = cbui->plf->height;
    w_grid->w = grid_unit_sz * 10;
    w_grid->col_bckgrnd = COLOR_WHITE;

    for (s32 y = 4; y < grid->grid_h; ++y) {
        for (s32 x = 0; x < grid->grid_w; ++x) {
            GridSlot *b = grid->GetBlock(y, x);
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

    // render grid framing lines
    s16 ax = w_grid->x0 - 0.3f * grid_unit_sz;
    s16 ay = w_grid->y0;
    s16 bx = w_grid->x0 - 0.3f * grid_unit_sz;
    s16 by = w_grid->y0 + w_grid->h;
    RenderLineRGBA(cbui->plf->image_buffer, cbui->plf->width, cbui->plf->height, ax, ay, bx, by, COLOR_GRAY_60);
    ax = w_grid->x0 + w_grid->w + 0.3f * grid_unit_sz;
    ay = w_grid->y0;
    bx = w_grid->x0 + w_grid->w + 0.3f * grid_unit_sz;
    by = w_grid->y0 + w_grid->h;
    RenderLineRGBA(cbui->plf->image_buffer, cbui->plf->width, cbui->plf->height, ax, ay, bx, by, COLOR_GRAY_60);
    
    
    // render the falling block
    for (s32 y = 0; y < 4; ++y) {
        for (s32 x = 0; x < 4; ++x) {

            bool do_fill = grid->falling.data[y][x];
            s32 yy = y - 4 + grid->falling.grid_y;
            s32 xx = x + grid->falling.grid_x;
            if (do_fill && yy >= 0 && yy < grid->grid_h - 4 && xx >= 0 && x < grid->grid_w) {

                Widget *g = UI_Plain();
                g->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
                g->features_flg |= WF_ABSREL_POSITION;
                g->w = grid_unit_sz;
                g->h = grid_unit_sz;
                g->x0 = xx * grid_unit_sz;
                g->y0 = yy * grid_unit_sz;
                g->col_border = COLOR_WHITE;
                g->sz_border = 1;
                g->col_bckgrnd = grid->falling.color;

                UI_Pop();
            }
        }
    }


    // render the next block
    s32 offset_x = - 5 * grid_unit_sz;
    s32 offset_y = grid_unit_sz;
    for (s32 y = 0; y < 4; ++y) {
        for (s32 x = 0; x < 4; ++x) {

            bool do_fill = grid->next.data[y][x];
            if (do_fill) {

                Widget *g = UI_Plain();
                g->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
                g->features_flg |= WF_ABSREL_POSITION;
                g->w = grid_unit_sz;
                g->h = grid_unit_sz;
                g->x0 = x * grid_unit_sz + offset_x;
                g->y0 = y * grid_unit_sz + offset_y;
                g->col_border = COLOR_WHITE;
                g->sz_border = 1;
                g->col_bckgrnd = grid->next.color;

                UI_Pop();
            }
        }
    }
}

void DoMainScreen() {
    GridUpdate();

    if (testris->main_timeout >= 400) {
        testris->main_timeout = 0;
    }
    if (testris->main_timeout == 0) {
        if (grid->falling.tpe == BT_UNINITIALIZED) {
            if (grid->next.tpe == BT_UNINITIALIZED) {
                grid->falling = BlockSpawn();
            }
            else {
                grid->falling = grid->next;
            }
            grid->next = BlockSpawn();
        }
        BlockFall();
        GridLineEliminateStartAnimation();
    }
    testris->main_timeout += cbui->dt;

    if (GetChar('a')) {
        BlockLeftIfAble();
    }
    if (GetChar('d')) {
        BlockRightIfAble();
    }
    if (GetChar('w')) {
        BlockRotateIfAble();
    }
    if (GetChar('s')) {
        BlockFall();
    }

    RenderMainScreen();
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

void FillGridDataRandomly() {
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

void RunTestris() {
    cbui = CbuiInit();
    testris = &_g_testris_state;
    grid = &_grid;


    // setup the test state
    testris->mode = TM_MAIN;
    FillGridBottomRandomly();
    BlockSpawn();


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
