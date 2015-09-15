//
// Created by peixinho on 7/28/15.
//

#ifndef IFT_IFTFITNESSFUNC_H
#define IFT_IFTFITNESSFUNC_H

/* Fitness function used for optimization problems */
typedef float (*iftFitnessFunc)   (void *problem, float *theta);

#endif //IFT_IFTFITNESSFUNC_H
