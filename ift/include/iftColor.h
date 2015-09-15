#ifndef IFT_COLOR_H_
#define IFT_COLOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"

#define YCbCr_CSPACE 0
#define YCbCrNorm_CSPACE 1
#define RGB_CSPACE   2
#define RGBNorm_CSPACE   3
#define GRAY_CSPACE  4
#define GRAYNorm_CSPACE  5
#define WEIGHTED_YCbCr_CSPACE 6
#define LAB_CSPACE 7
#define LABNorm_CSPACE 8

#define WHITEPOINT_X	0.950456
#define WHITEPOINT_Y	1.0
#define WHITEPOINT_Z	1.088754

#define LABF(t)	\
	((t >= 8.85645167903563082e-3) ? \
	pow(t,0.333333333333333) : (841.0/108.0)*(t) + (4.0/29.0))

#define LABINVF(t)	\
	((t >= 0.206896551724137931) ? \
	((t)*(t)*(t)) : (108.0/841.0)*((t) - (4.0/29.0)))


typedef struct ift_fcolor {
	float val[3];
} iftFColor;


typedef struct ift_color {
  int val[3];
} iftColor;

typedef struct ift_colortable {
  iftColor *color;
  int ncolors;
} iftColorTable;

  iftColorTable *iftCreateColorTable(int ncolors);
  void           iftDestroyColorTable(iftColorTable **ctb);
  iftColorTable *iftBlueToRedColorTable(int ncolors);

  iftColor  iftRGBtoYCbCr(iftColor cin, int normalization_value);
  iftColor  iftYCbCrtoRGB(iftColor cin, int normalization_value);
  iftFColor iftRGBtoLab(iftColor rgb, int normalization_value);
  iftFColor iftRGBtoLabNorm(iftColor rgb, int normalization_value);
  iftColor  iftLabtoRGB(iftFColor lab, int normalization_value);

  iftColor  iftRGBtoYCbCrBT2020(iftColor cin, const int rgbBitDepth, const int yCbCrBitDepth);
  iftColor  iftYCbCrBT2020toRGB(iftColor cin, const int yCbCrBitDepth, const int rgbBitDepth);
  iftColor  iftLabtoQLab(iftFColor lab,int normalization_value);
  iftFColor iftQLabToLab(iftColor qlab, int normalization_value);

#ifdef __cplusplus
}
#endif

#endif
