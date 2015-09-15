/**
 * @file iftSimilarity.h
 * @brief Functions to compute Similarities and Distances between images.
 *
 * @author Samuel Martins
 */

#ifndef _IFT_SIMILARITY_H_
#define _IFT_SIMILARITY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftAdjacency.h"
#include "iftImage.h"
#include "iftFImage.h"
#include "iftRepresentation.h"
#include "iftObjectModels.h"
#include "iftSegmentation.h"

typedef float (*iftImageSimilarityFunction) (iftImage *baseImage, iftImage *auxImage);


/**
 * @brief Computes the Similarity between two Binary Images using Dice.
 *
 * @author Samuel Martins
 * @date September 01, 2015
 *
 * Computes the Similarity between two Binary Images using Dice.
 * The binary image must have only values 1.
 *
 * @param bin1 First Binary Image.
 * @param bin2 Second Binary Image.
 * @return The dice similarity between bin1 and bin2.
 */
float 		iftSimilarityByDice(iftImage *bin1, iftImage *bin2);
float 		iftASSD(iftImage *baseImage, iftImage *auxImage);
iftMatrix 	*iftSimilarityMatrix(fileList *imageFiles, iftImageSimilarityFunction similarityFunction, float main_diagonal);
void 		iftWriteSimilarityMatrix(iftMatrix *result, fileList *imageFiles, char *out, int maximize);
double          iftShannonEntropy(iftImage *image);
double          iftJointEntropy(iftImage *image1, iftImage *image2);
double          iftNormalizedMutualInformation(iftImage *image1, iftImage *image2);
float           iftMeanDistFromSourceToTarget(iftImage *bin_source, iftImage * bin_target);
float           iftMeanDistBetweenBoundaries(iftImage *bin1, iftImage * bin2);

#ifdef __cplusplus
}
#endif

#endif


