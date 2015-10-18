#include "hog.h"

int main() {
	int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

	iftImage *img = iftReadImageByExt("orig/0043.pgm");

	iftImage *img_v1 = firstStep_normalize_v1(img);
	iftWriteImageP2(img_v1, "result/1_0043_v1_r5_L4095.pgm");
	
	iftImage *img_v2 = firstStep_normalize_v2(img);
	iftWriteImageP2(img_v2, "result/1_0043_v2_r5_K2000.pgm");

	iftImage *g_mag;
	iftImage *g_orient;
	secondStep_gradient_v2(img_v2, &g_mag, &g_orient);
	iftWriteImageP2(g_mag, "result/2_0043_v2_r3_mag.pgm");
	iftWriteImageP2(g_orient, "result/2_0043_v2_r3_orient.pgm");
	iftDestroyImage(&g_mag);
	iftDestroyImage(&g_orient);
	
	secondStep_gradient_v2(img_v1, &g_mag, &g_orient);
	iftWriteImageP2(g_mag, "result/2_0043_v1_r3_mag.pgm");
	iftWriteImageP2(g_orient, "result/2_0043_v1_r3_orient.pgm");

    iftDestroyImage(&img);
    iftDestroyImage(&img_v1);
	iftDestroyImage(&img_v2);
	iftDestroyImage(&g_mag);
	iftDestroyImage(&g_orient);

	MemDinFinal = iftMemoryUsed();
    if (MemDinInicial != MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial, MemDinFinal);

	return 0;
}
