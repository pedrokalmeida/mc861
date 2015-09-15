#ifndef IFT_REPRESENTATION_H_
#define IFT_REPRESENTATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftAdjacency.h"
#include "iftGQueue.h"
#include "iftImage.h"
#include "iftRadiometric.h"
#include "iftHistogram.h"
#include "iftFImage.h"
#include "iftSeeds.h"

  iftImage       *iftDistTrans(iftImage *bin, iftAdjRel *A, char side);
  iftImage       *iftBorderDistTrans(iftImage *label, iftAdjRel *A);
  iftImage       *iftShellDistTrans(iftImage *bin, iftAdjRel *A, char side, float max_dist);
  void            iftDistTransRootMap(iftImage *bin, iftAdjRel *A, char side, iftImage **dist, iftImage **root);
  iftFImage      *iftSignedDistTrans(iftImage *bin, iftAdjRel *A);
  iftFImage      *iftShellSignedDistTrans(iftImage *bin, iftAdjRel *A,float max_dist);
  iftFImage      *iftMSSkel(iftImage *bin); /* multiscale skeletons by geodesic length */

  /* compute surface skeleton by thresholding the geodesic skeleton
     and then select a given number of components */

  iftImage *iftSurfaceSkeleton(iftFImage *skel, float thres, int number_of_components); 

  // Computes a multiscale skeleton by collision angle. It disconnects
  // the surface skeleton from points nearby the boundary by a
  // distance threshold (e.g. dist_thres=16) 

  iftFImage       *iftMSSkelByAngle(iftImage *bin, float dist_thres);

  // threshold the multiscale skeleton by angle and select a number of connected components 

  iftImage *iftSurfaceSkeletonByAngle(iftFImage *skel_angle, float angle_thres, int number_of_components);

  // This function computes the multiscale skeleton and returns the distance transformed
  // used for computing it. Both maps can be used to compute a Medial Axis Transform.
  iftFImage 	 *iftMSSkel2DDistMap(iftImage *bin, iftAdjRel *A, char side, iftImage **dist);
  // This function should be used when the euclidean distance to the skeleton is irrelevant.
  // It is a wrapper for function iftMSSkel2DDistMap
  iftFImage      *iftMSSkel2D(iftImage *bin, iftAdjRel *A, char side);
  iftImage       *iftIntMSSkel2D(iftImage *bin, iftAdjRel *A, char side);
  // This function computes the border of all labels in the label image and then uses the
  // given set of border pixels to compute a distance transform from them.
  // These functions are a superset of the binary case.
  void  	  iftMultiLabelDistTransFromBorders(iftImage *label, iftAdjRel *A, char side, iftImage **dist, iftImage **root);
  // Computes the distance transform for a multi-label image from a given set S.
  // S is usually composed of the boundary pixels for all labels. This function
  // generalizes iftDistTrans and iftShellDistTrans since it accepts a maximum distance parameter.
  void            iftMultiLabelShellDistTransFromSet(iftSet *S, iftImage *label, iftAdjRel *A, char side, double max_dist,
					  iftImage **dist, iftImage **root);
  void 		  iftMultiLabelDistTransFromSet(iftSet *S, iftImage *label, iftAdjRel *A,	char side, iftImage **dist, iftImage **root);
  // Signed version of function above
  void 		  iftMultiLabelSignedDistTransFromBorders(iftImage *label, iftAdjRel *A, iftFImage **dist, iftImage **root);
  void            iftLabelRootPropagation(iftImage *bin, iftAdjRel *A, char side, iftImage **root, iftImage **label, iftImage **dist);
  iftImage       *iftRootPropagation(iftImage *bin, iftAdjRel *A, char side, float max_dist);
  iftFImage      *iftLabelContPixelByGeoLen(iftImage *bin);
  iftImage       *iftLiftImage(iftImage *img);
  iftImage       *iftDropImage(iftImage *bin);
  iftFImage      *iftIntegralImage(iftImage *img);
  float           iftGetIntegralValueInRegion(iftFImage *integ, iftVoxel *v, int npts);
  iftImage       *iftMarkGeometricCenters(iftImage *bin);
  iftImage       *iftComponentSizes(iftImage *bin, iftAdjRel *A);
  iftImage       *iftSurfaceArea(iftImage *bin);
  iftImage       *iftPerimeter(iftImage *bin);
  
#ifdef __cplusplus
}
#endif

#endif
