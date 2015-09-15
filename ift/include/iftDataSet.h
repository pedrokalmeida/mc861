#ifndef IFT_DATASET_H_
#define IFT_DATASET_H_

/**
 * @file
 * @brief Pattern Recognition datasets manipulation.
 */

/** @addtogroup MachineLearning
 * @{ */

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftImage.h"
#include "iftMImage.h"
#include "iftAdjacency.h"
#include "iftLabeledSet.h"
#include "iftFImage.h"
#include "iftRadiometric.h"
#include "iftMatrix.h"
#include "iftSort.h"
#include "iftSet.h"
#include "iftKernel.h"
#include "iftParseInput.h"
#include "iftDescriptors.h"

#define TEST      0
#define TRAIN     1
#define OUTLIER   2
#define ERROR     3

#define WEIGHT    0
#define LABEL     1
#define CLASS     2
#define POINT     3
#define STATUS    4

#define K_distance5 20.0

typedef struct ift_minkowski_table {
  float *dist;    /* look-up table for Minkowski distance
		     computation */
  int    nelems;  /* number of elements: 2*mult_factor + 1 */ 
  int    mult_factor; /* multiplication factor */
  float  exp_factor;  /* exponential factor */ 
} iftMinkowskiTable;

extern iftMinkowskiTable *iftMinkowski; /* Use iftSetMinkowskiTable to
					   initialize it */


/* the coefficients alpha can be used to modify the arc weight
computation in many different ways. These coefficients must be found
by optimization. */

typedef float (*iftArcWeightFun)(float *f1, float *f2, float *alpha, int n);


/* Table of distances */
typedef struct ift_distance_table {
  float **distance_table; /* n x n table of distances, for n samples */
  int nsamples;
} iftDistanceTable;

extern iftDistanceTable *iftDist; /* Use iftSetDistanceFunction
					  to change it to one of the
					  iftDistanceX, X=1,2...,7 */


/**
 * @brief A sample in a Pattern Recognition problem.
 * Contains the feature descriptor and, for supervised cases, the sample class.
 */
typedef struct ift_sample {
  float *feat;    // feature values
  int    truelabel;   // 1,2,...,nclasses
  int    label;   // 1,2,....,nlabels
  int    id;      // identification which may be either a voxel of the
                  // related image or the position of an image in the
                  // directory with a related list of images.
  
  float  weight;  // probability density, path cost, etc
  uchar  status;  // Status of the sample: TEST, TRAIN, ERROR, OUTLIER.
} iftSample;

/* structure used for feature space transformations based on PCA */

typedef struct _featspaceparam {
  float     *mean, *stdev; // parameters used for feature space normalization
                           // and centralization
  int        nfeats; // size of mean, stdev
  char      *w; // binary vector that indicates selected components for SupPCA
  int        ncomps; // size of w (number of PCA components before
                     // feature space reduction by SupPCA)
  iftMatrix *R; // Projection/rotation matrix (the columns are the eigenvectors)
  iftMatrix *W; // Whitening Transformation
} iftFeatSpaceParam;

/**
 * @brief Dataset structure for pattern recognition.
 * Contains a list of samples. Each sample containing the description features. And, for supervised cases, the sample class.
 * See also ::iftSample.
 */
typedef struct ift_dataset {
  iftSample *sample;   // sample
  int        nfeats;   // number of features
  int        nsamples; // number of samples
  int        nlabels;  // number of clusters 
  int        nclasses; // number of classes
  int        ntrainsamples; // number of training samples
  iftArcWeightFun iftArcWeight; // distance function
  float     *alpha;   // coefficients used for arc weight computation
  void      *ref_data; // it migh be an image, for voxel datasets, a text file with image filenames, for image datasets, a region graph, for supervoxel datasets, etc.
  iftFeatSpaceParam fsp; // parameters of the feature scape transformation
} iftDataSet;

/**
 * @brief Sampling strategy to split a dataset.
 * Splits dataset in TRAIN and TEST for several runs.
 * @author Peixinho
 */
typedef struct ift_sampler {
    char** status;
    int niters;
    int nsamples;
} iftSampler;

/**
 * @brief Creates a Sampler Strategy object
 * @param nsamples Number of samples.
 * @param niters Number of iterations.
 * @author Peixinho
 */
iftSampler* iftCreateSampler(int nsamples, int niters);

/**
 * @brief Destroy a sampler object.
 * @author Peixinho
 */
void iftDestroySampler(iftSampler** sampler);

/**
 * @brief Implements a KFold Sampling method.
  * @param nsamples Number of samples.
  * @param k Fold size.
  * @author Peixinho
 */
iftSampler* iftKFold(int nsamples, int k);

/**
 * @brief Repeats N times a KFold Sampling method. The default usage is a 5x2 cross validation.
 * @param nsamples Number of samples.
 * @param n Number of times to run kfold.
 * @param k Fold size.
 * @author Peixinho
 */
iftSampler* iftNKFold(int nsamples, int n, int k);

/**
 * @brief Leave One Out Sampling method. Uses all data to train, except one sample used to test. Repeats the process to each sample.
 * @param nsamples Number of samples.
 * @author Peixinho
 */
iftSampler* iftLeaveOneOut(int nsamples);

/**
 * @brief Repeats N times a Random Subsampling method.
 * @param nsamples Number of samples.
 * @param n Number of times to run subsampling.
 * @param ntrain Number of training samples.
 * @author Peixinho
 */
iftSampler* iftRandomSubsampling(int nsamples, int n, int ntrain);

/**
 * @brief Selects TRAIN and TEST samples according to the sampler strategy.
 * @param Z Dataset to split.
 * @param sampler Sampler strategy
 * @param iteration Current sampling iteration.
 * @author Peixinho
 */
void iftSampleDataSet(iftDataSet* Z, iftSampler* sampler, int iteration);

/**
 * @brief Check if the dataset contains class information and therefore is able to perform supervised classification.
 * @param Z The dataset to be checked
 * @return 1 if is a supervised dataset, 0 otherwise.
 */
char        iftIsSupervisedDataSet(iftDataSet *Z);

/**
 * @brief Check if the dataset is built of training samples.
 * @param Z The dataset to be checked
 * @return 1 if is a training dataset, 0 otherwise.
 */
char        iftIsTrainingDataSet(iftDataSet *Z);

/**
 * @brief Check if the dataset is built of testing samples.
 * @param Z The dataset to be checked.
 * @return 1 if is a testing dataset, 0 otherwise.
 */
char        iftIsTestingDataSet(iftDataSet *Z);

/**
 * @brief Check if the dataset has normalized data. See also iftNormalizeDataSet() and iftNormalizeDataSet2().
 * @param Z The dataset to be checked
 * @return 1 if is a normalized dataset, 0 otherwise.
 */
char        iftIsNormalizedDataSet(iftDataSet *Z);

/**
 * @brief Check if the dataset has centralized data. See also iftCentralizeDataSet().
 * @param Z The dataset to be checked
 * @return 1 if is a centralized dataset, 0 otherwise.
 */
char        iftIsCentralizedDataSet(iftDataSet *Z);

/**
 * @brief Check if the dataset has whitened data. See also iftWhiteningTransform.
 * @param Z The dataset to be checked
 * @return 1 if is a whitened dataset, 0 otherwise.
 */
char        iftIsWhitenedDataSet(iftDataSet *Z);

/**
 * @brief Check if the dataset is preprocessed by PCA. See also iftTransFeatSpaceByPCA(), iftTransFeatSpaceByPCA2() and iftTransFeatSpaceByPCA3().
 * @param Z The dataset to be checked
 * @return 1 if is a whitened dataset, 0 otherwise.
 */
char        iftIsTransformedDataSetByPCA(iftDataSet *Z);

/**
 * @brief Creates an empty dataset.
 * @param nsamples Number of samples in the dataset.
 * @param nfeats Number of features in dataset samples.
 * @return The new dataset.
 */
iftDataSet *iftCreateDataSet(int nsamples, int nfeats);

/**
 * @brief Creates an empty dataset, without feature vectors allocation. See also iftCreateDataSet().
 * Create an iftDataSet with no Feature allocation, used for address copy between datasets.
 * @param nsamples Number of samples in the dataset.
 * @param nfeats Number of features in dataset samples.
 * @return The new dataset.
 */
iftDataSet *iftCreateDataSet2(int nsamples, int nfeats);

/**
 * @brief Destroys a dataset object.
 * @param Z Dataset to be destroyed.
 */
void        iftDestroyDataSet(iftDataSet **Z);

/**
 * @brief Destroys a dataset object. See also iftDestroyDataSet().
 * Destroy an iftDataSet with no Feature deallocation, used for datasets with address feats copied.
 * @param Z Dataset to be destroyed.
 */
void 		iftDestroyDataSet2(iftDataSet **Z);

/**
 * @brief Creates a copy of the dataset.
 * @param Z The dataset to be copied.
 * @return The new dataset.
 */
iftDataSet *iftCopyDataSet(iftDataSet *Z);

/**
 * @brief Copy an iftDataSet without copying its feature vectors, only pointing to them. See also iftCopyDataset().
 * This function is similar to the iftCopyDataset, however, it only points to the feature vectors. Avoiding duplicated data.
 * @param Z DataSet to be copied.
 * @return The copied DataSet
*/
iftDataSet *iftCopyDataSet2(iftDataSet *Z);

/**
 * @brief Merge two datasets into one. The datasets should have different classes.
 * @warning This function assumes that Z1 and Z2 don't have common classes. This function also doesn't validate if the classes from Z1 and Z2 really are distinct between themselves, i. e., intersection(classes_Z1, classes_Z2) = empty
 * @param Z1 Dataset 1.
 * @param Z2 Dataset 2.
 * @return Merge of datasets.
 */
iftDataSet *iftMergeDataSetsWithDifferentClasses(iftDataSet *Z1, iftDataSet *Z2);

/**
 * @brief Validates if a group of datasets has the same parameters (Number of samples, Number of features and Number of classes).
*
* @param Zs Array of datasets to be validated.
* @param num_Z Number of datasets.
*/
void		iftValidateDataSets(iftDataSet **Zs, int num_Z);

/**
 * @brief Creates a dataset from an image, where each sample has the voxel brightness value (gray image), or the Y, Cb an Cr (color image).
 * @param img Image to be converted.
 * @return The image dataset.
 */
iftDataSet *iftImageToDataSet(iftImage *img);
void        iftImageGTToDataSet(iftImage* imgGT,iftDataSet* Zpixels);
iftDataSet *iftImageToDataSetUsingAdjacency(iftImage *img, iftAdjRel *A);
iftDataSet *iftObjectToDataSet(iftImage *label);
iftDataSet *iftReadXYDataSet(char *filename);
iftDataSet *iftImageSeedsToDataSet(iftImage *img, iftLabeledSet *S);
iftDataSet *iftMImageSeedsToDataSet(iftMImage *img, iftLabeledSet *S);
iftDataSet *iftImageRegionToDataSet(iftImage *img, iftImage *label);
iftDataSet *iftSupervoxelsToDataSet(iftImage *img, iftImage *label);
iftDataSet *iftSupervoxelsToAvgMinMaxDataSet(iftImage *img, iftImage *label);
iftDataSet *iftSupervoxelsToHistogramDataSet(iftImage *img, iftImage *label, int nbins);
iftDataSet *iftSupervoxelsToMeanSizeDataSet(iftImage* image, iftImage* label_image, char colorspace);
iftDataSet *iftSupervoxelsToSelectiveSearchDataset(iftImage *image, iftImage* label_image, int bins_per_band, char colorspace);

iftDataSet* iftSupervoxelsToLabColorMomentsDataset(iftImage* image, iftImage* label_image);
iftDataSet* iftSupervoxelsToLabHistogramDataset(iftImage* image, iftImage* label_image, int bins_per_band);
iftDataSet* iftSupervoxelsToBICDataset(iftImage* image, iftImage* label_image, int bins_per_band);
iftDataSet* iftSupervoxelsToUniformLBP2D(iftImage* image, iftImage* label_image);
iftDataSet* iftSupervoxelsToSimplifiedHOG2D(iftImage* image, iftImage* label_image, int nbins);
iftDataSet* iftConcatDatasetFeatures(iftDataSet** datasets, int ndatasets);

iftDataSet *iftRegionMergingDataSet(iftMImage *image, iftImage* label_image);

iftDataSet *iftMSupervoxelsToDataSet(iftMImage *mimg, iftImage *label);

iftDataSet *iftImageBorderDataSet(iftDataSet *Z1, int size, int nsamples);

/**
 * @brief Read a dataset file.
 * @param filename Filepath to the dataset.
 * @return The loaded dataset.
 */
iftDataSet *iftReadOPFDataSet(char *filename);

/**
 * @brief Read a dataset file. See also iftReadDataSet().
 * @param fp File pointer to the dataset.
 * @return The loaded dataset.
 */
iftDataSet *iftReadOPFDataSetFilePointer(FILE* fp);

/**
 * @brief Stores a dataset in a file.
 * @param Z The dataset to be stored.
 * @param filename Filepath to save the dataset.
 */
void        iftWriteOPFDataSet(iftDataSet *Z, char *filename);

/**
 * @brief Stores a dataset in a file. See also iftWriteOPFDataSet().
 * @param Z The dataset to be stored.
 * @param fp File pointer to save the dataset.
 */
void iftWriteOPFDataSetFilePointer(iftDataSet *Z, FILE* fp);

/**
 * @brief Remove information added to the dataset (labels and train/test status).
 * @param Z Dataset to reset.
 */
void        iftResetDataSet(iftDataSet *Z);

/**
 * @brief Select samples to train, respecting a priori distribution of classes. See also iftSelectUnsupTrainSamples().
 * The function marks each sample in the dataset with a @a status of TRAIN or TEST
 * @param Z Dataset to be splitted.
 * @param perc Percentage of training data (0,1).
 * @return Number of training samples.
 */
int         iftSelectSupTrainSamples(iftDataSet *Z, float perc);

/**
 * @brief Select samples to train. See also iftSelectSupTrainSamples().
 * The function marks each sample in the dataset with a @a status of TRAIN or TEST
 * @param Z Dataset to be splitted.
 * @param perc Percentage of training data (0,1).
 * @return Number of training samples.
 */
int         iftSelectUnsupTrainSamples(iftDataSet *Z, float perc);

/**
 * @brief Select samples to train. According to a weighted sample selection. See also iftSelectUnsupTrainSamples().
 * The function marks each sample in the dataset with a @a status of TRAIN or TEST.
 * Each sample has a @a weight field to determine the weight to be considered.
 * @param Z Dataset to be splitted.
 * @param perc Percentage of training data (0,1).
 * @return Number of training samples.
 */
int         iftSelectUnsupTrainSamplesByWeight(iftDataSet *Z, float train_perc);

void        iftCopyClassifResult(iftDataSet *Z1, iftDataSet *Z2);
iftFeatSpaceParam iftComputeOverallMeanAndStdevFromDatasets(iftDataSet **Zs, int num_Z);
iftDataSet *iftSetClassAsPositiveIntoDataSet(iftDataSet *Z, int positive_class, int *num_pos);

/**
 * @brief Normalize the dataset features. See also iftNormalizeDataSet2().
 * @param Z Dataset to be normalized.
 * @return The normalized dataset.
 */
iftDataSet *iftNormalizeDataSet(iftDataSet *Z);

/**
 * @brief Normalize the dataset features, using the previously computed mean and standard deviation. See also iftNormalizeDataSet().
 * @param Z Dataset to be normalized.
 * @return The normalized dataset.
 */
iftDataSet *iftNormalizeDataSet2(iftDataSet *Z, iftFeatSpaceParam fsp);

/**
 * @brief Normalize the dataset samples. Creating unit vector samples.
 * @param Z Dataset to be normalized.
 */
void        iftNormalizeSamples(iftDataSet *Z);
iftDataSet *iftNormalizeContrastDataSet(iftDataSet *Z);
iftDataSet *iftCentralizeDataSet(iftDataSet *Z);

/**
 * @brief Normalize the dataset samples. Creating unit vector samples.
 * @param Z Dataset to be normalized.
 * @return The normalized dataset.
 */
iftDataSet *iftUnitNormDataSet(iftDataSet* Z);
iftDataSet *iftRotateDataSet(iftDataSet *Z, iftMatrix *U);
iftDataSet *iftEliminateAmbiguousSamples(iftDataSet *Z);

/**
 * @brief Creates a new iftDataSet with samples that belongs to the given class.
 *
 * This function creates a new dataset with all the samples that belongs to the specified class.
 * The samples are copied in a new dataset without any change to the original one.
 *
 * @param Z Original dataset.
 * @param truelabel The class label to be selected.
 * @return A dataset only containing samples from the specific class.
 */
iftDataSet *iftExtractClass(iftDataSet *Z, int truelabel);
iftDataSet *iftExtractObjectClasses(iftDataSet *Z);
iftDataSet *iftExtractSamples(iftDataSet *Z, uchar status);

/**
 * @brief Join multiple datasets into one.
 *
 * @author Peixinho
 * @date 24 Aug 2015
 *
 * @param Z Array of datasets.
 * @param ndatasets Array of datasets size.
 */
iftDataSet *iftJoinDatasets(iftDataSet** Z, int ndatasets);

/**
 * @brief Counts the number of different classes present in the dataset. Also updates the ::iftDataSet.nclass variable.
 *
 * @author Peixinho
 * @date 24 Aug 2015
 *
 * @param Z Input dataset.
 * @return Number of classes in the dataset.
 */
int iftCountNumberOfClassesDataSet(iftDataSet* Z);

/**
 * @brief Normalize the testing dataset according to the data distribution found in the training dataset.
 * @param Z1 Training dataset.
 * @param Z2 Testing dataset.
 * @return Normalized testing dataset.
 */
iftDataSet *iftNormalizeTestDataSet(iftDataSet *Z1, iftDataSet *Z2);

iftDataSet *iftNormalizeTestDataSet2(iftDataSet *Z, iftFeatSpaceParam fsp);

/**
 * @brief Centralize the testing dataset according to the data distribution found in the training dataset.
 * @param Z1 Training dataset.
 * @param Z2 Testing dataset.
 * @return Centralized testing dataset.
 */
iftDataSet *iftCentralizeTestDataSet(iftDataSet *Z1, iftDataSet *Z2);
void        iftMultDataSetByScalar(iftDataSet *Z, float scalar);
iftDataSet *iftMMKernelToDataSet(iftMMKernel *K);
iftMMKernel *iftDataSetToMMKernel(iftDataSet *Z);

/**
 * @brief Apply the PCA dimensionality reduction in the dataset features.
 * @param Z The original dataset.
 * @param num_of_comps Number of principal components.
 * @return The reduced dataset.
 */
iftDataSet *iftTransFeatSpaceByPCA(iftDataSet *Z, int num_of_comps);
iftFeatSpaceParam iftTransFeatSpaceByPCA2(iftDataSet *Z, int num_of_comps);

/**
 * @brief Whiten the dataset through PCA.
 * @param Z The original dataset.
 * @return The whitened dataset.
 */
iftDataSet *iftWhiteningTransform(iftDataSet *Z);
iftDataSet *iftTransFeatSpaceBySupPCA(iftDataSet *Z, int num_of_comps);
iftDataSet *iftTransformTestDataSetByPCA(iftDataSet *Z1, iftDataSet *Z2);
iftDataSet *iftFastTransformTestDataSetByPCA(iftDataSet *Z1, iftDataSet *Z2);
iftDataSet *iftTransformTestDataSetByPCAWhitening(iftDataSet *Z1, iftDataSet *Z2, iftMatrix *S);
iftDataSet *iftInverseTransformTestDataSetByPCA(iftDataSet *Z1, iftDataSet *Z2);
iftDataSet *iftInverseTransformTestDataSetByPCA2(iftFeatSpaceParam fsp, iftDataSet *Z2);

iftImage   *iftDataSetLabel(iftDataSet *Z, char *ref_data_type);
iftImage   *iftDataSetWeight(iftDataSet *Z, char *ref_data_type);
void        iftConfusionMatrix(iftDataSet *Z);

/**
 * @brief Set the status of all samples in the dataset.
 * @param Z The dataset.
 * @param status The status applied to the samples. {TRAIN, TEST}
 */
void        iftSetStatus(iftDataSet *Z,uchar status);
void 	    iftSetStatusForSet(iftDataSet *Z, iftSet *set, uchar status);
iftMatrix  *iftCovarianceMatrix(iftDataSet *Z);
iftMatrix  *iftRotationMatrixByPCA(iftDataSet *Z);
iftMatrix  *iftRotationMatrixAndSingularValuesByPCA(iftDataSet *Z, iftMatrix **S);
iftMatrix  *iftDataSetToFeatureMatrix(iftDataSet *Z);
iftDataSet *iftFeatureMatrixToDataSet(iftMatrix *X);
iftDataSet *iftFeatureMatrixToDataSet2(iftMatrix *X, int nrows, int ncols);
iftVector   iftPrincipalAxis(iftImage *label);

iftDataSet *iftNormOneDataSet(iftDataSet *Z);

iftFeatSpaceParam iftCreateFeatSpaceParam(void);
iftFeatSpaceParam iftReadPCAMatrix(char *filename);
void 			  iftWritePCAMatrix(iftFeatSpaceParam fsp, char *filename);
iftFeatSpaceParam iftCopyFeatSpaceParam(iftDataSet *Z);
iftFeatSpaceParam iftCopyFeatSpaceParam2(iftFeatSpaceParam fsp);
iftFeatSpaceParam iftTransFeatSpaceByPCA3(iftDataSet *Z, int num_of_comps, iftMatrix **S);

iftDistanceTable *iftCreateDistanceTable(int nsamples);
void              iftDestroyDistanceTable(iftDistanceTable **dt); 
iftDistanceTable *iftReadDistanceTable(char *filename);
void              iftWriteDistanceTable(iftDistanceTable *dt, char *filename);
iftDistanceTable *iftCompDistanceTable(iftDataSet *Z);

FILE **iftBuildDataSetsFromMImages(int nimages, int nclasses, int nfeatures, int nfilters,
		int stride, iftAdjRel *A, char *mimage_directory, char *output_directory, iftImageNames *img_names);
void iftExtractSamplesOfMImages(iftMImage *img, int truelabel, iftAdjRel *A, int sample_stride, FILE **fp);

void iftSwitchSamples(iftSample *s1, iftSample *s2, int nfeats);
void iftCopySample(iftSample *sin, iftSample *sout, int nfeats);
void iftCopySampleWithoutFeats(iftSample *sin, iftSample *sout);
void iftSwitchNotSVsPerErrorSamples(iftDataSet *Ztrain, iftDataSet *Ztest, int *not_SVs,
		int num_not_SVs, int *error_samples, int num_errors);
iftDataSet *iftSelectSamplesOfTheClass(iftDataSet *Z, int truelabel);
iftDataSet *iftSelectNegativeSamples(iftDataSet *Z, int positive_class);
iftDataSet *iftBuildPatchDataSet(iftDataSet *Z, int patch, int nfeats);
iftDataSet *iftBuildDataSetFromPatches(iftDataSet *Z, int *patches, int num_of_patches, int nfeats);
iftDataSet *iftBuildPatchDataSetFromSample(iftSample sample, int num_of_patches, int nfeats);

/* Used by pyift */
void iftSetTrainingSupervoxelsFromSeeds(iftDataSet *dataset, iftImage *label_image, iftLabeledSet* seed);

iftDataSet* iftMImageToDataSet(iftMImage* mimg);
iftDataSet* iftMImageToDataSetInRegion(iftMImage* mimg, iftImage *mask);

iftDataSet* iftMImageToEdgesDataSet(iftMImage* mimg,iftAdjRel* A);
iftDataSet* iftMImageToLabeledEdgesDataSet(iftMImage* mimg,iftImage* imgGT, iftImage* imglabels,iftAdjRel* A);
iftFImage*  iftEdgesDataSetToFImage(iftDataSet* dataset,iftMImage* mimg,iftAdjRel* A);
iftImage*   iftPixelDataSetToImage(iftDataSet*,iftImage *img);
iftFImage*  iftPixelDataSetToFImage(iftDataSet* dataset,iftImage* img);
void        iftSetDistanceFunction(iftDataSet *Z, int function_number);

iftDataSet *iftAlignDataSetByPCA(iftDataSet *Z);

/*---------------------- Distance functions -----------------------------*/

/** @brief Default distance: Euclidean Distance
 * When alpha is either 0 or 1, it selects features for Euclidean distance. When alpha is from 0 to 1, it becomes a sort of weighted Euclidean distance.
 */
float iftDistance1(float *f1, float *f2, float *alpha, int n);

/**
 * @brief  Log of Euclidean Distance.
 */
float iftDistance2(float *f1, float *f2, float *alpha, int n);

/**
 * @brief In this function, alpha plays the expoents of the absolute differences between each feature. Features must be normalized within [0,1] to use this distance function and alpha >= 0.
 *
 */
float iftDistance3(float *f1, float *f2, float *alpha, int n);

float iftDistance4(float *f1, float *f2, float *alpha, int n); /* Log of
								  Distance
								  3 */


float iftDistance5(float *f1, float *f2, float *alpha, int n);


/**
 * @brief Inner-product-based distance for centralized datasets. Assumes feature vectors with norm 1.
 */
float iftDistance6(float *f1, float *f2, float *alpha, int n);

/**
 * @brief Minkowsky distance. Assumes feature vectors with norm 1.
 */
float iftDistance7(float *f1, float *f2, float *alpha, int n);

float iftDistance8(float *f1, float *f2, float *alpha, int n);

float iftDistance9(float *f1, float *f2, float *alpha, int n);

float iftDistance10(float*, float*, float*, int);

float iftDistance11(float*, float*, float*, int);

/* initialize look-up table for Minkowski
				       distance computation. */
void  iftSetMinkowskiTable(int mult_factor, float exp_factor);

void  iftDestroyMinkowskiTable(void);

float iftDistMeanSizeSupervoxel(float *f1, float *f2, float *alpha, int n);
float iftDistSelectiveSearchSupervoxel(float *f1, float *f2, float *alpha, int n);
float iftRegionMergingDist(float *f1, float *f2, float *alpha, int n);

/* --------------------- Merging functions ----------------------------- */

float* iftMergeMeanSizeSupervoxel(float *f1, float*f2, float*alpha, int n);
float* iftMergeSelectiveSearchSupervoxel(float *f1, float *f2, float *alpha, int n);
float* iftMergeVector(float *f1, float*f2, float*alpha, int n);
float* iftRegionMergingFeat(float *f1, float*f2, float*alpha, int n);

#ifdef __cplusplus
}
#endif

/** @} */

#endif

