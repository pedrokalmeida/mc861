/*
- Recebe sete imagens em ordem.
- Roda o SVM sobre cada imagem recebida e obtem uma estimativa.
- (Se pá voce vai precisar de 26+10 SVMs??)
- Retornar uma string com a placa!
*/
#include "ift.h"
#include "hog.h"

#define CHAR_X 16
#define CHAR_Y 20

char *myCharDetec(iftSVM** classifier, iftImage **images){
   char *plate = (char*)malloc(8*sizeof(char));

   int descriptor_size = (HOG_N1/HOG_N2-HOG_N3+1)*(HOG_M1/HOG_M2-HOG_M3+1)*HOG_N3*HOG_M3*9*((CHAR_X)*(CHAR_Y));
   iftDataSet *dataset = iftCreateDataSet(7, descriptor_size);

   for (int l = 0; l < 7 ; l++) {
		iftImage *img_normalized = normalize(images[l]);
		iftImage *img_mag;
		iftImage *img_orient;
		gradient(img_normalized, &img_mag, &img_orient);
		iftDestroyImage(&img_normalized);

		for(int i=0; i<images[l]->xsize; i+=1) {
   		for(int j=0; j<images[l]->ysize; j+=1) {
            iftFeatures* feat = hog(img_mag, img_orient, i, j);
				for(int f=0; f<feat->n; f++) {
					dataset->sample[l].feat[((i*images[l]->xsize+j)*feat->n)+f] = feat->val[f];
				}
            iftDestroyFeatures(&feat);
         }
      }
		iftDestroyImage(&img_mag);
		iftDestroyImage(&img_orient);
   }

   for(int i=0; i<7; i++){
      plate[i] = '-';
   }

   for(int i=0; i<10+26; i++){
      iftSVMClassifyOVO(classifier[i], dataset, TEST);

      for(int l=0; l<7; l++){
         if(dataset->sample[l].label == 1){
            if(plate[l] != '-'){
               printf("Conflito entre svms detectado!\n");
            }
            plate[l] = i+'0';
         }
      }
   }

   plate[7] = '\0';

   return plate;
}

int main(int argc, char* argv []) {
   HOG_N1 = CHAR_X;
   HOG_M1 = CHAR_Y;

   char classifierPath[100];
   char imagesDir[7][500];
   iftImage *images[7];

   printf("please input: \t<classifier>\n\t\t<image 1>\n\t\t<image 2>\n\t\t<image 3>\n\t\t<image 4>\n\t\t<image 5>\n\t\t<image 6>\n\t\t<image 7>\n");
	scanf("%s", classifierPath);
   for(int i=0; i<7; i++){
      scanf("%s", imagesDir[i]);
   }

	int MemDinInicial, MemDinFinal;
   MemDinInicial = iftMemoryUsed();

   iftSVM **svm = (iftSVM**)malloc((10+26)*sizeof(iftSVM*));

   for(int i=0; i<10+26; i++){
      char aux[1000];
      sprintf(aux, "%s_%2d", classifierPath, i);
      svm[i] = iftReadSVM(aux);
   }

   for(int i=0; i<7; i++){
      images[i] = iftReadImageByExt(imagesDir[i]);
   }

   char *detectedPlate = myCharDetec(svm, images);

   printf("Placa detectada:%s\n", detectedPlate);

   for(int i=0; i<7; i++){
      iftDestroyImage(&images[i]);
   }

   return 0;
}
