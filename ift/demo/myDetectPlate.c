#include "ift.h"
#include "hog.h"

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

int countNumPixelsCandidatesTotal(iftImage *candImg) {
	int num = 0;
	for(int i=0; i<candImg->xsize; i+=1) {
	for(int j=0; j<candImg->ysize; j+=1) {
		int p = i + j*candImg->xsize;
	//for (int p = 0; p < candImg->n; ++p) {
        if (candImg->val[p] > 0) {
            num++;
		}
    }}
    return num;	
}

int main(int argc, char* argv []) {

	char inputPath[100], labelPath[100], candPath[100], classifierPath[100], outputPath[100];
	
	printf("please input: \t<images folder>\n\t\t<labels folder>\n\t\t<candidates folder>\n\t\t<classifier>\n\t\t<output detect folder>\n");
	scanf("%s", inputPath);
	scanf("%s", labelPath);
	scanf("%s", candPath);
	scanf("%s", classifierPath);
	scanf("%s", outputPath);
	
	int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();
        
    iftSVM* svm = iftReadSVM(classifierPath);
	
    iftDir* imgsDir = iftLoadFilesFromDirectory(inputPath, "pgm");
    iftDir* labelsDir = iftLoadFilesFromDirectory(labelPath, "pgm");
	iftDir* candsDir = iftLoadFilesFromDirectory(candPath, "pgm");
	
	if(imgsDir->nfiles!=labelsDir->nfiles ||imgsDir->nfiles!=candsDir->nfiles) {
        iftError("Different number of images and/or labels and/or candidates.", argv[0]);
    }

    iftImage* candImg;
    iftImage* origImg;
    iftImage* labelImg;
    iftImage* maskImg;
    
    iftImage* normImg;
    iftImage* magImg;
    iftImage* orientImg;

    iftDataSet* Zt;

    float meanAcc = 0.0f;
	float minAcc = 100.0;
	
	int DESCRIPTOR_SIZE = (HOG_N1/HOG_N2-HOG_N3+1)*(HOG_M1/HOG_M2-HOG_M3+1)*HOG_N3*HOG_M3*9;

    for (int i = 0; i <imgsDir->nfiles; ++i) {

        printf("Image: %s ", imgsDir->files[i]->pathname);

        labelImg = iftReadImageByExt(labelsDir->files[i]->pathname);
        origImg = iftReadImageByExt(imgsDir->files[i]->pathname);
        candImg = iftReadImageByExt(candsDir->files[i]->pathname);
        
        normImg = normalize(origImg);
        gradient(normImg, &magImg, &orientImg);

        int numCandidates = countNumPixelsCandidatesTotal(candImg);
        Zt = iftCreateDataSet(numCandidates, DESCRIPTOR_SIZE);
        iftSetStatus(Zt, TEST);

		int j=0;
		for (int p = 0; p < origImg->n; ++p) {
			if(candImg->val[p] > 0) {
				iftVoxel v = iftGetVoxelCoord(candImg, p); 
				iftFeatures* feat = hog(magImg, orientImg, v.x, v.y);
				for (int t = 0; t < feat->n ; t++) {
					Zt->sample[j].feat[t] = feat->val[t];
					Zt->sample[j].id = p;
				}
				iftDestroyFeatures(&feat);
				j++;
			}
		}
       
        iftSVMClassifyOVO(svm, Zt, TEST);

        float acc = 0.0f;
        int numPixels = 0, numPixelsOut = 0, accNo = 0;
        
         maskImg = iftCopyImage(origImg);

        j=0;
        for (int p = 0; p < candImg->n; ++p) {
			if(candImg->val[p] > 0) {
				if(Zt->sample[j].label == 1) {					
					if (labelImg->val[p] > 0) {
						//acc+=1.0;
                  continue;
					} else {
						//accNo++;
                  continue;
					}
					//numPixelsOut++;
               continue;
				} else {
					maskImg->val[p] = 0;
				}
				j++;
			} else {
				maskImg->val[p] = 0;
			}
			
			/*if (labelImg->val[p] > 0) {
                numPixels++;
            }*/
        }

         maskImg = selectCandidates(maskImg);

        j=0;
        for (int p = 0; p < candImg->n; ++p) {
			if(candImg->val[p] > 0) {
				if(maskImg->val[p] >= 1) {					
					if (labelImg->val[p] > 0) {
						acc+=1.0;
					} else {
						accNo++;
					}
					numPixelsOut++;
				} else {
					origImg->val[p] = 0;
               continue;
				}
				j++;
			} else {
				origImg->val[p] = 0;
            continue;
			}
			
			if (labelImg->val[p] > 0) {
                numPixels++;
            }
        }
        

		acc/= numPixels;
		if(acc <  minAcc) 
			minAcc = acc;
        printf("Detection precision: %4.2f - %4.2f\n", acc, (float)accNo/numPixelsOut);

        meanAcc += acc/imgsDir->nfiles;
        
        char* detectedfile = iftJoinPathnames(outputPath, iftBasename(imgsDir->files[i]->pathname));

        iftWriteImageP5(origImg, detectedfile);

        iftDestroyDataSet(&Zt);
        iftDestroyImage(&candImg);
        iftDestroyImage(&origImg);
        iftDestroyImage(&labelImg);
		iftDestroyImage(&normImg);
        iftDestroyImage(&magImg);
        iftDestroyImage(&orientImg);
    }

	iftDestroyDir(&imgsDir);
    iftDestroyDir(&labelsDir);
    iftDestroySVM(svm);

    printf("\n\nMean Detection Precision: %4.2f\n", meanAcc);
    printf("%4.2f\n", minAcc);
    
    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial!=MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial,MemDinFinal);
               
    return 0;

}
