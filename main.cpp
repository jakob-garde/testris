#define ENABLE_GLFW

#include "lib/jg_baselayer.h"
#include "lib/jg_cbui.h"




void RunProgram() {
    MContext *ctx = InitBaselayer();


    PlafGlfw* plf = PlafGlfwInit("Testris");
    u64 frameno = 0;
    InitImUi(plf->width, plf->height, &frameno);


    while (GetWindowShouldClose(plf) == false && GetEscape() == false) {
        XSleep(1);


        UI_FrameEnd(ctx->a_tmp, plf->width, plf->height);
        PlafGlfwUpdate(plf);
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
