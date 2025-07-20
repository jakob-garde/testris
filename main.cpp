#define ENABLE_GLFW


#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"


struct CbuiState {
    MContext *ctx;
    u64 frameno;
    PlafGlfw *plf;
    bool running;
};

static CbuiState _g_cbui_state;
static CbuiState *cbui;

CbuiState *CbuiInit__() {
    cbui = &_g_cbui_state;
    cbui->ctx = InitCbui(&cbui->frameno, &cbui->plf);
    cbui->running = true;
    return cbui;
}

void CbuiFrameStart() {
    ArenaClear(cbui->ctx->a_tmp);
    cbui->frameno++;
    PlafGlfwUpdate(cbui->plf);
    ImageBufferClear(cbui->plf->width, cbui->plf->height);


    // TODO: clean up these globals
    g_mouse_x = cbui->plf->cursorpos.x;
    g_mouse_y = cbui->plf->cursorpos.y;
    g_mouse_down = MouseLeft().ended_down;
    g_mouse_pushed = MouseLeft().pushed;
}

void CbuiFrameEnd() {
    UI_FrameEnd(cbui->ctx->a_tmp, cbui->plf->width, cbui->plf->height);
    SpriteRender_BlitAndCLear(InitImageRGBA(cbui->plf->width, cbui->plf->height, g_image_buffer));
    PlafGlfwUpdate(cbui->plf);

    cbui->running = cbui->running && !GetEscape() && !GetWindowShouldClose(cbui->plf);

    // TODO: get delta t and framerate under control
    XSleep(1);
}

void CbuiExit() {
    PlafGlfwTerminate(cbui->plf);
}


struct Testris {
    u32 mode;
};
static Testris _g_testris_state;


void DoMainMenu() {
    UI_LayoutExpandCenter();

    bool close;
    UI_CoolPopUp(400, 170, 20, &close);
    if (close) cbui->running = false;

    UI_Label("Rotate         Mouse Left");
    UI_Label("Pan            Mouse Right");
    UI_Label("Drag Object    Mouse Left");
    UI_Label("Drag Vertical  Hold Ctrl");
    UI_Label("F1             Help Menu");
    UI_Label("Esc            Quit");
}


void RunProgram() {
    cbui = CbuiInit__();

    Testris *testris = &_g_testris_state;
    testris->mode = 0;

    while (cbui->running) {
        CbuiFrameStart();


        switch (testris->mode) {
            case 0 : {
                DoMainMenu();
            } break;

            case 1 : {

            } break;

        default:
            break;
        }


        CbuiFrameEnd();
    }
    CbuiExit();
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
