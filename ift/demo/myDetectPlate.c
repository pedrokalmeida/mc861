#include "ift.h"
#include "hog.h"
#include "iftSelectCandidates.h"

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
        int numPixelsPlate = 0, numPixels = 0, accNo = 0;
        
        maskImg = iftCopyImage(origImg);

		
        j=0;
        for (int p = 0; p < candImg->n; ++p) {
			if(candImg->val[p] > 0) {
				if(Zt->sample[j].label == 1) {	
					maskImg->val[p] = origImg->val[p] + 1;				
					if (labelImg->val[p] > 0) {
						acc+=1.0;
					} else {
						accNo++;
					}
					numPixels++;
				} else {
					maskImg->val[p] = 0;
				}
				j++;
			} else {
				maskImg->val[p] = 0;
			}
			
			if (labelImg->val[p] > 0) {
                numPixelsPlate++;
            }
        }
		
		acc/= numPixelsPlate;
		if(acc <  minAcc) 
			minAcc = acc;
        printf("Detection precision: %4.2f - %4.2f\n", acc, (float)accNo/numPixels);

        meanAcc += acc/imgsDir->nfiles;
        
        char* detectedfile = iftJoinPathnames(outputPath, iftBasename(imgsDir->files[i]->pathname));

//        iftWriteImageP5(origImg, detectedfile);
		iftWriteImageP5(maskImg, detectedfile);
       
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
