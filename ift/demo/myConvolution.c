#include "ift.h"
#include <stdlib.h>

int main(int argc, char *argv[]){
	iftImage* orig;
	iftAdjRel* A;
	iftKernel* kernel;
	int width, height, pos;
    char inputPath[100], outputPath[100], kernelPath[100];
    timer *t1 = NULL, *t2 = NULL;
    char outfile[100];
    
	printf("please input: \t<input images dir>\n\t\t<output images dir>\n\t\t<kernel dir>\n");
	scanf("%s", inputPath);
	scanf("%s", outputPath);
	scanf("%d %d", &width, &height);
	A = iftCreateAdjRel(width*height);
	kernel = iftCreateKernel(A);
	pos = 0;
	for (int x=-width/2;x<(width+1)/2;x++){
		for (int y=-height/2;y<(height+1)/2;y++) {
			kernel->A->dx[pos] = x;
			kernel->A->dy[pos] = y;
			kernel->A->dz[pos] = 0;
			scanf("%f", &kernel->weight[pos]);
			pos++;
		}
	}
	/*scanf("%s", kernelPath);
	kernel = iftReadKernel(kernelPath);*/

    iftDir* inputDir = iftLoadFilesFromDirectory(inputPath, "pgm");
	
	for (int i = 0; i < inputDir->nfiles; ++i) {

        orig = iftReadImageP5(inputDir->files[i]->pathname);

        t1 = iftTic();

        iftImage *out = iftLinearFilter(orig, kernel);
        out = iftAbs(out);

        t2 = iftToc();

        sprintf(outfile, "%s/%s", outputPath, iftBasename(inputDir->files[i]->pathname));

		printf("%s", outfile);
        iftWriteImageP2(out, outfile);

        fprintf(stdout, "%dth image plate candidates located in %f ms\n", i+1,  iftCompTime(t1, t2));

        iftDestroyImage(&orig);
        iftDestroyImage(&out);
    }
    return 0;
}
