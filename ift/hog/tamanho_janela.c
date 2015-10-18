#include "ift.h"

int main() {
		
	char labelPath[150];
	printf("please input: \t<labels folder>\n");
	scanf("%s", labelPath);
	iftDir *gt_dir = iftLoadFilesFromDirectory(labelPath, "pgm");
	int altMin = INFINITY_INT, altMax = 0, compMin = INFINITY_INT, compMax = 0;

	iftVoxel orig_pixel;
	iftImage *gt_img;

	int somaAlt = 0, somaComp = 0;
	int i;
	for(i=0; i<gt_dir->nfiles; i++) {
		int min_x = INFINITY_INT, max_x = 0, min_y = INFINITY_INT, max_y = 0;
		 gt_img = iftReadImageByExt(gt_dir->files[i]->pathname);

        for (int p = 0; p < gt_img->n ; p++) {
            if (gt_img->val[p] != 0) {               
                orig_pixel = iftGetVoxelCoord(gt_img, p);
                if(orig_pixel.x < min_x)
					min_x = orig_pixel.x;
				if(orig_pixel.x > max_x)
					max_x = orig_pixel.x;
					
				if(orig_pixel.y < min_y)
					min_y = orig_pixel.y;
				if(orig_pixel.y > max_y)
					max_y = orig_pixel.y;	
            }
        }
		int alt = max_y - min_y;
		int comp = max_x - min_x;
		if(comp == 154) {
			printf("%s %d\n", iftBasename(gt_dir->files[i]->pathname), alt);
		}
		somaAlt += alt;
		somaComp += comp;
if(alt != 66) {
		if(altMin > alt )
			altMin = alt;
		if(altMax < alt )
			altMax = alt;
}
if(comp != 154) {
		if(compMin > comp )
			compMin = comp;
		if(compMax < comp )
			compMax = comp;
}
	}
	printf("Altura mínima: %d\n", altMin);
	printf("Altura máxima: %d\n", altMax);
	printf("Altura média: %d\n\n", somaAlt/i);

	printf("Largura mínima: %d\n", compMin);
	printf("Largura máxima: %d\n", compMax);
	printf("Largura média: %d\n", somaComp/i);

	return 0;
}
