#include "ift.h"
#include <stdlib.h>

iftImage* selectCandidates(iftImage *orig) {
	iftImage *aux;
    iftAdjRel *A = NULL;

	/* TODO */

	// qualquer coisa
    A = iftCircular(5.0);
    aux = iftNormalizeImage(orig, A, 4095);
    
    return aux;
}

int main(int argc, char *argv[]) {
    iftImage* orig;
    char inputPath[100], outputPath[100], sedir[100];
    timer *t1 = NULL, *t2 = NULL;
    char outfile[100];
	FILE *se;
    char *line;

    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

	printf("please input: \t<input images dir>\n\t\t<output images dir>\n\t\t<SE dir>\n");
	scanf("%s", inputPath);
	scanf("%s", outputPath);
	scanf("%s", sedir);
	
    iftDir* inputDir = iftLoadFilesFromDirectory(inputPath, "pgm");

	se = fopen(sedir, "r");
    if (se) {
		while (fgets(line, sizeof(line), se)) {
			
		}
    }
    else {
		printf("SE file not found\n");
		return (0);
    }

    for (int i = 0; i < inputDir->nfiles; ++i) {

        orig = iftReadImageP5(inputDir->files[i]->pathname);

        t1 = iftTic();

        iftImage *candidates = selectCandidates(orig);

        t2 = iftToc();

        sprintf(outfile, "%s/%s", outputPath, iftBasename(inputDir->files[i]->pathname));

		printf("%s", outfile);
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
