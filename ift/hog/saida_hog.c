#include "hog.h"

int main() {
	int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

	iftDir *orig_dir;	
    orig_dir = iftLoadFilesFromDirectory("orig", "pgm");
    char name[100];

	for(int i=0; i<orig_dir->nfiles; i++) {
		iftImage *img = iftReadImageByExt(orig_dir->files[i]->pathname);

		iftImage *img_v1 = firstStep_normalize_v1(img);
		sprintf(name, "%s/1_v1_r5_L4095_%s", "result", iftBasename(orig_dir->files[i]->pathname));
		iftWriteImageP2(img_v1, name);
	
		iftImage *img_v2 = firstStep_normalize_v2(img);
		sprintf(name, "%s/1_v2_K2000_%s", "result", iftBasename(orig_dir->files[i]->pathname));
		iftWriteImageP2(img_v2, name);

		iftImage *g_mag;
		iftImage *g_orient;
		secondStep_gradient_v2(img_v2, &g_mag, &g_orient);
		sprintf(name, "%s/2_v2_r3_mag_%s", "result", iftBasename(orig_dir->files[i]->pathname));
		iftWriteImageP2(g_mag, name);
		sprintf(name, "%s/2_v2_r3_orient_%s", "result", iftBasename(orig_dir->files[i]->pathname));
		iftWriteImageP2(g_orient, name);
		iftDestroyImage(&g_mag);
		iftDestroyImage(&g_orient);
	
		secondStep_gradient_v2(img_v1, &g_mag, &g_orient);
		sprintf(name, "%s/2_v1_r3_mag_%s", "result", iftBasename(orig_dir->files[i]->pathname));
		iftWriteImageP2(g_mag, name);
		sprintf(name, "%s/2_v1_r3_orient_%s", "result", iftBasename(orig_dir->files[i]->pathname));
		iftWriteImageP2(g_orient, name);

		iftDestroyImage(&img);
		iftDestroyImage(&img_v1);
		iftDestroyImage(&img_v2);
		iftDestroyImage(&g_mag);
		iftDestroyImage(&g_orient);
	}
	MemDinFinal = iftMemoryUsed();
    if (MemDinInicial != MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial, MemDinFinal);

	return 0;
}
