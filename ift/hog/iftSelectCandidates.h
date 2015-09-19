//
// Created by Alan Peixinho on 8/31/15.
//

#ifndef IFT_IFTSELECTCANDIDATES_H
#define IFT_IFTSELECTCANDIDATES_H

#define MIN_VOLUME 1000

/**
 * @brief Remove components with less than minVolume pixels.
 */
void iftRemoveSmallComponents(iftImage *img, int minVolume) {

    int p, i;
    int max = iftMaximumValue(img);
    int *volume = iftAllocIntArray(max + 1);
    int *labels = iftAllocIntArray(max + 1);

    for (p = 0; p < img->n; ++p) {
        if (img->val[p] > 0)
            volume[img->val[p]]++;
    }

    int nlabels = 1;
    for (i = 1; i <= max; ++i) {
        if (volume[i] >= minVolume)
            labels[i] = nlabels++;
        else
            labels[i] = 0;
    }

    for (p = 0; p < img->n; ++p) {
        img->val[p] = labels[img->val[p]];
    }

    free(labels);
    free(volume);
}

iftImage *selectCandidates(iftImage *orig) {

    iftImage *aux[4];
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


#endif //IFT_IFTSELECTCANDIDATES_H
