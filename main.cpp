#define ENABLE_GLFW

#include "lib/jg_baselayer.h"
#include "lib/jg_cbui.h"


struct TestrisState {
    u64 frameno;
    PlafGlfw *plf;
    MContext *ctx;
    u32 mode;
};
static TestrisState _g_state;
static TestrisState *g_state;


void DoMainMenu() {
    UI_LayoutExpandCenter();

    UI_Label("Hello testris");

}


void RunProgram() {
    g_state = &_g_state;
    g_state->ctx = InitBaselayer();
    g_state->plf = PlafGlfwInit("Testris");
    g_state->frameno = 0;
    g_state->mode = 0;

    InitImUi(g_state->plf->width, g_state->plf->height, &g_state->frameno);



    while (GetWindowShouldClose(g_state->plf) == false && GetEscape() == false) {
        XSleep(1);

        switch (g_state->mode) {
            case 0 : {
                DoMainMenu();
            } break;

            case 1 : {

            } break;

        default:
            break;
        }


        UI_FrameEnd(g_state->ctx->a_tmp, g_state->plf->width, g_state->plf->height);
        PlafGlfwUpdate(g_state->plf);
    }
}


void Test() {
    printf("Running tests ...\n");
}


int main (int argc, char **argv) {
    TimeProgram;
    BaselayerAssertVersion(0, 2, 3);
    CbuiAssertVersion(0, 2, 0);

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
