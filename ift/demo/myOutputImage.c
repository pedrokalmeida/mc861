#include "ift.h"
#include "iftSelectCandidates.h"

int main(int argc, char* argv []) {

    char origPath[100], inputPath[100], labelPath[100], candPath[100], outputPath[100];
    
    printf("please input: \t<images folder>\n\t\t<images input folder>\n\t\t<labels folder>\n\t\t<candidates folder>\n\t\t<output detect folder>\n");
    scanf("%s", origPath);
    scanf("%s", inputPath);
    scanf("%s", labelPath);
    scanf("%s", candPath);
    scanf("%s", outputPath);
    
    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();
    
    iftDir* origDir = iftLoadFilesFromDirectory(origPath, "pgm");
    iftDir* imgsDir = iftLoadFilesFromDirectory(inputPath, "pgm");
    iftDir* labelsDir = iftLoadFilesFromDirectory(labelPath, "pgm");
    iftDir* candDir = iftLoadFilesFromDirectory(candPath, "pgm");

    iftImage* origImg;
    iftImage* labelImg;
    iftImage* candImg;        
    iftImage* inputImg;
    iftImage* outputImg;
    
    float meanAcc = 0.0f;
    float meanAccRegion = 0.0f;
    float minAcc = 100.0;

    int falhas = 0;
    
    for (int i = 0; i <imgsDir->nfiles; ++i) {

        printf("Image: %s ", imgsDir->files[i]->pathname);
        
        origImg = iftReadImageByExt(origDir->files[i]->pathname);
        labelImg = iftReadImageByExt(labelsDir->files[i]->pathname);
        candImg = iftReadImageByExt(candDir->files[i]->pathname);
        inputImg = iftReadImageByExt(imgsDir->files[i]->pathname);
        
        outputImg = iftCopyImage(origImg);
        
        int numPixelsPlate = 0;
        int num_candidates = iftMaximumValue(candImg);
        int numPixelsByCandidate[] =  {0,0,0,0,0,0,0,0,0,0};
        for (int p = 0; p < inputImg->n; ++p) {
            if(inputImg->val[p] > 0) {        
                numPixelsByCandidate[candImg->val[p]-1]++;
            }
            
            if (labelImg->val[p] > 0) {
                numPixelsPlate++;
            }
        }
        int labelPlate = -1;
        int maxNumPixelByCandidate = 0;
        for(int k=0; k<num_candidates; k++) {
            if(numPixelsByCandidate[k] > maxNumPixelByCandidate) {
                labelPlate = k+1;
                maxNumPixelByCandidate = numPixelsByCandidate[k];
            }
        }

        for (int p = 0; p < inputImg->n; ++p) {
            if(inputImg->val[p] > 0 && candImg->val[p] == labelPlate) {        
                outputImg->val[p] = origImg->val[p] + 1;
            } else {
                outputImg->val[p] = 0;
            }
        }

/*
         iftImage *aux;
         aux = outputImg;
         outputImg = selectCandidates(outputImg);
         iftDestroyImage(&aux);
*/

        //coordenadas definindo caixa contendo pixels identificados como placa
        int min_x = INFINITY_INT, max_x = 0, min_y = INFINITY_INT, max_y = 0;
        int min_x_label = INFINITY_INT, max_x_label = 0, min_y_label = INFINITY_INT, max_y_label = 0;
        iftVoxel orig_pixel;
        for (int p = 0; p < outputImg->n ; p++) {
            if (outputImg->val[p] != 0) {                
                orig_pixel = iftGetVoxelCoord(outputImg, p);
                if(orig_pixel.x < min_x)
                    min_x = orig_pixel.x;
                if(orig_pixel.x > max_x)
                    max_x = orig_pixel.x;
                    
                if(orig_pixel.y < min_y)
                    min_y = orig_pixel.y;
                if(orig_pixel.y > max_y)
                    max_y = orig_pixel.y;
            }    
            if (labelImg->val[p] != 0) {                
                orig_pixel = iftGetVoxelCoord(labelImg, p);
                if(orig_pixel.x < min_x_label)
                    min_x_label = orig_pixel.x;
                if(orig_pixel.x > max_x_label)
                    max_x_label = orig_pixel.x;
                    
                if(orig_pixel.y < min_y_label)
                    min_y_label = orig_pixel.y;
                if(orig_pixel.y > max_y_label)
                    max_y_label = orig_pixel.y;    
            }
        }    
        
        //min_x -= 5;
        //max_x += 5;
        min_y -= 5;
        max_y += 5;
        
        if(min_x < 0) 
            min_x = 0;
        if(min_y < 0)
            min_y = 0;
        if(max_x > outputImg->xsize) 
            max_x = outputImg->xsize;
        if(max_y > outputImg->ysize) 
            max_y = outputImg->ysize;


        float acc = 0;
        float pixelOut = 0;
        for(int x=min_x; x<max_x; x++) {
        for(int y=min_y; y<max_y; y++) {
            int p = x + y*outputImg->xsize;
            outputImg->val[p] = origImg->val[p];
            if(labelImg->val[p] > 0) 
                acc+= 1.0;
            else {
                if(x < min_x_label || x > max_x_label || y < min_y_label || y > max_y_label)
                    pixelOut+=1.0;
            }
        }}

        acc /= numPixelsPlate;
        float acc_region = (float)pixelOut/((max_x-min_x)*(max_y-min_y));
        printf("Detection precision: %4.2f - %4.2f\n", acc, acc_region);
           if(acc <  minAcc) 
            minAcc = acc;

        meanAcc += acc;
        //meanAccRegion += acc_region;

        if (acc < 0.7)
            falhas++;
        char* detectedfile = iftJoinPathnames(outputPath, iftBasename(imgsDir->files[i]->pathname));

        iftWriteImageP5(outputImg, detectedfile);
       

        iftDestroyImage(&origImg);
        iftDestroyImage(&labelImg);
        iftDestroyImage(&candImg);
        iftDestroyImage(&outputImg);
        iftDestroyImage(&inputImg);
    }
    
    //printf("\n\nMean Detection Precision: %4.2f - %4.2f\n", (float)meanAcc/imgsDir->nfiles, (float)meanAccRegion/imgsDir->nfiles);
    printf("\n\nMean Detection Precision: %4.2f\n", (float)meanAcc/imgsDir->nfiles);
    printf("%4.2f\n", minAcc);
    printf("%d placas não detectadas\n", falhas);

    iftDestroyDir(&imgsDir);
    iftDestroyDir(&labelsDir);
    iftDestroyDir(&origDir);
    iftDestroyDir(&candDir);

    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial!=MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial,MemDinFinal);
               
    return 0;

}
