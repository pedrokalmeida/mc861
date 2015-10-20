#include "ift.h"
#include <stdlib.h>

#define MIN_VOLUME 1000

void iftRemoveSmallComponents(iftImage *img, int minVolume) {

    int p, i;
    int max = iftMaximumValue(img);
    int *volume = iftAllocIntArray(max + 1);
    int *labels = iftAllocIntArray(max + 1);

/* MUDANCA */

	int *minorx  = iftAllocIntArray(max + 1);
	int *largerx  = iftAllocIntArray(max + 1);
	int *minory = iftAllocIntArray(max + 1);
	int *largery = iftAllocIntArray(max + 1);

	for (p = 0; p < max+1; ++p) {
		minory[p] = img->ysize;
		minorx[p] = img->xsize;
	}

    for (p = 0; p < img->n; ++p) {
        if (img->val[p] > 0) {
            volume[img->val[p]]++;   //determina o tamanho de cada candidato/componente da imagem
			if (p/img->xsize < minory[img->val[p]])
				minory[img->val[p]] = p/img->xsize;
			if (p/img->xsize > largery[img->val[p]])
				largery[img->val[p]] = p/img->xsize;
			
			if (p%img->xsize < minorx[img->val[p]])
				minorx[img->val[p]] = p%img->xsize;
			if (p%img->xsize > largerx[img->val[p]]) 
				largerx[img->val[p]] = p%img->xsize;
		}
    }
	
/*	printf("\n menor y: ");

	for (p = 1; p <= max; p++)
		printf("%d ", minory[p]);

	printf("\n maior y: ");

	for (p = 1; p <= max; p++)
		printf("%d ", largery[p]);

	printf("\n menor x: ");

	for (p = 1; p <= max; p++)
		printf("%d ", minorx[p]);

	printf("\n maior x: ");

	for (p = 1; p <= max; p++)
		printf("%d ", largerx[p]);

	printf("\n \n");*/

    int nlabels = 1;			//determina os candidatos que sao maiores que o tamanho de corte
    for (i = 1; i <= max; ++i) {
        if ((largerx[i] - minorx[i]) < (largery[i] - minory[i])) {
		labels[i] = 0;
	}
	else {		
	if (volume[i] >= minVolume)
    	labels[i] = nlabels++;
	else
    	labels[i] = 0;
	}
    }

/* FIM DA MUDANCA */

    for (p = 0; p < img->n; ++p) {		//apaga os candidatos menores que o tamanho de corte
        img->val[p] = labels[img->val[p]];
    }

    free(labels);
    free(volume);
}

iftImage* selectCandidates(iftImage *orig) {
	iftImage *aux[4];// *sem, *com;
    iftKernel *K = NULL;
    iftAdjRel *A = NULL;

    A = iftCircular(5.0);
    aux[0] = iftNormalizeImage(orig, A, 4095);

    aux[1] = iftCloseBasins(aux[0]);
    aux[2] = iftSub(aux[1], aux[0]);
    iftDestroyImage(&aux[1]);

    K = iftSobelXKernel2D();
    aux[1] = iftFastLinearFilter(aux[2], K, 0);
    iftDestroyImage(&aux[0]);
    iftDestroyImage(&aux[2]);
    iftDestroyAdjRel(&A);
    aux[2] = iftAbs(aux[1]);
    iftDestroyImage(&aux[1]);
    /* MUDANCA */
    A = iftRectangular(15, 5);
    aux[2] = iftClose(aux[2], A);
    aux[2] = iftOpen(aux[2], A);
    /* FIM DA MUDANCA */
    
    A = iftRectangular(10, 3);
    aux[0] = iftAlphaPooling(aux[2], A, 4, 2);

    iftDestroyImage(&aux[2]);
    int max = iftMaximumValue(aux[0]);
    int t = iftOtsu(aux[0]);

    aux[2] = iftThreshold(aux[0], t, max, 255);

    A = iftCircular(2.0);
    aux[3] = iftFastLabelComp(aux[2], A);

    iftRemoveSmallComponents(aux[3], MIN_VOLUME);

    iftImage* final = iftAddRectangularBoxFrame(aux[3], 4, 0, 0, 0);

    iftDestroyImage(&aux[0]);
    iftDestroyImage(&aux[1]);
    iftDestroyImage(&aux[2]);
    iftDestroyImage(&aux[3]);

    iftDestroyAdjRel(&A);
    iftDestroyKernel(&K);

    return final;
}

int main(int argc, char *argv[]) {
    iftImage* orig;
    char inputPath[100], outputPath[100];
    timer *t1 = NULL, *t2 = NULL;
    char outfile[100];

    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

	printf("please input: \t<input images dir>\n\t\t<output images dir>\n");
	scanf("%s", inputPath);
	scanf("%s", outputPath);
    iftDir* inputDir = iftLoadFilesFromDirectory(inputPath, "pgm");

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
