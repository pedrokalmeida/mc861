#include "hog.h"
#include "iftSelectCandidates.h"

int calcula_qtd_descritores(iftImage *img_candidates, int numCandidates){	
	int qtd_descritores = 0;
	for(int i=0; i<numCandidates; i++) {
		iftImage *bb_mag;
		iftImage *bb_orient;
		iftCreateBoundingBox2D(img_candidates, (i+1), img_candidates, img_candidates, &bb_mag, &bb_orient);
		qtd_descritores += ((bb_mag->xsize - HOG_N1)*(bb_mag->ysize - HOG_M1));
		iftDestroyImage(&bb_mag);
		iftDestroyImage(&bb_orient);
	}
	return qtd_descritores;
}

int main(int argc, char *argv[]) {

	iftDir *orig_dir, *gt_dir, *cand_dir;	
	iftImage *orig_img, *gt_img, *cand_img;
    iftDataSet *dataset;
    iftFeatures *feat;
	int f, i, j, k, l, p, origp; 
	int index_cand, numpixels_gt, numpixels_cand;
	int num_candidates, total_candidates, descriptor_size;
	iftVoxel orig_pixel;
	
	int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

	if (argc!=5){
        fprintf(stdout,"Usage: myExtractFeatures <dir_orig_images> <dir_gt_detection> <dir_candidates> <opf_dataset>\n");
        fprintf(stdout,"       dir_orig_images:        directory of original images\n");
        fprintf(stdout,"       dir_gt_detection:       directory of trainig images (binary images)\n");
        fprintf(stdout,"       dir_candidates:         directory of candidate images (gray-scale images)\n");
        fprintf(stdout,"       opf_dataset   :         dataset of candidates (positive and negative samnples)\n");
        exit(1);
    }
    
    orig_dir = iftLoadFilesFromDirectory(argv[1], "pgm");
    gt_dir = iftLoadFilesFromDirectory(argv[2], "pgm");
    cand_dir = iftLoadFilesFromDirectory(argv[3], "pgm");

    descriptor_size = (HOG_N1/HOG_N2-HOG_N3+1)*(HOG_M1/HOG_M2-HOG_M3+1)*HOG_N3*HOG_M3*9;
	total_candidates = 0;
    for (i = 0; i < gt_dir->nfiles ; ++i) {
        // Read candidate images
        cand_img = iftReadImageByExt(cand_dir->files[i]->pathname);
        num_candidates = iftMaximumValue(cand_img);
        total_candidates += calcula_qtd_descritores(cand_img, num_candidates);
        iftDestroyImage(&cand_img);
    }
    dataset = iftCreateDataSet(total_candidates, descriptor_size);
    dataset->nclasses = 2;

    index_cand = 0;
    for (l = 0; l < gt_dir->nfiles ; l++) {
        // Read gt and candidate images
        gt_img = iftReadImageByExt(gt_dir->files[l]->pathname);
        cand_img = iftReadImageByExt(cand_dir->files[l]->pathname);
        orig_img = iftReadImageByExt(orig_dir->files[l]->pathname);
        
        numpixels_gt = 0;
        for (p = 0; p < gt_img->n ; p++) {
            if (gt_img->val[p] != 0) {
                numpixels_gt++;
            }
        }

		num_candidates = iftMaximumValue(cand_img);
		
		 /* -----------------------HOG------------------------------- */
		iftImage *img_normalized = firstStep_normalize_v2(orig_img);
		iftDestroyImage(&orig_img);
		
		iftImage *img_mag;
		iftImage *img_orient;
		secondStep_gradient_v2(img_normalized, &img_mag, &img_orient);
		iftDestroyImage(&img_normalized);
	 
		iftImage **g_mag = malloc(sizeof(iftImage) * num_candidates); //ou sizeof(*iftImage) ou sizeof(iftImage*)
		iftImage **g_orient = malloc(sizeof(iftImage) * num_candidates); 
		iftVoxel *pixel_ref = malloc(sizeof(iftVoxel) * num_candidates);
		
		int qtd_descritores = 0;
        for (k = 0; k < num_candidates ; k++) {		
			pixel_ref[k] = iftCreateBoundingBox2D(cand_img, (k+1), img_mag, img_orient, &g_mag[k], &g_orient[k]);
			qtd_descritores += ((g_mag[k]->xsize - HOG_N1)*(g_mag[k]->ysize - HOG_M1));
		}
		iftDestroyImage(&cand_img);
		iftDestroyImage(&img_mag);
		iftDestroyImage(&img_orient);

		for (k = 0; k < num_candidates ; k++) {		
			for(i=0; i < g_mag[k]->xsize-HOG_N1; i++) {
				for(j=0; j<g_mag[k]->ysize-HOG_M1; j++) {
					//Obtém vetor de características
					feat = thirdStep_histogram_v2(g_mag[k], g_orient[k], i, j);
					for(f=0; f<feat->n; f++) {
						dataset->sample[index_cand].feat[f] = feat->val[f];
					}				
					iftDestroyFeatures(&feat);
					//Conta quantos pixel pertencem à placa
					numpixels_cand = 0;
					for(int x=0; x<HOG_N1; x++){
						for(int y=0; y<HOG_M1; y++){
							orig_pixel.x = pixel_ref[k].x + x;
							orig_pixel.y = pixel_ref[k].y + y;
							origp = iftGetVoxelIndex(gt_img, orig_pixel);
							if(gt_img->val[origp] != 0) {
								numpixels_cand++;
							}
						}
					}
					if (numpixels_cand > (7*numpixels_gt/10)) {
						dataset->sample[index_cand].truelabel = 1; // positive
					} else {
						dataset->sample[index_cand].truelabel = 2; // negative
					}
					index_cand++;
				}
			}
			iftDestroyImage(&g_mag[k]);
			iftDestroyImage(&g_orient[k]);
		}
		free(pixel_ref);
		free(g_mag);
		free(g_orient);
		iftDestroyImage(&gt_img);
	}

    // Write candidates dataset
    iftWriteOPFDataSet(dataset, argv[4]);
    
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
