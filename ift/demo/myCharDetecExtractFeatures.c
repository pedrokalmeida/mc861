/*
- Ler imagens do professor de digitos e numeros.
- Separar uma porcentagem das imagens de cada digito/caractere para teste.
- Treinar um SVM com todos os digitos/caracteres de treino 10 vezes.
- Para cada treino, testar a acuracia sobre as imagens de teste.
- Escolher e salvar o melhor SVM.
*/

#include "hog.h"

#define CHAR_X 16
#define CHAR_Y 20

int main(int argc, char *argv[]) {
   HOG_N1 = CHAR_X;
   HOG_M1 = CHAR_Y;

	char digitsPath[100], lettersPath[100], datasetPath[100];
	printf("please input: \t<digits folder>\n\t\t<letters folder>\n\t\t<dataset>\n");
	scanf("%s", digitsPath);
	scanf("%s", lettersPath);
	scanf("%s", datasetPath);
	
	int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();


   iftDir *digits_dir, *letters_dir;
   digits_dir = iftLoadFilesFromDirectory(digitsPath, "pgm");
   letters_dir = iftLoadFilesFromDirectory(lettersPath, "pgm");

   iftImage *img;
   iftDataSet **dataset = (iftDataSet**)malloc((10+26)*sizeof(iftDataSet*));
   iftFeatures *feat;

   int descriptor_size = (HOG_N1/HOG_N2-HOG_N3+1)*(HOG_M1/HOG_M2-HOG_M3+1)*HOG_N3*HOG_M3*9*((CHAR_X)*(CHAR_Y));
   int total_candidates = digits_dir->nfiles + letters_dir->nfiles;

   printf("%d caractericas x %d descritores = %d \n", descriptor_size, total_candidates, descriptor_size*total_candidates);

   for(int i=0; i<10+26; i++){
      dataset[i] = iftCreateDataSet(total_candidates, descriptor_size);
      dataset[i]->nclasses = 2; // 10 digitos + 26 letras
   }

   timer *t1 = NULL, *t2 = NULL;
   int index_cand = 0;

   for (int l = 0; l < digits_dir->nfiles ; l++) {
      t1 = iftTic();
      img = iftReadImageByExt(digits_dir->files[l]->pathname);

		iftImage *img_normalized = normalize(img);
		iftImage *img_mag;
		iftImage *img_orient;
		gradient(img_normalized, &img_mag, &img_orient);
		iftDestroyImage(&img_normalized);

		for(int i=0; i<img->xsize; i+=1) {
   		for(int j=0; j<img->ysize; j+=1) {
            feat = hog(img_mag, img_orient, i, j);
				for(int f=0; f<feat->n; f++) {
               for(int l=0; l<10+26; l++)
   					dataset[l]->sample[index_cand].feat[((i*img->xsize+j)*feat->n)+f] = feat->val[f];
				}
            iftDestroyFeatures(&feat);
         }
      }

      // Define a label a partir do nome do arquivo.
      //// Para as imagens de digitos, esse valor fica na sexta posição do nome da imagem.
      char *imageName = basename(digits_dir->files[l]->pathname);
      printf("Digito: %d\n", ((imageName[4]-'0')*10 + (imageName[5]-'0')));
      for(int i=0; i<10+26; i++){
         if(((imageName[4]-'0')*10 + (imageName[5]-'0')) == i+1){
            dataset[i]->sample[index_cand].truelabel = 1;
         } else {
            dataset[i]->sample[index_cand].truelabel = 2;
         }
      }

		index_cand++;

		iftDestroyImage(&img);
		iftDestroyImage(&img_mag);
		iftDestroyImage(&img_orient);

		t2 = iftToc();
		printf("%s - ", iftBasename(digits_dir->files[l]->pathname));
		printf("%dth image: extract features in %f ms\n", l+1, iftCompTime(t1, t2));
   }


   for (int l = 0; l < letters_dir->nfiles ; l++) {
      t1 = iftTic();
      img = iftReadImageByExt(letters_dir->files[l]->pathname);

		iftImage *img_normalized = normalize(img);
		iftImage *img_mag;
		iftImage *img_orient;
		gradient(img_normalized, &img_mag, &img_orient);
		iftDestroyImage(&img_normalized);

		for(int i=0; i<img->xsize; i+=1) {
   		for(int j=0; j<img->ysize; j+=1) {
            feat = hog(img_mag, img_orient, i, j);
				for(int f=0; f<feat->n; f++) {
               for(int i=0; i<10+26; i++)
   					dataset[i]->sample[index_cand].feat[((i*img->xsize+j)*feat->n)+f] = feat->val[f];
				}
            iftDestroyFeatures(&feat);
         }
      }

      // Define a label a partir do nome do arquivo.
      //// Para as imagens de digitos, esse valor fica na sexta posição do nome da imagem.
      char *imageName = basename(letters_dir->files[l]->pathname);
      printf("Digito: %d\n", ((imageName[4]-'0')*10 + (imageName[5]-'0')));
      for(int i=0; i<10+26; i++){
         if(((imageName[4]-'0')*10 + (imageName[5]-'0') + 10) == i+1){
            dataset[i]->sample[index_cand].truelabel = 1;
         } else {
            dataset[i]->sample[index_cand].truelabel = 2;
         }
      }

		index_cand++;

		iftDestroyImage(&img);
		iftDestroyImage(&img_mag);
		iftDestroyImage(&img_orient);

		t2 = iftToc();
		printf("%s - ", iftBasename(letters_dir->files[l]->pathname));
		printf("%dth image: extract features in %f ms\n", l+1, iftCompTime(t1, t2));
   }

    // Write candidates dataset
    for(int i=0; i<10+26; i++){
       char aux[1000];
       sprintf(aux, "%s_%2d", datasetPath, i);
       iftWriteOPFDataSet(dataset[i], aux);

       // Free
       iftDestroyDataSet(&dataset[i]);
    }

    // Free
    iftDestroyDir(&digits_dir);
    iftDestroyDir(&letters_dir);

    MemDinFinal = iftMemoryUsed();
    if (MemDinInicial!=MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial,MemDinFinal);

    return(0);
}
