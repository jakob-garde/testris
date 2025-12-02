#define ENABLE_GLFW
#define GLEW_STATIC


#include "lib/jg_baselayer.h"
#include "lib/jg_cbui.h"
#include "src/testris.h"


f32 RenderGame() {
    // render the grid
    f32 grid_unit_sz = cbui->plf->height / (1.0f * grid.visible_height);

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

    for (s32 y = 4; y < grid.height; ++y) {
        for (s32 x = 0; x < grid.width; ++x) {
            GridSlot *b = grid.GetSlot(y, x);
            if (b->solid == true) {
                Widget *g = WidgetGetNew();
                TreeSibling(g);

                g->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
                g->features_flg |= WF_ABSREL_POSITION;
                g->w = grid_unit_sz;
                g->h = grid_unit_sz;
                g->x0 = x * grid_unit_sz;
                g->y0 = (y - 4) * grid_unit_sz;
                g->col_border = COLOR_WHITE;
                g->sz_border = 1;
                g->col_bckgrnd = b->color;
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

            bool do_fill = grid.falling.data[y][x];
            s32 yy = y - 4 + grid.falling.grid_y;
            s32 xx = x + grid.falling.grid_x;
            if (do_fill && yy >= 0 && yy < grid.height - 4 && xx >= 0 && x < grid.width) {

                Widget *g = UI_Plain();
                g->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
                g->features_flg |= WF_ABSREL_POSITION;
                g->w = grid_unit_sz;
                g->h = grid_unit_sz;
                g->x0 = xx * grid_unit_sz;
                g->y0 = yy * grid_unit_sz;
                g->col_border = COLOR_WHITE;
                g->sz_border = 1;
                g->col_bckgrnd = grid.falling.color;

                UI_Pop();
            }
        }
    }

    // render the next block
    s32 offset_x = - 5 * grid_unit_sz;
    s32 offset_y = grid_unit_sz;
    for (s32 y = 0; y < 4; ++y) {
        for (s32 x = 0; x < 4; ++x) {

            bool do_fill = grid.next.data[y][x];
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
                g->col_bckgrnd = grid.next.color;

                UI_Pop();
            }
        }
    }

    return w_grid->w + 0.6f * grid_unit_sz;
}


void UpdateTime() {
    testris.t_fall += cbui->dt;
    if (testris.t_fall >= testris.t_fall_interval) {
        testris.t_fall = 0;
    }

    testris.t_fall_interval = 400;
}


void DoMainScreen() {
    UpdateTime();
    UpdateGridState();

    // controls
    if (GetChar('w') || GetUp()) {
        BlockRotateIfAble();
    }
    else if (GetChar('a') || GetLeft()) {
        BlockLeftIfAble();
    }
    else if (GetChar('d') || GetRight()) {
        BlockRightIfAble();
    }

    else if (GetChar('s') || GetDown()) {
        BlockFallOrFreeze();
    }
    else if (GetSpace()) {
        while (BlockFallOrFreeze());
    }

    // auto-fall
    else if (testris.t_fall == 0) {
        BlockFallOrFreeze();
    }

    // render
    RenderGame();
}


void DoGameOver() {
    f32 grid_visual_width = RenderGame();

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

    SetFontSize(FS_36);
    Widget *q = UI_Label("GAME OVER");


    if (GetSpace()) {
        if (TimeSinceModeStart_ms() > 300.0f) {
            ClearGridTopAndMiddle();
            FillGridBottomRandomly();

            grid.falling = BlockCreate();
            grid.next = BlockCreate();

            testris.SetMode(TM_MAIN, cbui->t_framestart);
        }
    }
}

void DoTitleScreen() {
    f32 grid_visual_width = RenderGame();

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

    SetFontSize(FS_36);
    UI_Label("");
    UI_Label("TESTRIS");
    SetFontSize(FS_18);
    UI_Label("[l/r/u/d space]");

    if (GetSpace()) {
        grid.falling = BlockCreate();
        grid.next = BlockCreate();

        testris.SetMode(TM_MAIN, cbui->t_framestart);
    }
}


void RunTestris(bool start_in_fullscreen) {
    cbui = CbuiInit("Testris", start_in_fullscreen);

    FillGridBottomRandomly();

    while (cbui->running) {
        CbuiFrameStart();

        switch (testris.mode) {
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

    bool start_in_fullscreen = CLAContainsArg("--fullscreen", argc, argv);
    RunTestris(start_in_fullscreen);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    TimeProgram;
    BaselayerAssertVersion(0, 2, 3);
    CbuiAssertVersion(0, 2, 1);

    RunTestris(false);
}
