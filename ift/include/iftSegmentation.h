#ifndef IFT_SEGMENTATION_H_
#define IFT_SEGMENTATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftAdjacency.h"
#include "iftFiltering.h"
#include "iftGQueue.h"
#include "iftImage.h"
#include "iftMImage.h"
#include "iftImageForest.h"
#include "iftFImage.h"
#include "iftSeeds.h"
#include "iftBMap.h"
#include "iftFIFO.h"
#include "iftRadiometric.h"
#include "iftDataSet.h"
#include "iftClustering.h"
#include "iftClassification.h"
#include "iftImageMath.h"
#include "iftMathMorph.h"
#include "iftRepresentation.h"
#include "iftMetrics.h"


typedef struct ift_smooth_frontier {
  iftFImage *border_weight;
  iftFImage *norm_factor;
  iftFImage *prev_weight;
  iftFImage *next_weight;
  iftImage  *prev_label;
  iftImage  *next_label;
  iftImage  *prev_marker;
  iftImage  *next_marker;
  float      smooth_factor;
  int        smooth_iterations;
} iftSmoothBorder;

iftSmoothBorder *iftCreateSmoothBorder(iftImage *basins, iftAdjRel *A, int smooth_iterations, float smooth_factor);
void 		 iftDestroySmoothBorder (iftSmoothBorder **smooth);

/* Pre-processing for image segmentation by watershed transforms */
iftImage      *iftEnhanceObject(iftImage *img, iftLabeledSet *seed, int obj);
iftImage      *iftEnhanceEdges(iftImage *img, iftAdjRel *A, iftLabeledSet *seed, float alpha);
iftImage      *iftMEnhanceObject(iftMImage *img, iftLabeledSet *seed, int obj);
iftImage      *iftMEnhanceEdges(iftMImage *img, iftAdjRel *A, iftLabeledSet *seed, float alpha);

/* Watershed transforms from labeled markers */

iftImage  *iftImageDomes(iftImage  *img, iftAdjRel *A);
iftImage  *iftImageBasins(iftImage *img, iftAdjRel *A);
iftImage  *iftWatershed(iftImage *img, iftAdjRel *A, iftLabeledSet *seed);
void       iftDiffWatershed(iftImageForest *fst, iftLabeledSet *seed, iftSet *trees_for_removal);
void 	     iftRelaxObjects(iftImageForest *fst, iftSmoothBorder *smooth);
iftImage  *iftRelaxedWatershed(iftImage *basins, iftAdjRel *A, iftLabeledSet *seed, int num_smooth_iterations, float smooth_factor);
iftImage  *iftEnhanceWhenCrIsGreaterThanCb(iftImage *img);
iftImage  *iftWatershedOnPixelDist(iftDataSet *dataset, iftAdjRel *A,iftLabeledSet *seed);

iftFImage *iftWeightNormFactor(iftFImage *weight, iftAdjRel *A);

/* Watershed transforms from grayscale markers: Note that, in order to
   obtain a connected operator for some graph topology, the adjacency
   relation must be the same used to compute the input image.  */

iftImage  *iftWaterGray(iftImage *basins, iftImage *marker, iftAdjRel *A);
void       iftWaterGrayForest(iftImageForest *fst, iftImage *marker);
iftImage  *iftWaterDist(iftImage *dist, iftImage *label, int H, iftAdjRel *A);
iftImage  *iftDualWaterGray(iftImage *domes, iftImage *marker, iftAdjRel *A);

iftImage *iftOrientedWatershed(iftImage *img, iftImage *basins, iftAdjRel *A, iftLabeledSet *seed, char orientation);

/* Thresholding */
int        iftCumHistogramThres(iftImage *img, float perc);
int        iftOtsu(iftImage *img);
int        iftOtsuInRegion(iftImage *img, iftImage *mask);

iftImage  *iftThreshold(iftImage *img, int lowest, int highest, int value);
iftImage  *iftFThreshold(iftFImage *img, float lowest, float highest, int value);
iftImage  *iftBinarizeByOPF(iftImage *orig, iftImage *enha, int init_thres, float train_perc);

iftImage  *iftAboveAdaptiveThreshold(iftImage *img, iftAdjRel *A, float perc, int niters, int value); /* Computes the mean value within the adjacency region A and selects pixels that are above perc*mean, by assigning value (positive number) to them. Repeats this process for a certain number of iterations (niters), excluding pixels already selected. */
iftImage  *iftBelowAdaptiveThreshold(iftImage *img, iftAdjRel *A, float perc, int niters, int value); /* Computes the mean value within the adjacency region A and selects pixels that are below perc*mean, by assigning value (positive number) to them. Repeats this process for a certain number of iterations (niters), excluding pixels already selected. */

/* post-processing for relaxed IFT (see also iftNonUsedCode.c) */

iftFImage *iftSmoothWeightImage(iftImage *basins, float beta);
iftImage  *iftFastSmoothObjects(iftImage *labelIn, iftFImage *weight, \
				int niters);

iftImage* iftBorderImageToLabelImage(iftImage* basins, iftImage* border, float border_radius);

/* Similarity measures for binary segmentation */

float iftFBetaScore(iftImage *bin, iftImage *gt, float beta);
float iftDiceSimilarity(iftImage *bin, iftImage *gt);
float *iftDiceSimilarityMultiLabel (iftImage *mlabel, iftImage *mlgt, int number_of_objects);

float iftFScoreError(iftImage *bin, iftImage *gt);
float *iftFScoreMultiLabel (iftImage *mlabel, iftImage *mlgt, int number_of_objects);

/* Error statistics */
float *iftASDError(iftImage *current_segmentation, iftFImage **object_edt_gt, int number_of_objects);
float *iftASSDError(iftImage *current_segmentation, iftImage *gt_truth, int number_of_objects);
iftErrorClassification iftSegmentationErrors(iftImage* gt_image, iftImage* cl_image);

float iftBoundaryRecall(iftImage *gt_image, iftImage *label_image, iftAdjRel *A);
float iftBoundaryRecallFromBImage(iftImage *border_image, iftImage *label_image, iftAdjRel *A);
float iftUnderSegmentation(iftImage *gt_image, iftImage *label_image, float tol_per);
float iftUnderSegmentationSLIC2(iftImage *gt_image, iftImage *label_image, float tol_per);
float iftUnderSegmentationTurbopixel(iftImage *gt_image, iftImage *label_image);
float iftUnderSegmentationMin(iftImage *gt_image, iftImage *label_image);
float iftCompactness(iftImage *label);
float iftTopologyMeasure(iftImage *label);

iftImage *iftSelectAndPropagateRegionsAboveArea(iftImage *label, int area);
iftImage *iftSmoothRegionsByDiffusion(iftImage *label, iftImage *orig, float smooth_factor, int niters);
char iftIsSegmentationConsistent(iftImageForest *fst);
int iftIsLabelRootConnected (iftImage *pred, iftImage *label, int p);

iftLabeledSet *iftLabelToForestGeodesicRobot(iftImage *gradient,iftImage *label, int seeds_per_iteration, int number_of_objects, int min_distance_border, int max_marker_size, int min_marker_size);

#ifdef __cplusplus
}
#endif

#endif
