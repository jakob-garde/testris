#define ENABLE_GLFW
#define GLEW_STATIC

#include "lib/jg_baselayer.h"
#include "lib/jg_cbui.h"

// testris and grid types
#include "src/testris_types.h"

// game state
static Testris testris;
static Grid grid;

// functionality
#include "src/testris_lib.h"
#include "src/render_and_update.h"


// the game loop
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


#if WINDOWS
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    TimeProgram;
    BaselayerAssertVersion(0, 2, 3);
    CbuiAssertVersion(0, 2, 1);

    RunTestris(false);
}
#endif
