#ifndef IFT_ADJACENCY_H_
#define IFT_ADJACENCY_H_

/**
 * @file
 * @brief Voxel adjacency manipulation.
 */

/** @addtogroup Image
 * @{ */

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"

/*  Adjacency relation in 3D */

/**
 * @brief Defines a neighborhood around a voxel.
 */
typedef struct ift_adjrel {
    int *dx, *dy, *dz;
    /* displacements to achieve the n adjacent voxels. */
    int n; /* number of adjacent voxels. */
} iftAdjRel;

/* Adjacency relation to be used in faster implementations that
   disconsider the image's border */

/**
 * @brief Defines a neighborhood around a voxel, with a faster Iteration.
 * @warning Can only be used for images with same size. See also ::iftAdjRel.
 */
typedef struct ift_fastadjrel {
    int n;
    /* number of adjacent voxels */
    int *dq;
    /* displacements to reach adjacent voxels for a given image */
    int bx, by, bz; /* sizes of the image's border to be disconsidered */
} iftFastAdjRel;


iftAdjRel *iftCreateAdjRel(int n); /* Allocates memory for a 3D
				      adjacency relation */

/**
 * @brief Deallocates memory for a adjacency relation object.
 * @param A Adjacency to be destroyed.
 */
void iftDestroyAdjRel(iftAdjRel **A);

/**
 * @brief Creates a 3D ball of radius @a r as adjacency relation.
 * @warning This function is used for 3D images.
 * @param r Sphere radius.
 * @return The spheric adjacency neighborhood.
 */
iftAdjRel *iftSpheric(float r);

/**
 * @brief Creates a 3D half-ball of radius @a r as adjacency relation, in the corresponding axis and direction.
 * This adjacency is useful for segmenting a volume in a single direction, e.g., a video-volume where z axis is time.
 *
 * @param r Sphere radius.
 * @param axis Axis to be considered.
 * @param direction The direction to follow in @a axis. (-1, 1)
 * @return The hemispheric adjacency.
 * */
iftAdjRel *iftHemispheric(float r, char axis, int direction);

iftAdjRel *iftSphericEdges(float r);

/**
 * @brief Creates a 2D ball of radius r on the xy plane as adjacency relation
 * @warning This function is used for 2D images.
 * @param r Circle radius.
 * @return The circular adjacency.
 * */
iftAdjRel *iftCircular(float r);

iftAdjRel *iftCircularEdges(float r);

/**
 * @brief Creates a 2D ball of radius @a r on the xy plane as adjacency relation for contour pixel labeling.
 * @param r Circle radius.
 * @return The Clock circular adjacency.
 **/
iftAdjRel *iftClockCircular(float r);

iftAdjRel *iftRightSide(iftAdjRel *A, float r);

iftAdjRel *iftLeftSide(iftAdjRel *A, float r);

/**
 * @brief Creates a rectangle adjacency with specified dimensions.
 * @warning This function is used for 2D images.
 * @param xsize Rectangle width.
 * @param ysize Rectangle height.
 * @return The rectangular adjacency.
 */
iftAdjRel *iftRectangular(int xsize, int ysize);

/**
 * @brief Creates a Cuboid adjacency with specified dimensions.
 *
 * @warning This function is used for 3D images.
 *
 * @param xsize Rectangle width.
 * @param ysize Rectangle height.
 * @param zsize Rectangle depth.
 * @return The cuboid adjacency.
 */
iftAdjRel *iftCuboid(int xsize, int ysize, int zsize);

/**
 * @brief Creates a new copy of the given adjacency.
 * @param A Adjacency to be copied.
 * @return Copy of adjacency @a A.
 */
iftAdjRel *iftCopyAdjacency(iftAdjRel *A);

iftFastAdjRel *iftCreateFastAdjRel(iftAdjRel *A, int *tby, int *tbz);

/* create an adjacency relation to speed up implementations for a given image by computing the displacements to the adjaceny voxels based on the look-up tables tby and tbz of the image. The fast implementation must disconsider the image's border */

void iftDestroyFastAdjRel(iftFastAdjRel **F);

void iftMaxAdjShifts(iftAdjRel *A, int *dx, int *dy, int *dz);

void iftWriteAdjRel(iftAdjRel *A, char *filename);

iftAdjRel *iftReadAdjRel(char *filename);


iftVoxel iftGetAdjacentVoxel(iftAdjRel *A, iftVoxel u, int adj);

/**
 * @brief Check if the adjacency is suitable for 3D images.
 * @param A The checked adjacency.
 * @return 1 if it is a 3D adjacency, 0 otherwise.
 */
int iftIsAdjRel3D(iftAdjRel *A);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
