#ifndef IFT_IMAGEMATH_H_
#define IFT_IMAGEMATH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftImage.h"
#include "iftFImage.h"

  iftImage  *iftAdd(iftImage *img1, iftImage *img2);
  iftImage  *iftSub(iftImage *img1, iftImage *img2);
  iftImage  *iftAnd(iftImage *img1, iftImage *img2);
  iftImage  *iftOr(iftImage *img1, iftImage *img2);
  iftImage  *iftMult(iftImage *img1, iftImage *img2);
  iftImage  *iftAbs(iftImage *img);
  iftImage  *iftComplement(iftImage *img);
  iftImage  *iftMask(iftImage *img, iftImage *bin);
  iftImage  *iftAddValue(iftImage *img, int value);
  iftImage  *iftAddValueInRegion(iftImage *img, int value, iftImage *mask);
  iftFImage *iftSQRT(iftImage *img1);

#ifdef __cplusplus
}
#endif

#endif
