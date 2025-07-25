#define ENABLE_GLFW


#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"
#include "src/testris.h"


f32 RenderMainScreen() {
    // render the grid
    f32 grid_unit_sz = cbui->plf->height / 20.0f;

    UI_LayoutExpandCenter();
    Widget *w_grid  = WidgetGetCached("testris_grid");
    TreeBranch(w_grid);
    w_grid->frame_touched = cbui->frameno;
    w_grid->features_flg |= WF_ABSREL_POSITION;
    w_grid->features_flg |= WF_LAYOUT_VERTICAL;
    w_grid->features_flg |= WF_EXPAND_VERTICAL;
    w_grid->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w_grid->h = cbui->plf->height;
    w_grid->w = grid_unit_sz * 10;
    w_grid->x0 = (cbui->plf->width - w_grid->w) / 2.0f;
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

                UI_Pop();
            }
        }
    }

    // render grid framing lines
    s16 ax = round( w_grid->x0 - 0.3f * grid_unit_sz );
    s16 ay = round( w_grid->y0 );
    s16 bx = round( w_grid->x0 - 0.3f * grid_unit_sz );
    s16 by = round(w_grid->y0 + w_grid->h );
    RenderLineRGBA(cbui->plf->image_buffer, cbui->plf->width, cbui->plf->height, ax, ay, bx, by, COLOR_GRAY_60);
    ax = round( w_grid->x0 + w_grid->w + 0.3f * grid_unit_sz );
    ay = round( w_grid->y0 );
    bx = round( w_grid->x0 + w_grid->w + 0.3f * grid_unit_sz );
    by = round( w_grid->y0 + w_grid->h );
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

    return w_grid->w + 0.6f * grid_unit_sz;
}


void UpdateGameSpeed() {
    if (testris->t_fall_interval == 0) {
        testris->t_fall_interval = 400;
    }
    f32 t_was = testris->t_fall_interval;

    // TODO: impl. the escalating speed

    if (t_was != testris->t_fall_interval) {
        printf("Game speed set to: %f ms\n", testris->t_fall_interval);
    }
}


void DoMainScreen() {
    // spawn
    if (grid->falling.tpe == BT_UNINITIALIZED) {
        if (grid->next.tpe == BT_UNINITIALIZED) {
            grid->falling = BlockSpawn();
        }
        else {
            grid->falling = grid->next;
        }
        grid->next = BlockSpawn();
    }

    // check filled lines
    UpdateGrid();
    UpdateGameSpeed();

    // auto-fall
    if (testris->t_fall >= testris->t_fall_interval) {
        testris->t_fall = 0;
    }
    if (testris->t_fall == 0) {
        BlockFall();
    }
    testris->t_fall += cbui->dt;

    // controls
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
    if (GetSpace()) {
        while (BlockFall());
    }

    // render
    RenderMainScreen();
}


void DoGameOver() {
    f32 grid_visual_width = RenderMainScreen();

    UI_Pop();
    Widget *w = WidgetGetCached("game_over_panel");
    w->frame_touched = cbui->frameno;
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_LAYOUT_CENTER;

    w->w = grid_visual_width - 2;
    w->h = 0.4* w->w;
    w->sz_border = 0;
    w->col_bckgrnd = COLOR_WHITE;
    w->col_border = COLOR_GRAY_50;

    TreeBranch(w);

    SetFontSize(FS_24);
    Widget *q = UI_Label("GAME OVER");


    //if (GetSpace() && ((cbui->TimeSince(testris->mode_t_start)) > 1000.0f) ) {
    if (GetSpace()) {
        if (TimeSinceModeStart_ms() > 300.0f) {
            *grid = {};
            FillGridBottomRandomly();

            testris->SetMode(TM_MAIN, cbui->t_framestart);
        }
    }
}

void DoTitleScreen() {
    f32 grid_visual_width = RenderMainScreen();

    UI_Pop();
    Widget *w = WidgetGetCached("game_over_panel");
    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
    w->features_flg |= WF_LAYOUT_VERTICAL;
    w->features_flg |= WF_ALIGN_CENTER;

    w->w = grid_visual_width - 2;
    w->h = 0.4* w->w;
    w->sz_border = 0;
    w->col_bckgrnd = COLOR_WHITE;
    w->col_border = COLOR_GRAY_50;

    TreeBranch(w);

    SetFontSize(FS_24);
    UI_Label("");
    UI_Label("TESTRIS");
    SetFontSize(FS_18);
    UI_Label("[l/r/u/d space]");

    if (GetSpace()) {
        testris->SetMode(TM_MAIN, cbui->t_framestart);
    }
}


void RunTestris() {
    cbui = CbuiInit();
    testris = &_g_testris_state;
    grid = &_grid;


    // setup the test state
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

            case TM_GAMEOVER : {
                DoGameOver();
            } break;

            default: break;
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
