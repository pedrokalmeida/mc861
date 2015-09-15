/**
 * @file iftBow.h
 * @brief Bag of Visual Words module
 * @author Alan Peixinho
 *
 * @date  12 April 15
 */

#include "iftCommon.h"
#include "iftImage.h"
#include "iftDataSet.h"
#include "iftUtil.h"
#include "iftKmeans.h"
#include "iftMetrics.h"

#ifndef IFT_IFTBOW_H
#define IFT_IFTBOW_H

/** @addtogroup Descriptor
 * @{ */

/** @addtogroup BagOfWords
 * @brief Bag of Visual Words descriptor
 * @{ */


/** @brief Features Extractor Prototype */
typedef int (*iftPatchFeatsExtractor)(iftImage* img, int ibegin, int jbegin, int kbegin, int isize, int jsize, int ksize, float* featsOut);

/** @brief Patches Extractor Prototype */
typedef int (*iftPatchesSampler)(iftImage* img, int xsize, int ysize, int zsize, int nSamples, int *patchesOut);


/** @brief Bag of Words (kernels) estimator Prototype. */
typedef iftDataSet*(*iftBowKernelsEstimator)(iftDataSet* z, int nkernels);

/** @brief Coding and Pooling over iftMImage Prototype */
typedef int (*iftBowCodingPooling)(iftMImage* mImg, float* featsOut, int n);

/**
 * @brief Bag of Words Feature Extractor (Bow)
 *
 * Contains the Bow architecture information along with visual words learned.
 *
 * @author Peixinho
 */
typedef struct ift_Bow
{
    /** Number of patches sampled per image. */
    int nPatchfeats;
    /** Number of samples per image. */
    int samplesPerImage;
    /** Dictionary size. Number of visual words.*/
    int dictionarySize;

    /** Input image xsize.*/
    int xImgSize;
    /** Input image ysize */
    int yImgSize;
    /** Input image zsize. */
    int zImgSize;

    /** Xsize for sampled image patches. */
    int xPatchSize;
    /** Ysize for sampled image patches. */
    int yPatchSize;
    /** Zsize for sampled image patches. */
    int zPatchSize;

    /** Stride size along x axis. */
    int xStride;
    /** Stride size along y axis. */
    int yStride;
    /** Stride size along z axis. */
    int zStride;

    /** Number of features extracted in each image sampled path. */
    int nfeats;

    /** Dictionary. Visual words. */
    iftDataSet *dictionary;

    /** Function to extract features from sampled patches.  */
    iftPatchFeatsExtractor featsExtractor;
    /** Function to sample patches in image. */
    iftPatchesSampler sampler;
    /** Function to select the best K visual words. */
    iftBowKernelsEstimator dictionaryEstimator;
    /** Function to apply coding and pooling in the final Bow multiband image representation. */
    iftBowCodingPooling codingPooling;

} iftBow;

/**
 * @brief Extract the Raw pixels brightness.
 * Implements iftPatchFeatsExtractor()
 * @return Number of extracted features
 */
int iftBowPatchRawFeatsExtractor(iftImage* img, int ibegin, int jbegin, int kbegin, int isize, int jsize, int ksize, float* featsOut);

/**
 * @brief Extract the Raw pixels YCbCr.
 * Implements iftPatchFeatsExtractor()
 */
int iftBowPatchColorRawFeatsExtractor(iftImage* img, int ibegin, int jbegin, int kbegin, int isize, int jsize, int ksize, float* featsOut);



/**
 * @brief Extract random samples from image
 * Implements iftPatchesSampler()
 */
int iftBowPatchesRandomSampler(iftImage* img, int xsize, int ysize, int zsize, int nsamples, int *patchesOut);

/**
 * Extracts samples from image in a dense way
 * Implements iftPatchSampler()
 */
int iftBowPatchesDenseSampler(iftImage* img, int xsize, int ysize, int zsize, int nsamples, int *patchesOut);


/**
 * @brief KMeans Bag of Words estimator. Implements iftBowKernelsEstimator()
 */
iftDataSet* iftBowKMeansKernels(iftDataSet* z, int nkernels);

/**
 * @brief Supervised KMeans Bag of Words estimator. Implements iftBowKernelsEstimator()
 *
 * Computes a KMeans Bag of Words estimator for samples in each class.
 */
iftDataSet* iftBowSupKMeansKernels(iftDataSet* z, int nkernels);

/**
 * @brief Soft coding followed of histogram pooling
 * Implements iftBowCodingPooling()
 */
int iftBowSoftCoding(iftMImage* mImg, float *featsOut, int n);

/**
 * @brief Hard coding followed of histogram pooling
 * Implements iftBowCodingPooling()
 */
int iftBowHardCoding(iftMImage* mImg, float *featsOut, int n);

/**
 * @brief Don't realize coding nor histogram pooling, just grab the raw iftMImages
 * Implements iftBowCoding()
 */
int iftNoCoding(iftMImage* mImg, float *featsOut, int n);

/**
 * @brief Horizontal and vertical sum of intensities. Preserves spatial information.
 * Implements iftBowCoding()
 */
int iftBowHVSumPooling(iftMImage *mImg, float *featsOut, int n);

/**
 * @brief Create a Bow feature extractor.
 */
iftBow *iftCreateBow(int xImgSize, int yImgSize, int zImgSize, int xPatchSize, int yPatchSize, int zPatchSize,
                     int xStrideSize, int yStrideSize, int zStrideSize, int nFeats, iftPatchesSampler sampler,
                     iftPatchFeatsExtractor featsExtractor, iftBowKernelsEstimator kernelsEstimator,
                     iftBowCodingPooling coding, int samplesPerImage, int nPatchFeats, int dictSize);

/**
 * @brief Validate Bow params, to check if the architecture is feasible
 */
int iftValidateParamsBow(iftBow *bow);

/**
 * @brief Destroy a Bow feature extractor.
 */
void iftDestroyBow(iftBow **bow);

/**
 * @brief Save Bow feature extractor. (Not implemented)
 */
void iftBowWrite(iftBow *bow);

/**
 * @brief Load Bow feature extractor. (Not implemented)
 */
iftBow *iftBowRead(const char *filename);

/**
 * @brief Learn Visual Words over image dataset.
 */
void iftBowLearn(iftBow *bow, iftDir *imgArray, int nImages, iftImageLoader imgLoader);

/**
 * @brief Compute Bow iftMImage from image.
 */
iftMImage* iftBowTransform(iftBow *bow, iftImage *img);

/**
 * @brief Compute Bow features from image.
 */
iftFeatures* iftBowExtractFeatures(iftBow * bow, iftImage* img);

/**
 * @brief Compute Bow features from a batch of images.
 */
iftDataSet *iftBowExtractFeaturesBatch(iftBow *bow, iftDir *imgArray, int nImgs, iftImageLoader imgLoader);

/** @} */


/** @} */

#endif //IFT_IFTBOW_H
