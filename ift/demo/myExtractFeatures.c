#include "hog.h"

int main(int argc, char *argv[]) {

	char inputPath[100], labelPath[100], candidatePath[100], datasetPath[100];
	printf("please input: \t<images folder>\n\t\t<labels folder>\n\t\t<candidate folder>\n\t\t<dataset>\n");
	scanf("%s", inputPath);
	scanf("%s", labelPath);
	scanf("%s", candidatePath);
	scanf("%s", datasetPath);
	
	int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

	iftDir *orig_dir, *gt_dir, *cand_dir;	
    orig_dir = iftLoadFilesFromDirectory(inputPath, "pgm");
    gt_dir = iftLoadFilesFromDirectory(labelPath, "pgm");
    cand_dir = iftLoadFilesFromDirectory(candidatePath, "pgm");

	
	iftImage *orig_img, *gt_img, *cand_img;
    iftDataSet *dataset;
    iftFeatures *feat;
	
	int descriptor_size = (HOG_N1/HOG_N2-HOG_N3+1)*(HOG_M1/HOG_M2-HOG_M3+1)*HOG_N3*HOG_M3*9;
	int total_candidates = 0;
    for (int i = 0; i < gt_dir->nfiles ; ++i) {
        // Read candidate images
        cand_img = iftReadImageByExt(cand_dir->files[i]->pathname);
        total_candidates += countNumPixelsCandidates(cand_img);
        iftDestroyImage(&cand_img);
    }
    printf("%d caractericas x %d descritores = %d \n", descriptor_size, total_candidates, descriptor_size*total_candidates);

    dataset = iftCreateDataSet(total_candidates, descriptor_size);
    printf("%d\n", dataset->nsamples);
    dataset->nclasses = 2;
    
	timer *t1 = NULL, *t2 = NULL;

	int numpixels_gt, numpixels_cand;
    iftVoxel orig_pixel;
    int index_cand = 0;
    for (int l = 0; l < gt_dir->nfiles ; l++) {
		t1 = iftTic();
        // Read gt and candidate images
        gt_img = iftReadImageByExt(gt_dir->files[l]->pathname);
        cand_img = iftReadImageByExt(cand_dir->files[l]->pathname);
        orig_img = iftReadImageByExt(orig_dir->files[l]->pathname);
        
        
        //coordenadas definindo caixa contendo pixels identificados como placa
        int min_x = INFINITY_INT, max_x = 0, min_y = INFINITY_INT, max_y = 0;
        
        numpixels_gt = 0;
        for (int p = 0; p < gt_img->n ; p++) {
            if (gt_img->val[p] != 0) {
                numpixels_gt++;
                
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
		 // -----------------------HOG------------------------------- //
		iftImage *img_normalized = normalize(orig_img);
		
		iftImage *img_mag;
		iftImage *img_orient;
		gradient(img_normalized, &img_mag, &img_orient);
		iftDestroyImage(&img_normalized);
		
		//for (int p = 0; p < gt_img->n ; p++) {
		for(int i=0; i<gt_img->xsize; i+=3) {
		for(int j=0; j<gt_img->ysize; j+=3) {
			int p = i + j*gt_img->xsize;
			if (cand_img->val[p] != 0) {
				orig_pixel = iftGetVoxelCoord(cand_img, p);
				feat = hog(img_mag, img_orient, orig_pixel.x, orig_pixel.y);
				for(int f=0; f<feat->n; f++) {
					dataset->sample[index_cand].feat[f] = feat->val[f];
				}				
				iftDestroyFeatures(&feat);
				
				int x0 = orig_pixel.x - HOG_N1/2;
				int xn = x0 + HOG_N1;
				int y0 = orig_pixel.y - HOG_M1/2;
				int yn = y0 + HOG_M1;
				/*if(x0 <= min_x && xn >= max_x && y0 <= min_y && yn >= max_y) {
					//if(gt_img->val[p] != 0) {
						dataset->sample[index_cand].truelabel = 1;
					//} else {
					//	dataset->sample[index_cand].truelabel = 2;
					//}
				} else {
					dataset->sample[index_cand].truelabel = 2;
				}*/
				
				// QUAL MELHOR MANEIRA DE DEFINIR QUAL VETORES DE CARACTERÍSTICAS IDENTIFICAM PLACA????
				if(gt_img->val[p] != 0) {
					dataset->sample[index_cand].truelabel = 1;//pertence à placa
				} else {
					dataset->sample[index_cand].truelabel = 2;//não pertence à placa
				}
				index_cand++;
			}
		}}
		
		t2 = iftToc();
		printf("%s - ", iftBasename(gt_dir->files[l]->pathname));
		printf("%dth image: extract features in %f ms\n", l+1, iftCompTime(t1, t2));
		
		iftDestroyImage(&img_mag);
		iftDestroyImage(&img_orient);

		iftDestroyImage(&gt_img);
		iftDestroyImage(&orig_img);
		iftDestroyImage(&cand_img);
	}  

    // Write candidates dataset
    iftWriteOPFDataSet(dataset, datasetPath);
    
    // Free
    iftDestroyDataSet(&dataset);
    iftDestroyDir(&gt_dir);
    iftDestroyDir(&orig_dir);
    iftDestroyDir(&cand_dir);
    /* ---------------------------------------------------------- */

    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial!=MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial,MemDinFinal);

    return(0);
}
