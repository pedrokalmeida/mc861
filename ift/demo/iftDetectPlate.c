#include "ift.h"
#include "iftExtractFeatures.h"
#include "iftSelectCandidates.h"


int main(int argc, char* argv []) {

    if(argc<5) {
        iftError("Usage: <images folder> <labels folder> <classifier> <output detect folder>", argv[0]);
    }

    iftSVM* svm = iftReadSVM(argv[3]);

    iftDir* imgsDir = iftLoadFilesFromDirectory(argv[1], "pgm");
    iftDir* labelsDir = iftLoadFilesFromDirectory(argv[2], "pgm");

    if(imgsDir->nfiles!=labelsDir->nfiles) {
        iftError("Different number of images and labels.", argv[0]);
    }

    iftImage* candImg;
    iftImage* origImg;
    iftImage* labelImg;

    iftDataSet* Zt;

    float meanAcc = 0.0f;

    for (int i = 0; i <imgsDir->nfiles; ++i) {

        printf("Image: %s ", imgsDir->files[i]->pathname);

        labelImg = iftReadImageByExt(labelsDir->files[i]->pathname);
        origImg = iftReadImageByExt(imgsDir->files[i]->pathname);
        candImg = selectCandidates(origImg);

        int numCandidates = iftMaximumValue(candImg);
        Zt = iftCreateDataSet(numCandidates, DESCRIPTOR_SIZE);
        iftSetStatus(Zt, TEST);

        // Select positive and negative examples
        for (int j = 0; j < numCandidates ; j++) {
            // Get bounding box
            iftImage* bb_img = iftCreateBoundingBox2D(origImg, candImg, (j+1));
            // Extract BIC descriptor
            iftFeatures* feat = iftExtractBIC(bb_img, DESCRIPTOR_SIZE/2);
            for (int t = 0; t < feat->n ; t++) {
                Zt->sample[j].feat[t] = feat->val[t];
                Zt->sample[j].id = j+1;
            }

            // Write image name and candidate number
            //fprintf(f,"%s %d\n", orig_dir->files[i]->pathname, (j+1));
            // Increment candidate index
            iftDestroyImage(&bb_img);
            iftDestroyFeatures(&feat);
        }

        iftSVMClassifyOVO(svm, Zt, TEST);

        float acc = 0.0f;
        int numPixels = 0;
        
        for (int p = 0; p < candImg->n; ++p) {
            int candidate = candImg->val[p];
            //check if the pixel was marked as plate with the automatic method
            int isPlate;
            if(candidate>0) isPlate = Zt->sample[candidate-1].label==1;
            else isPlate = 0;

            if(!isPlate) {
                origImg->val[p] = 0;
            }

            if (labelImg->val[p] > 0) {
                numPixels++;
                if(labelImg->val[p] == isPlate) {
                    acc+=1.0;
                }
            }

            candImg->val[p] = isPlate;
        }

        char* detectedfile = iftJoinPathnames(argv[4], iftBasename(imgsDir->files[i]->pathname));


        iftWriteImageP5(origImg, detectedfile);

        iftDestroyDataSet(&Zt);
        iftDestroyImage(&candImg);
        iftDestroyImage(&origImg);
        iftDestroyImage(&labelImg);

        acc/= numPixels;

        printf("Detection precision: %4.2f\n", acc);

        meanAcc += acc/imgsDir->nfiles;

    }

    printf("\n\nMean Detection Precision: %4.2f\n", meanAcc);

}