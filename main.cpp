#define ENABLE_GLFW


#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"


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
