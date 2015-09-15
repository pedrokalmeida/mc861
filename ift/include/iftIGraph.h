#ifndef IFT_IGRAPH_H_
#define IFT_IGRAPH_H_

#ifdef __cplusplus
extern "C" {
#endif

  /* TODO LIST: iftRegionIGraph (graph of superpixels from label
     image), iftSurfaceIGraph (graph of 2D/3D object surface), and all
     IFTs */ 

#include "iftCommon.h"
#include "iftImage.h"
#include "iftMImage.h"
#include "iftAdjacency.h"
#include "iftSet.h"
#include "iftLabeledSet.h"
#include "iftBMap.h"
#include "iftFHeap.h"
#include "iftCompTree.h"

#define   COMPLETE 0 /* graph where all nodes are adjacent to each other */  
#define   EXPLICIT 1 /* graph with adjacency list of the nodes */
#define   IMPLICIT 2 /* graph with translation-invariant adjacency relation */

/* Criteria for iftWaterGray*/
#define   HEIGHT 0
#define   AREA   1
#define   VOLUME 2

typedef struct ift_inode {
  int     voxel;  /* voxel (or supervoxel representative) */
  float   weight; /* the weight of the node */
  iftSet *adj;    /* index list to adjacent nodes (explicit graphs only) */
} iftINode;

typedef struct ift_igraph {
  iftINode   *node;    /* list of graph nodes */
  int         nnodes;  /* number of graph nodes */
  int         nfeats;  /* number of image features */
  iftImage   *index;   /* node index */
  float     **feat;    /* image features */
  int        *label, *marker, *root, *pred; /* forest annotation */
  float      *pvalue;  /* forest annotation */
  iftAdjRel  *A;       /* adjacency relation (implicit graphs only) */
  char        type;    /* COMPLETE, IMPLICIT, EXPLICIT */
} iftIGraph;

  /* graph with nodes indicated by mask values greater than 0. */

  iftIGraph *iftCompleteIGraph(iftMImage *img, iftImage *mask); 
 
  /* graph with nodes indicated by mask values greater than 0. The
   arcs are defined by the adjacency relation A */

  iftIGraph *iftImplicitIGraph(iftMImage *img, iftImage *mask, iftAdjRel *A);

  /* graph with nodes indicated by mask values greater than 0. The
   arcs are defined by the adjacency relation A and stored with the
   nodes. */

  iftIGraph *iftExplicitIGraph(iftMImage *img, iftImage *mask, iftAdjRel *A);

  /* graph with nodes defined by mask and arcs defined by the k
     closest neighbors within adjacency A */
 
  iftIGraph *iftSpatialKnnIGraph(iftMImage *img, iftImage *mask, iftAdjRel *A, int K);

  /* graph with nodes defined by mask and arcs defined by the threshold df */  

  iftIGraph *iftSpatialIGraph(iftMImage *img, iftImage *mask, iftAdjRel *A, float df);

  /* graph with nodes indicated by mask and arcs defined by the k
     closest neighbors */
 
  iftIGraph *iftKnnIGraph(iftMImage *img, iftImage *mask, int K);

  /* Destroy image graph from memory */

  void       iftDestroyIGraph(iftIGraph **igraph);
  
  void       iftIGraphDomes(iftIGraph *igraph); /* estimate domes of the pdf */
  void       iftIGraphBasins(iftIGraph *igraph); /* complement of the domes */
  
  /* Get path value image */

  iftImage *iftIGraphPathValue(iftIGraph *igraph);

  /* Get image with node weights */
  
  iftImage *iftIGraphWeight(iftIGraph *igraph);

  /* Get label image */
  
  iftImage *iftIGraphLabel(iftIGraph *igraph);
  
  /* Compute clusters among the graph nodes and, for explicit graphs,
     propagate labels to the remaining voxels when
     label_propagation=1 */

  //  void iftIGraphClusters(iftIGraph *igraph, char label_propagation);

  /* Compute superpixels by using the IFT-SLIC algorithm */
  
  int iftIGraphISF(iftIGraph *igraph, iftImage *seeds, float alpha, float beta0, int niters);

  /* Compute the new centers */
  int *iftIGraphSuperpixelCenters(iftIGraph *igraph, int nregions);

  /* Normalize feature vector */

  void iftNormIGraphFeatures(iftIGraph *igraph);

  /* Create a max tree from a graph.*/

  iftCompTree *iftIGraphCreateMaxTree(iftIGraph *igraph);

  /* Create a min tree from a graph.*/
  iftCompTree *iftIGraphCreateMinTree(iftIGraph *igraph);

  /* Return an igraph that is identical to the input igtraph, except for the 
     features, which are simply allocated (using the nFeatures parameter). */

  iftIGraph * iftIGraphResetFeatureSpace(iftIGraph *igraph, int nFeatures);

  /* Return the maximum value of the feature indexed by "feature" in the 
     input igraph. */

  float iftIGraphMaximumFeatureValue (iftIGraph * igraph, int feature);

  /* Return the maximum value of the feature indexed by "feature" in the 
     input igraph. */

  float iftIGraphMaximumWeight (iftIGraph * igraph);

  /* Propagate forest attributes by using the maximum arc weight between
     adjacent nodes as clustering criterion and the path values of the
     nodes as priority function to resolve ties */

  void iftIGraphClusterVoxelsByMaxArcWeight(iftIGraph *igraph, uchar pvalue_order);

  /* Computes the optimum-path forest by superior reconstruction with
     root, predecessor, and label propagation. The marker of this
     watershed transform from grayscale marker is created by adding
     D+1 to the weight. */

  void iftIGraphWaterGrayByDepth(iftIGraph *igraph, int D);

  /* Computes the optimum-path forest by superior reconstruction with
     root, predecessor, and label propagation. The marker of this
     watershed transform from grayscale marker is created by subtracting
     H+1 from the weight. */

  void iftIGraphDualWaterGrayByHeight(iftIGraph *igraph, int H);

  /* 
      */
  void iftIGraphDualWaterGray(iftIGraph *igraph, int criterion, int thres);

  /* 
      */
  void iftIGraphWaterGray(iftIGraph *igraph, int criterion, int thres);

  /* Computes the minima and maxima of the node weights */

  iftImage *iftIGraphWeightMinima(iftIGraph *igraph);
  iftImage *iftIGraphWeightMaxima(iftIGraph *igraph);

  /* Copies the feature indicated by feature_index to the weight of the graph.*/
  
  void iftIGraphCopyFeatureToWeight(iftIGraph *igraph, int feature_index);

  /* Compute the maximum arc weight in the graph */

  float iftIGraphMaximumFeatureDist(iftIGraph *igraph);

  /* Return the root of the forest by applying path compression on the
     root map */

  int iftIGraphRootVoxel(iftIGraph *igraph, int q);

  /* Enumerate the roots of the forest from 1 to N and propagate its
     labels to the remaining voxels */

  int iftIGraphEnumerateRootsAndPropagateLabels(iftIGraph *igraph); 

  /* Copy an image graph */

  iftIGraph *iftCopyIGraph(iftIGraph *igraph);

  /* Compute a minimum spanning tree of an image graph */

  iftIGraph *iftIGraphMST(iftIGraph *igraph);

  /* Set weight of the nodes in the graph */

  void iftIGraphSetWeight(iftIGraph *igraph, iftImage *weight);


#ifdef __cplusplus
}
#endif

#endif

