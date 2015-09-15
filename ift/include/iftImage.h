/**
 * @file
 * @brief Image manipulation functions.
 *
 */

#ifndef IFT_IMAGE_H_
#define IFT_IMAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftColor.h"
#include "iftFIFO.h"
#include "iftAdjacency.h"

/**
 * @addtogroup Image
 * @{
 *
 */

/**
 * @brief iftImage definition and prototypes.
 *
 * @author Falcao
*/
typedef struct ift_image {
    /** Brightness pixels array. */
    int *val;
    /** Blue component pixels array */
    ushort *Cb;
    /** Red component pixels array */
    ushort *Cr;

    /** X axis size. */
    int xsize;
    /** Y axis size. */
    int ysize;
    /** Z axis size. */
    int zsize;

    /** X axis voxel size. */
    float dx;
    /** Y axis voxel size. */
    float dy;
    /** Z axis voxel size. */
    float dz;

    int *tby, *tbz;        // speed-up voxel access tables


    int maxval, minval;
    /** Number of pixels. */
    int n; // minimum and maximum values, and number of voxels
} iftImage;

typedef struct {
    int hdrlen;
    int bpp;
    int dimensions;
    int W, H, D, T;
    float dx, dy, dz;
    int be_hint;
    int dt;
} AnalyzeHdr;

typedef struct {
    int hdrlen;
    char data_type[10];
    char db_name[18];
    int extents;
    short int error;
    char regular;
    char hkey0;
} Ana_Hdr1;

typedef struct {
    short int dim[8];
    short int unused[7];
    short int data_type;
    short int bpp;
    short int dim_un0;
    float pixdim[8];
    float zeroes[8];
    int maxval;
    int minval;
} Ana_Hdr2;

#define iftGetXCoord(s, p) (((p) % (((s)->xsize)*((s)->ysize))) % (s)->xsize)
#define iftGetYCoord(s, p) (((p) % (((s)->xsize)*((s)->ysize))) / (s)->xsize)
#define iftGetZCoord(s, p) ((p) / (((s)->xsize)*((s)->ysize)))
#define iftGetVoxelIndex(s, v) ((v.x)+(s)->tby[(v.y)]+(s)->tbz[(v.z)])
#define iftDiagonalSize(s) (ROUND(sqrtf(s->xsize*s->xsize + s->ysize*s->ysize + s->zsize*s->zsize)))

#define iftImageCenter(img) ((iftVoxel){(img)->xsize/2, (img)->ysize/2, (img)->zsize/2})

/**
 * @brief Image loader Prototype.
 *
 * A common interface between loading images functions. Can be used to receive any image loading function as a parameter.
 *
 */
typedef iftImage *(*iftImageLoader)(const char *filename, ...);

void iftVerifyImageDomains(iftImage *img1, iftImage *img2, char *function);

/**
 * @brief Check if the image has color information.
 * @param img Image to be checked.
 * @return 1 for color images, 0 for grayscale images.
 */
char iftIsColorImage(iftImage *img);

/**
 * @brief Check if the image has 3D information.
 * @param img Image to be checked.
 * @return 1 for 3D images, 0 for 2D images.
 */
char iftIs3DImage(iftImage *img);

/**
 * @brief Image X axis size.
 * @param img Image.
 * @return X axis size.
 */
int iftXSize(iftImage *img);

/**
 * @brief Image Y axis size.
 * @param img Image.
 * @return Y axis size.
 */
int iftYSize(iftImage *img);

/**
 * @brief Image Z axis size.
 * @param img Image.
 * @return Z axis size.
 */
int iftZSize(iftImage *img);

void iftDestroyPyImage(iftImage *img);

/**
 * @brief Gets the correspondent voxel to index p.
 *
 * @param p Voxel index.
 * @return Voxel coordinates from index p.
 */
iftVoxel iftGetVoxelCoord(iftImage *img, int p);

/**
 * @brief Creates a new iftImage, with (xsize, ysize, zsize) dimensions.
 *
 * For color images, check iftCreateColorImage()
 *
 * @param xsize X axis dimension
 * @param ysize Y axis dimension
 * @param zsize Z dimension
 * @return The new iftImage.
 */
iftImage *iftCreateImage(int xsize, int ysize, int zsize);

/**
 * @brief Creates a new colored iftImage, with (xsize, ysize, zsize) dimensions.
 *
 * @param xsize X axis dimension
 * @param ysize Y axis dimension
 * @param zsize Z dimension
 * @return The new iftImage.
 */
iftImage *iftCreateColorImage(int xsize, int ysize, int zsize);

/**
 * @brief Destroys the image.
 * @warning Receives a pointer to an image, not the image itself.
 *
 * @param img A pointer to the image to be destroyed.
 */
void iftDestroyImage(iftImage **img);

/**
 * @brief Recomputes the minimum and maximum value for image.
 *
 * This function updates the values of minval and maxval variables from iftImage.
 *
 * @param img The image to be updated.
 */
void iftUpdateMinMax(iftImage *img);

/**
 * @brief Copies the voxel size from img1 to img2.
 *
 * @param img1 Source image.
 * @param img2 Destination image.
 */
void iftCopyVoxelSize(iftImage *img1, iftImage *img2);

/**
 * @brief Copies the blue and red (YCbCr) components from img1 to img2.
 *
 * Copy the blue and red components from img1 to img2. Both images should have the same domain.
 *
 * @param img1 Source image.
 * @param img2 Destination image.
 */
void iftCopyCbCr(iftImage *img1, iftImage *img2);

/**
 * @brief Set the image blue and red (YCbCr) components to a certain value.
 *
 * @param img Target image.
 * @param value blue and red component value.
 */
void iftSetCbCr(iftImage *img, ushort value);

/**
 * @brief Set the image blue (YCbCr) components to a certain value.
 *
 * @param img Target image.
 * @param value blue component value.
 */
void iftSetCb(iftImage *img, ushort value);

/**
 * @brief Set the image red (YCbCr) components to a certain value.
 *
 * @param img Target image.
 * @param value red component value.
 */
void iftSetCr(iftImage *img, ushort value);

/**
 * @brief Check if the voxel belongs to the image domain.
 *
 * @param img Target image.
 * @param v Voxel coordinates.
 */
char iftValidVoxel(iftImage *img, iftVoxel v);

/**
 * Gets the maximum brightness value in the image.
 * @param img Target image.
 * @return Maximum brightness value.
 */
int iftMaximumValue(iftImage *img);

/**
 * Gets the maximum blue (YCbCr) value in the image.
 * @param img Target image.
 * @return Maximum blue value.
 */
int iftMaximumCb(iftImage *img);

/**
 * Gets the maximum red (YCbCr) value in the image.
 * @param img Target image.
 * @return Maximum red value.
 */
int iftMaximumCr(iftImage *img);

/**
 * Gets the minimum brightness value in the image.
 * @param img Target image.
 * @return Minimum brightness value.
 */
int iftMinimumValue(iftImage *img);

/**
 * @brief Read a PGM image from disk. Implements iftImageLoader()
 * @param filename Image path.
 * @return The loaded image.
 */
iftImage *iftReadImage(const char *filename, ...);

/**
 * @brief Read an image from disk according to the extension type (Accepts PGM and PPM). Implements iftImageLoader()
 * @param filename Image path.
 * @return The loaded image.
 */
iftImage *iftReadImageByExt(const char *filename, ...);

/**
 * @brief Writes an image into disk according to the extension type (Accepts PGM and PPM).
 * @param filename Image path.
 * @param The image to be written.
 */
void iftWriteImageByExt(iftImage *img, const char *filename, ...);

/**
 * @brief Writes an image into disk.
 * @param Filename to image path.
 * @param The image to be written.
 */
void iftWriteImage(iftImage *img, const char *filename, ...);

/**
 * @brief Read a PGM (P5 format) image from disk. Implements iftImageLoader()
 * @param filename Image path.
 * @return The loaded image.
 */
iftImage *iftReadImageP5(const char *filename, ...);

/**
 * @brief Writes a PGM image into disk (P5 format).
 * @param Filename to image path.
 * @param The image to be written.
 */
void iftWriteImageP5(iftImage *img, const char *filename, ...);

/**
 * @brief Converts the image to a PGM (P5 format) file and reads the image from disk. Implements iftImageLoader()
 * @warning This function is not thread safe, be carefull.
 * @param filename Image path.
 * @return The loaded image.
 */
iftImage *iftReadImageAsP5(const char *filename, ...);

void iftWriteImageExtFormat(iftImage *img, const char *filename, ...);

/**
 * @brief Converts the image to a PGM (P6 format) file and reads the image from disk. Implements iftImageLoader()
 * @warning This function is not thread safe, be carefull.
 * @param filename Image path.
 * @return The loaded image.
 */
iftImage *iftReadImageAsP6(const char *filename, ...);

/**
 * @brief Read a PGM (P6 format) image from disk. Implements iftImageLoader()
 * @param filename Image path.
 * @return The loaded image.
 */
iftImage *iftReadImageP6(const char *filename, ...);

/**
 * @brief Writes a PGM image into disk (P6 format).
 * @param Filename to image path.
 * @param The image to be written.
 */
void iftWriteImageP6(iftImage *img, const char *filename, ...);

/**
 * @brief Read a PGM (P2 format) image from disk. Implements iftImageLoader()
 * @param filename Image path.
 * @return The loaded image.
 */
iftImage *iftReadImageP2(const char *filename, ...);

/**
 * @brief Writes a PGM image into disk (P2 format).
 * @param Filename to image path.
 * @param The image to be written.
 */
void iftWriteImageP2(iftImage *img, const char *filename, ...);


iftImage *iftExtractObject(iftImage *label, int obj_code, iftVoxel *pos);

void iftInsertObject(iftImage *bin, iftImage *label, int obj_code, iftVoxel pos);

/**
 * @brief Creates a copy of an image.
 *
 * @param Source image.
 * @return The copy of the image.
 */
iftImage *iftCopyImage(iftImage *img);

/**
 * @brief Creates an image with a solid cube inside.
 *
 * Creates a cuboid inside an image with with 0.9 times the images coordinates.
 *
 * @param xsize X axis size.
 * @param ysize Y axis size.
 * @param zsize Z axis size.
 * @return The cuboid container image.
 */
iftImage *iftCreateCuboid(int xsize, int ysize, int zsize);

iftImage *iftCSVtoImage(const char *filename, ...);

/**
 * @brief Check if two voxels are adjacent according to a given adjacency.
 *
 * @param img The domain image.
 * @param A The considered adjacency.
 * @param u First image voxel.
 * @param v Second image voxel.
 * @return 1 if the voxels are adjacent, 0 otherwise.
 */
char iftAdjacentVoxels(iftImage *img, iftAdjRel *A, iftVoxel u, iftVoxel v);

/**
 * @brief Gets the image gradient magnitude (ignoring the gradient direction).
 *
 * @param img The target image.
 * @param A The adjacency relationship.
 * @return The gradient magnitude image.
 */
iftImage *iftImageGradientMagnitude(iftImage *img, iftAdjRel *A);

iftImage *iftAddFrame(iftImage *img, int sz, int value);

iftImage *iftRemFrame(iftImage *fimg, int sz);

iftImage *iftAddRectangularBoxFrame(iftImage *img, int sx, int sy, int sz, int value);

iftImage *iftRemRectangularBoxFrame(iftImage *fimg, int sx, int sy, int sz);

/**
 * @brief Set the image brightness according to value.
 *
 * @param img Target image.
 * @param value brightness value.
 */
void iftSetImage(iftImage *img, int value);

/**
 * @brief Gets the XY slice 2D image from a Z coordinate in a 3D image.
 *
 * @param img Target image
 * @param zcoord Z axis coordinate.
 * @return The 2D image slice.
 */
iftImage *iftGetXYSlice(iftImage *img, int zcoord);

/**
 * @brief Gets the ZX slice 2D image from a Y coordinate in a 3D image.
 *
 * @param img Target image
 * @param ycoord Y axis coordinate.
 * @return The 2D image slice.
 */
iftImage *iftGetZXSlice(iftImage *img, int ycoord);

/**
 * @brief  Gets the YZ slice 2D image from a X coordinate in a 3D image.
 *
 * @param img Target image
 * @param xcoord X axis coordinate.
 * @return The 2D image slice.
 */
iftImage *iftGetYZSlice(iftImage *img, int xcoord);

/**
 * @brief Inserts a 2D image as a XY slice in a 3D image in a specified Z coordinate.
 *
 * @param img Target image
 * @param slice Inserted 2D image slice
 * @param zcoord Z axis coord.
 */
void iftPutXYSlice(iftImage *img, iftImage *slice, int zcoord);

/**
 * @brief Inserts a 2D image as a ZX slice in a 3D image in a specified Y coordinate.
 *
 * @param img Target image
 * @param slice Inserted 2D image slice
 * @param ycoord Y axis coord.
 */
void iftPutZXSlice(iftImage *img, iftImage *slice, int ycoord);

/**
 * @brief Inserts a 2D image as a YZ slice in a 3D image in a specified X coordinate.
 *
 * @param img Target image
 * @param slice Inserted 2D image slice
 * @param xcoord X axis coord.
 */
void iftPutYZSlice(iftImage *img, iftImage *slice, int xcoord);

iftImage *iftSwitchXByZ(iftImage *img);

iftImage *iftSwitchYByZ(iftImage *img);

iftImage *iftSwitchXByY(iftImage *img);

iftImage *iftInvertX(iftImage *img);

iftImage *iftInvertY(iftImage *img);

iftImage *iftInvertZ(iftImage *img);

/**
 * @brief Get the closest voxel to the image geometric center. See also iftGeometricCenter()
 *
 * @param obj Target image.
 * @return The center voxel.
 */
iftVoxel iftGeometricCenterVoxel(iftImage *obj);

/**
 * @brief Get the image geometric center. See also iftGeometricCenterVoxel()
 *
 * @param obj Target image.
 * @return The center point
 */
iftPoint iftGeometricCenter(iftImage *obj);

/**
 * @brief Computes the diagonal size of an object (non zero voxels) inside the image.
 *
 * @param obj Object image
 */
int iftObjectDiagonal(iftImage *obj);


void iftGetDisplayRange(iftImage *img, int *lower, int *higher);

/**
 * @brief Gets an image composed by the blue (YCbCr) component of another.
 *
 * @param img Target image
 * @return The blue component image.
 */
iftImage *iftImageCb(iftImage *img);

/**
 * @brief Gets an image composed by the red (YCbCr) component of another.
 *
 * @param img Target image
 * @return The red component image.
 */
iftImage *iftImageCr(iftImage *img);


iftImage *iftImageRed(iftImage *img);

iftImage *iftImageGreen(iftImage *img);

iftImage *iftImageBlue(iftImage *img);

iftImage *iftImageGray(iftImage *img);

iftImage *iftCreateGaussian(int xsize, int ysize, int zsize, iftVoxel mean, float stdev, int maxval);

iftImage *iftRegionBorders(iftImage *label, int value);

iftImage *iftExtractROI(iftImage *img, iftVoxel uo, iftVoxel uf);

void iftInsertROI(iftImage *roi, iftImage *img, iftVoxel pos);

float iftObjectVolume(iftImage *label, int obj_code);

iftImage *iftCreateImageWithGaussianHistogram(int xsize, int ysize, int zsize, float mean, float stdev, int maxval);

iftImage *iftCreateImageWithTwoGaussiansHistogram(int xsize, int ysize, int zsize, float mean1, float stdev1,
                                                  float mean2, float stdev2, int maxval);

iftImage *iftReadRawSlices(char *basename, int first, int last, int xsize, int ysize, int bits_per_voxel);

void iftWriteRawSlices(iftImage *img, char *basename);

iftImage *iftExtractGreyObject(iftImage *image);

iftImage *iftExtractGreyObjectPos(iftImage *image, iftVoxel *pos);

iftImage *iftCrop2DImageByBorder(iftImage *img, int border);

iftImage *iftCropImage(iftImage *img, int dx, int dy, int dz);

/**
 * @brief Gets the interpolated voxel value in a given position.
 *
 * @param img Target image.
 * @param p position to be interpolated.
 * @return Interpolated brightness value.
 */
int iftImageValueAtPoint(iftImage *img, iftPoint P);

/**
 * @brief Gets the 2D image interpolated pixel value in a given position.
 *
 * @warning This function is for 2D images.
 *
 * @param img Target image.
 * @param p position to be interpolated.
 * @return Interpolated brightness value.
 */
int iftImageValueAtPoint2D(iftImage *img, iftPoint P);

/**
 * @brief Gets the minimum bounding box necessary to cover the image objects.
 *
 * Computes the minimum bounding box necessary to cover all the object voxels (the non zero voxels).
 *
 * @param img Target image.
 * @return The minimum bounding box.
 */
iftBoundingBox iftObjectMinimumBoundingBox(iftImage *img);

/**
 * @brief Gets the object variance in each axis.
 *
 * Gets the object variance for all the axes (non zero voxels).
 *
 * @param img Target image
 * @return The object variance in each axis.
 */
iftVector iftObjectAxesVariance(iftImage *img);

/* Simple voxel sampling methods. See iftMImage.h(c) for more
   complex ones. They return a binary mask with the selected
   voxels. */

int iftNumberOfElements(iftImage *mask);

void iftExtremeVoxelsInROI(iftImage *mask, iftVoxel *uo, iftVoxel *uf);

iftImage *iftSelectImageDomain(int xsize, int ysize, int zsize);

iftImage *iftSelectRegionOfInterest(int xsize, int ysize, int zsize, iftVoxel uo, iftVoxel uf);

/**
 * Set an image voxel to a specified color (YCbCr).
 *
 * @param img Target image
 * @param p Voxel index
 * @param YCbCr color
 */
static inline void iftSetYCbCr(iftImage *img, int p, iftColor YCbCr) {
    img->val[p] = YCbCr.val[0];
    img->Cb[p] = YCbCr.val[1];
    img->Cr[p] = YCbCr.val[2];
}

// Fast functions for accessing/putting RGB values into images.
// For efficiency, they do not check if @param img is colored.
static inline void iftSetRGB(iftImage *img, int p, int R, int G, int B, int normalization_value) {
    iftColor RGB;
    RGB.val[0] = R;
    RGB.val[1] = G;
    RGB.val[2] = B;

    iftSetYCbCr(img, p, iftRGBtoYCbCr(RGB, normalization_value));
}

// Fast functions for accessing/putting RGB values into images.
// For efficiency, they do not check if @param img is colored.
static inline void iftSetRGB2(iftImage *img, int p, iftColor rgb, int normalization_value) {
    iftSetRGB(img, p, rgb.val[0], rgb.val[1], rgb.val[2], normalization_value);
}

static inline iftColor iftGetRGB(iftImage *img, int p, int normalization_value) {
    iftColor YCbCr;

    YCbCr.val[0] = img->val[p];
    YCbCr.val[1] = img->Cb[p];
    YCbCr.val[2] = img->Cr[p];

    return iftYCbCrtoRGB(YCbCr, normalization_value);
}

//Converts between color spaces. This function quantizes the LAB cspace between 0 and 255.
iftImage *iftConvertColorSpace(iftImage *image, char origin_cspace, char dest_cspace);

// iftDicomInfo *iftGetDicomInfo(iftNameMetricPair *orderedFiles, int size);
// iftDicomInfo *iftReadDicomInfo(char *file);
// void iftWriteDicomInfo(char *path, char *fileName, iftDicomInfo* dicomInfo);
// void  iftDestroyDicomInfo(iftDicomInfo **dicomInfo);
iftImage *iftReadDicom(iftNameMetricPair *orderedFiles, int size, int xsize, int ysize, float dx, float dy, float dz,
                       int bits_per_voxel);

iftImage *iftLooseImage(iftImage *image, int xsize, int ysize, int zsize);

void iftCenterImages(iftImage *image1, iftImage *image2, iftImage **centeredImage1, iftImage **centeredImage2);

iftNameMetricPair *iftSortDicomDistance(char *source, int *size);

iftImage *iftColorTableToImage(iftColorTable *ct, int xsize, int ysize);

void iftTickColorTableImage(iftImage *img, float minval, float maxval, int nticks, const char *filename);

// By TVS
char *iftGetImageType(const char *filename, ...);

// Scene to analyze file conversion
int ana_fio_swab_16(int val);

int ana_fio_swab_32(int val);

int ana_fio_abs_read_8(FILE *f, long offset);

int ana_fio_abs_read_16(FILE *f, long offset, int is_big_endian);

int ana_fio_abs_read_32(FILE *f, long offset, int is_big_endian);

float ana_fio_abs_read_float32(FILE *f, long offset, int is_big_endian);

int ana_fio_read_8(FILE *f);

int ana_fio_read_16(FILE *f, int is_big_endian);

int ana_fio_read_32(FILE *f, int is_big_endian);

float ana_fio_read_float32(FILE *f, int is_big_endian);

int ana_fio_abs_write_8(FILE *f, long offset, int val);

int ana_fio_abs_write_16(FILE *f, long offset, int is_big_endian, int val);

int ana_fio_abs_write_32(FILE *f, long offset, int is_big_endian, int val);

int ana_fio_abs_write_float32(FILE *f, long offset, int is_big_endian, float val);

int ana_fio_abs_write_zeroes(FILE *f, long offset, int n);

int ana_fio_write_8(FILE *f, int val);

int ana_fio_write_16(FILE *f, int is_big_endian, int val);

int ana_fio_write_32(FILE *f, int is_big_endian, int val);

int ana_fio_write_float32(FILE *f, int is_big_endian, float val);

int ana_fio_write_zeroes(FILE *f, int n);

void *SwapEndian(void *Addr, const int Nb);

void iftScn2Ana(iftImage *image, char *destinyBaseName);

iftImage *iftAna2Scn(char *imageBaseName);

/** @} */

#ifdef __cplusplus
}
#endif


#endif


