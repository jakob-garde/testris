#include "lib/jg_baselayer.h"
#include "lib/jg_cbui.h"


void RunProgram() {
    TimeFunction;

    printf("Executing program ...\n");
}


void Test() {
    printf("Running tests ...\n");
}


int main (int argc, char **argv) {
    TimeProgram;
    BaselayerAssertVersion(0, 2, 3);
    CbuiAssertVersion(0, 2, 0);

    if (CLAContainsArg("--input", argc, argv)) {
        s32 cnt = ParseInt( CLAGetArgValue("--input", argc, argv) );
        printf("input: %d\n", cnt);
        exit(0);
    }
    else if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
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
        InitBaselayer();
        RunProgram();
    }
}
