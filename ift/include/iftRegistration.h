/** @file
    @brief Image Registration module
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IFT_REGISTRATIONMSPS_H_
#define IFT_REGISTRATIONMSPS_H_

#include "iftCommon.h"
#include "iftAdjacency.h"
#include "iftImage.h"
#include "iftFImage.h"
#include "iftRepresentation.h"
#include "iftGeometric.h"
#include "iftObjectModels.h"

/**
 * @addtogroup Image
 * @{
 */

/** @defgroup ImageRegistration
 *  @{
 *
 *  @brief Image registration utilities
 *
 */

void             iftRegisterImageElastix(char *fixedImagePath, char *movingImagePath, char *paramsAffine, char *paramsBSpline, char *outBasename);
void             iftRegisterDirectoryElastix(char *fixedImagePath, char *movingImagesDir, char *paramFileAffine, char *paramFileBSpline, char *outImagesDir, char *outMasksDir);
void             iftDeformMaskTransformix(char *binaryMaskPath, char *paramsBSpline, char *outPath);
void             iftDeformDirectoryTransformix(char *masksDirectory, char *outDirectory);
void             iftAlignImagesForSimilarity(char *dirIn, char *dirOut);


/**
 * @brief Register a cloud point in order to maximize the given score image.
 *
 * Given an array of points this function tries to apply affine transformations in order to better fit these points maximizing the score image.
 * The score image can be computed as the Inverted Euclidean Distance Transform as in iftEuclideanScoreImage(), or any other maximization score.
 *
 * @author Peixinho
 *
 * @date 12 Aug 2015
 *
 **/
iftMatrix* iftShapeRegister(iftPoint* orig, int norig, iftImage* score);


/**
 * @brief Computes the inverse of Euclidean Distance Transform in a border Image.
 *
 * @param img The input binary border image.
 * @param decay The decay factor for the score, larger decay factors creates a score image that penalizes points too far from the border.
 *
 * @author Peixinho
 * @date 12 Aug 2015
 */
iftImage* iftEuclideanScoreImage(iftImage* img, float decay);
iftMatrix *iftShapeTransform(iftPoint *orig, int norig, float rx, float rz, iftImage* score);

/** @}*/

/** @}*/

#endif

#ifdef __cplusplus
}
#endif

