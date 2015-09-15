#include "ift.h"
#include "iftSelectCandidates.h"

int main(int argc, char *argv[]) {

    iftImage* orig;
    timer *t1 = NULL, *t2 = NULL;
    char outfile[100];

    /*--------------------------------------------------------*/

    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

    /*--------------------------------------------------------*/

    if (argc != 3) {
        fprintf(stdout, "Usage: iftSelectCandidates <input images folder> <output candidates folder>\n");
        fprintf(stdout, "       input images folder:       path to original grayscale images\n");
        fprintf(stdout, "       output candidate folder: path to binary images with candidates for plate location\n");
        exit(1);
    }

    iftDir* inputDir = iftLoadFilesFromDirectory(argv[1], "pgm");

    for (int i = 0; i < inputDir->nfiles; ++i) {

        orig = iftReadImageP5(inputDir->files[i]->pathname);

        t1 = iftTic();

        iftImage *candidates = selectCandidates(orig);

        t2 = iftToc();

        sprintf(outfile, "%s/%s", argv[2], iftBasename(inputDir->files[i]->pathname));

        iftWriteImageP2(candidates, outfile);

        fprintf(stdout, "%dth image plate candidates located in %f ms\n", i+1,  iftCompTime(t1, t2));

        iftDestroyImage(&orig);
        iftDestroyImage(&candidates);
    }

    /* ---------------------------------------------------------- */
    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial != MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial, MemDinFinal);

    return (0);
}

