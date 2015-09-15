#ifndef IFT_LABELEDSET_H_
#define IFT_LABELEDSET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iftCommon.h"
#include "iftSet.h"

typedef struct ift_labeledset {
  int elem;
  int label;
  int marker;
  int handicap;
  struct ift_labeledset *next;
} iftLabeledSet;

void iftInsertLabeledSet(iftLabeledSet **S, int elem, int label);
void iftInsertLabeledSetMarkerAndHandicap(iftLabeledSet **S, int elem, int label, int marker, int handicap);
int  iftRemoveLabeledSet(iftLabeledSet **S, int *label);
void iftRemoveLabeledSetElem(iftLabeledSet **S, int elem);
void iftDestroyLabeledSet(iftLabeledSet **S);

iftLabeledSet* iftCopyLabeledSet(iftLabeledSet *s);
iftLabeledSet* iftCopyOrderedLabeledSet(iftLabeledSet *s);

void iftConcatLabeledSet(iftLabeledSet **S1,iftLabeledSet **S2);
void iftRemoveSubsetLabeledSet(iftLabeledSet **S1,iftLabeledSet **S2);

int iftLabeledSetSize(iftLabeledSet *s);

iftSet* iftLabeledSetToSet(iftLabeledSet *S, int lb);
iftLabeledSet* iftCopyLabels(iftLabeledSet *S, int lb);

#ifdef __cplusplus
}
#endif

#endif

