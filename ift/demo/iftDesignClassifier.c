#include "ift.h"

#define NRUNS 10
#define TRAIN_PERC 0.75

int main(int argc, char * argv[]) {

    if(argc<3) {
        iftError("Usage: <training dataset> <best classifier>", argv[0]);
    }

    float bestAcc = 0.0f;
    float acc;

    iftDataSet* Z = iftReadOPFDataSet(argv[1]);

    for (int i = 0; i < NRUNS; ++i) {

        iftSelectSupTrainSamples(Z, TRAIN_PERC);
        iftSVM* svm = iftCreateLinearSVC(1e4);

        iftSVMTrainOVO(svm, Z);
        iftSVMClassifyOVO(svm, Z, TEST);

        acc = iftTruePositives(Z);

        printf("%2dth Classifier => %6.2f%%\n", i+1, 100*acc);

        if(acc > bestAcc) {
            bestAcc = acc;
            iftWriteSVM(svm, argv[2]);
        }

        iftDestroySVM(svm);
    }
    return 0;

}
