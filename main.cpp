#define ENABLE_GLFW


#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"


struct TestrisState {
    u64 frameno;
    PlafGlfw *plf;
    MContext *ctx;
    u32 mode;
    bool running;
};
static TestrisState _g_state;
static TestrisState *g_state;


void DoMainMenu() {
    UI_LayoutExpandCenter();

    bool close;
    UI_CoolPopUp(400, 170, 20, &close);
    if (close) g_state->running = false;

    UI_Label("Rotate         Mouse Left");
    UI_Label("Pan            Mouse Right");
    UI_Label("Drag Object    Mouse Left");
    UI_Label("Drag Vertical  Hold Ctrl");
    UI_Label("F1             Help Menu");
    UI_Label("Esc            Quit");
}


void RunProgram() {
    g_state = &_g_state;
    g_state->frameno = 0;
    g_state->mode = 0;
    g_state->ctx = InitCbui(&g_state->frameno, &g_state->plf);
    g_state->running = true;

    while (g_state->running) {

        // TODO: should be in the gamestate struct, and generalized in the init.h cbui unit
        ArenaClear(g_state->ctx->a_tmp);
        g_state->frameno++;
        PlafGlfwUpdate(g_state->plf);
        ImageBufferClear(g_state->plf->width, g_state->plf->height);


        g_mouse_x = g_state->plf->cursorpos.x;
        g_mouse_y = g_state->plf->cursorpos.y;
        g_mouse_down = MouseLeft().ended_down;
        g_mouse_pushed = MouseLeft().pushed;

        switch (g_state->mode) {
            case 0 : {
                DoMainMenu();
            } break;

            case 1 : {

            } break;

        default:
            break;
        }


        // TODO: should also be under control in init.h unit
        UI_FrameEnd(g_state->ctx->a_tmp, g_state->plf->width, g_state->plf->height);
        SpriteRender_BlitAndCLear(InitImageRGBA(g_state->plf->width, g_state->plf->height, g_image_buffer));
        PlafGlfwUpdate(g_state->plf);

        bool running = running && !GetEscape() && !GetWindowShouldClose(g_state->plf);


        // TODO: get delta t and framerate under control
        XSleep(1);
    }

    PlafGlfwTerminate(g_state->plf);
}


void Test() {
    printf("Running tests ...\n");
}


int main (int argc, char **argv) {
    TimeProgram;
    BaselayerAssertVersion(0, 2, 3);
    CbuiAssertVersion(0, 2, 1);

    if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
        printf("--test:          run test functions\n");
        printf("--count [int]:   an example parameter\n");
        exit(0);
    }
    else if (CLAContainsArg("--version", argc, argv)) {
        BaselayerPrintVersion();
    }
    else if (CLAContainsArg("--test", argc, argv)) {
        Test();
    }
    else {
        RunProgram();
    }
}
