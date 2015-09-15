#ifndef IFT_FIMAGE_H_
#define IFT_FIMAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftImage.h"
#include "iftAdjacency.h"

#define iftFGetXCoord(s,p) (((p) % (((s)->xsize)*((s)->ysize))) % (s)->xsize)
#define iftFGetYCoord(s,p) (((p) % (((s)->xsize)*((s)->ysize))) / (s)->xsize)
#define iftFGetZCoord(s,p) ((p) / (((s)->xsize)*((s)->ysize)))
#define iftFGetVoxelIndex(s,v) ((v.x)+(s)->tby[(v.y)]+(s)->tbz[(v.z)])
#define iftFDiagonalSize(s) (ROUND(sqrtf(s->xsize*s->xsize + s->ysize*s->ysize + s->zsize*s->zsize)))

#define iftFImageCenter(img) ((iftVoxel){(img)->xsize/2, (img)->ysize/2, (img)->zsize/2})

typedef struct ift_fimage {
  float *val;
  int    xsize,ysize,zsize;
  float  dx,dy,dz;
  int   *tby, *tbz;
  float  maxval, minval;
  int    n;
} iftFImage;

void        iftVerifyFImageDomains(iftFImage *img1, iftFImage *img2, char *function);
char        iftIs3DFImage(iftFImage *img);
int         iftFXSize(iftFImage *img);
int         iftFYSize(iftFImage *img);
int         iftFZSize(iftFImage *img);
iftVoxel    iftFGetVoxelCoord(iftFImage *img, int p);
iftFImage  *iftFCopyImage(iftFImage *img);
iftFImage  *iftCreateFImage(int xsize,int ysize,int zsize);
void        iftDestroyFImage(iftFImage **img);
char        iftFValidVoxel(iftFImage *img, iftVoxel v);
char        iftFValidPoint(iftFImage *img, iftPoint P);
float       iftFMaximumValue(iftFImage *img);
float       iftFMinimumValue(iftFImage *img);
void        iftFUpdateMinMax(iftFImage *img);
void        iftFCopyVoxelSize(iftFImage *img1, iftFImage *img2);
void        iftFSetImage(iftFImage *img, float value);
iftFImage  *iftImageToFImage(iftImage *img);
iftImage   *iftFImageToImage(iftFImage *img, int Imax);
iftFImage  *iftFNormalizeImageLocally(iftFImage *img, iftAdjRel *A);
iftFImage  *iftFReadImage(char *filename);
void        iftFWriteImage(iftFImage *img, char *filename);
iftFImage  *iftFGetXYSlice(iftFImage *img, int zcoord);
iftFImage  *iftFGetZXSlice(iftFImage *img, int ycoord);
iftFImage  *iftFGetYZSlice(iftFImage *img, int xcoord);
void        iftFPutXYSlice(iftFImage *img, iftFImage *slice, int zcoord);
void        iftFPutZXSlice(iftFImage *img, iftFImage *slice, int ycoord);
void        iftFPutYZSlice(iftFImage *img, iftFImage *slice, int xcoord);
iftFImage  *iftFReadRawSlices(char *basename, int first, int last, int xsize, int ysize);
void        iftFWriteRawSlices(iftFImage *img, char *basename);
float       iftFImageValueAtPoint(iftFImage *img, iftPoint P);
float       iftFImageValueAtPoint2D(iftFImage *img, iftPoint P);
iftImage   *iftAttCoefToHU(iftFImage *attcoef, double mean_of_water);
iftFImage  *iftFExtractROI(iftFImage *img, iftVoxel uo, iftVoxel uf);
void        iftFInsertROI(iftFImage *roi, iftFImage *img, iftVoxel pos);
iftFImage  *iftFAddFrame(iftFImage *img, int sz, float value);
iftFImage  *iftFRemFrame(iftFImage *fimg, int sz);
iftFImage  *iftFAddRectangularBoxFrame(iftFImage *img, int sx, int sy, int sz, float value);
iftFImage  *iftFRemRectangularBoxFrame(iftFImage *fimg, int sx, int sy, int sz);

#ifdef __cplusplus
}
#endif

#endif


