#ifndef IFT_INTERPOLATION_H_
#define IFT_INTERPOLATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftColor.h"
#include "iftImage.h"
#include "iftFImage.h"
#include "iftMImage.h"
#include "iftDataSet.h"
#include "iftAdjacency.h"
#include "iftPlane.h"
#include "iftMatrix.h"
#include "iftSegmentation.h"
#include "iftRepresentation.h"
#include "iftMSPS.h"

// Returns the coordinate on the border of the image for the given
// dimension that is closest to the original coordinate
static inline int iftBorderInterpolateCoord(int dim_size, int p) {
  return (p < 0) ? 0 : ((p >= dim_size)? dim_size - 1 : p);
}

// Interpolates a voxel outside the float image's domain to a border
// voxel
static inline iftVoxel iftBorderInterpolateVoxel(iftImage *img, iftVoxel v) {
  v.x = iftBorderInterpolateCoord(img->xsize, v.x);
  v.y = iftBorderInterpolateCoord(img->ysize, v.y);
  v.z = iftBorderInterpolateCoord(img->zsize, v.z);

  return v;
}

// Interpolates a voxel outside the float image's domain to a border
// voxel
static inline iftVoxel iftFBorderInterpolateVoxel(iftFImage *img, iftVoxel v) {
  v.x = iftBorderInterpolateCoord(img->xsize, v.x);
  v.y = iftBorderInterpolateCoord(img->ysize, v.y);
  v.z = iftBorderInterpolateCoord(img->zsize, v.z);

  return v;
}

iftPlane *iftFindBestCutPlane(iftImage *weight, iftPoint pos, int xviewsize, int yviewsize);
iftPlane *iftFindBestObjectCutPlane(iftImage *obj, iftImage *weight);
iftImage  *iftResliceImage(iftImage *img, iftPlane *pl, int xsize, int ysize, int zsize);
iftImage  *iftGetSlice(iftImage *img, iftPlane *pl, int xsize, int ysize);
iftImage  *iftInterp2D(iftImage *img, float sx, float sy);
iftImage  *iftInterp(iftImage *img, float sx, float sy, float sz);
iftFImage  *iftFGetSlice(iftFImage *img, iftPlane *pl, int xsize, int ysize);
iftFImage *iftFInterp2D(iftFImage *img, float sx, float sy);
iftMImage *iftMInterp2D(iftMImage *mimg, float sx, float sy);
iftMImage *iftMInterp(iftMImage *img, float sx, float sy, float sz);
iftFImage *iftFInterp(iftFImage *img, float sx, float sy, float sz);
iftImage  *iftShapeBasedInterp2D(iftImage *label, float sx, float sy);
iftImage  *iftShapeBasedInterp(iftImage *label, float sx, float sy, float sz);
iftImage  *iftResliceOnPrincipalAxis(iftImage *img, iftImage *bin);

#ifdef __cplusplus
}
#endif

#endif

