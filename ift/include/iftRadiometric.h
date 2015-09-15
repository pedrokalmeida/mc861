#ifndef IFT_RADIOMETRIC_H_
#define IFT_RADIOMETRIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftImage.h"
#include "iftFImage.h"
#include "iftColor.h"
#include "iftGQueue.h"
#include "iftHistogram.h"

/* Radiometric Transformations */

iftImage *iftLinearStretch(iftImage *img, float f1, float f2, float g1, float g2);
iftFImage *iftFLinearStretch(iftFImage *img, float f1, float f2, float g1, float g2);
iftImage *iftNormalize(iftImage *img, float minval, float maxval);
iftImage *iftNormalizeWithNoOutliers(iftImage *img, int minval, int maxval);
iftImage *iftWindowAndLevel(iftImage *img, int width, int level, int maxval);
iftImage *iftGaussianStretch(iftImage *img, float mean, float stdev, int maxval);
iftImage *iftExponenStretch(iftImage *img, float f1, float f2, float g1, float g2);
iftImage *iftEqualize(iftImage *img, int max_val);
iftImage *iftMatchHistogram(iftImage *img, iftImage *ref);

/* Radiometric Resolution */
int 	  iftRadiometricResolution(iftImage *img);


#ifdef __cplusplus
}
#endif

#endif
