/*
- Ler imagens do professor de digitos e numeros.
- Separar uma porcentagem das imagens de cada digito/caractere para teste.
- Treinar um SVM com todos os digitos/caracteres de treino 10 vezes.
- Para cada treino, testar a acuracia sobre as imagens de teste.
- Escolher e salvar o melhor SVM.
*/

#include "ift.h"

#define NRUNS 10
#define TRAIN_PERC 0.75

/*iftDataSet* convertDataSet(iftDataSet *Z, int trueLabel){
   iftDataSet *converted = iftCreateDataSet(Z->nsamples, Z->nfeats);
   converted->nclasses = 2;

   printf("samples:%d\n", Z->nsamples);

   for(int i=0; i<Z->nsamples; i++){
		for(int f=0; f<Z->nfeats; f++) {
			converted->sample[i].feat[f] = Z->sample[i].feat[f];
		}

      if (Z->sample[i].truelabel == trueLabel){
         converted->sample[i].truelabel = 1;
      } else {
         converted->sample[i].truelabel = 2;
      }
   }

   return converted;
}*/

int main(int argc, char * argv[]) {

	char dataset[100], classifier[100];
   char aux[1000];
	printf("please inputeeeee: \t<training dataset>\n\t\t<<best classifier>\n");
	scanf("%s", dataset);
	scanf("%s", classifier);

   float bestAcc;
   float acc;

   for(int label=0; label<10+26; label++){
      bestAcc = 0.0f;
      char aux[1000];
      sprintf(aux, "%s_%2d", dataset, label);
      printf("%s\n", aux);
      iftDataSet* Z = iftReadOPFDataSet(aux);
      printf("samples:%d\n", Z->nsamples);
      printf("Gerando label %d\n", label);
      for (int i = 0; i < NRUNS; ++i) {
         iftDataSet* currentZ = iftCopyDataSet(Z);
        iftSelectSupTrainSamples(currentZ, TRAIN_PERC);
        iftSVM* svm = iftCreateLinearSVC(1e4);

        iftSVMTrainOVO(svm, currentZ);
        iftSVMClassifyOVO(svm, currentZ, TEST);

        acc = iftTruePositives(currentZ);

        printf("%2dth Classifier => %6.2f%%\n", i+1, 100*acc);

        if(acc > bestAcc) {
            bestAcc = acc;
            sprintf(aux, "%s_%2d", classifier, label);
            iftWriteSVM(svm, aux);
        }

        iftDestroySVM(svm);
      }
      iftDestroyDataSet(&Z);
   }
    return 0;

}
