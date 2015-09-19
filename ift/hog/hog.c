#include "ift.h"
#include "iftExtractFeatures.h"
#include "iftSelectCandidates.h"
#include "iftHistogram.h"
#include <math.h>

#define HOG_K 4000
#define HOG_L 4095
#define HOG_r_normalize 5.0
#define HOG_r_gradient 3.0

// tamanho janela de detecção (em pixels)
#define HOG_N1 120 
#define HOG_M1 50

// tamanho célula (em pixels)
#define HOG_N2 6
#define HOG_M2 6

// tamanho bloco (em células)
#define HOG_N3 3
#define HOG_M3 3

iftImage *firstStep_normalize_v1(iftImage *orig) {
	iftAdjRel *adj = iftCircular(HOG_r_normalize);
	
	iftImage *img_normalized = iftNormalizeImage(orig, adj, HOG_L);
	
	iftDestroyAdjRel(&adj);
	
	return img_normalized;
}

iftImage *firstStep_normalize_v2(iftImage *orig) {
    iftAdjRel *adj = iftCircular(HOG_r_normalize);

    iftImage* img_normalized = iftCreateImage(orig->xsize, orig->ysize, orig->zsize);

    for (int p = 0; p < orig->n; ++p) {
		if(orig->val[p] == 0) { //verificação para garantir que nunca tenha divisão por zero
			img_normalized->val[p] = orig->val[p];
		} else {
			iftVoxel v = iftGetVoxelCoord(orig, p); 
			int s = 0;
			for (int i = 0; i < adj->n; ++i) { 
				iftVoxel u = iftGetAdjacentVoxel(adj, v, i);
				
				if(iftValidVoxel(orig, u)) {
					int q = iftGetVoxelIndex(orig, u);
					s+= (orig->val[q] * orig->val[q]);
				}
			}
			img_normalized->val[p] = (orig->val[p] / sqrt(s)) * HOG_K; //Fórmula de normalização
		}
    }

	iftDestroyAdjRel(&adj);
    
    return img_normalized;
}

iftImage *secondStep_gradient_v1(iftImage *img) {	
    iftAdjRel *adj = iftCircular(HOG_r_gradient);
	iftImage *img_mag = iftImageGradientMagnitude(img, adj);
	iftDestroyAdjRel(&adj);
	return img_mag;
}

iftImage *secondStep_gradient_v1_sobel(iftImage *img) {	
    iftImage *img_mag_sobel = iftSobelGradientMagnitude(img);
	return img_mag_sobel;
}

void secondStep_gradient_v2(iftImage *img, char *pathname_mag, char *pathname_orient, iftImage **g_mag, iftImage **g_orient) {	
    iftAdjRel *adj = iftCircular(HOG_r_gradient);
    
    iftKernel* Kx = iftUniformKernel(adj);
    iftKernel* Ky = iftUniformKernel(adj);

	iftVoxel v;
	v.x = HOG_r_gradient;
	v.y = HOG_r_gradient;
	v.z = 0;
	
	double c = 2*pow((HOG_r_gradient/3),2.0);
	
	for(int i=1; i<adj->n; ++i) {
		iftVoxel u = iftGetAdjacentVoxel(adj, v, i);
		
		double dist = iftVoxelDistance(u, v);
		int dist_x = u.x - v.x;
		double Wx = exp((-1) * dist * dist / c) *dist_x/dist;
		Kx->weight[i] = Wx; 
		
		int dist_y = u.y - v.y;
		double Wy = exp((-1) * dist * dist / c)*dist_y/dist;		
		Ky->weight[i] = Wy; 
	}
	
	iftImage* img_mag = iftCreateImage(img->xsize, img->ysize, img->zsize);
	iftImage* img_orient = iftCreateImage(img->xsize, img->ysize, img->zsize);

    for (int i = 0; i < img->n; ++i) { 
		iftVoxel p = iftGetVoxelCoord(img, i);
		
		double Gx = 0.0;
		double Gy = 0.0;
		
		for (int j = 1; j < adj->n; ++j) {
			iftVoxel q = iftGetAdjacentVoxel(adj, p, j);	
			
			if(iftValidVoxel(img, q)) { 
				int k = iftGetVoxelIndex(img, q);
				Gx += (img->val[k] - img->val[i]) * Kx->weight[j];
				Gy += (img->val[k] - img->val[i]) * Ky->weight[j]; 
			}
		}
		double G = sqrt(Gx*Gx + Gy*Gy); 
		img_mag->val[i] = G;
		
		Gx = Gx/G;
		Gy = Gy/G;
		
		if(Gy >= 0.0) {
			img_orient->val[i] = acos(Gx)*180/PI;
		} else {
			img_orient->val[i] = 360 - acos(Gx)*180/PI;
		}
	}
	
	iftWriteImageP2(img_mag, pathname_mag);
	iftWriteImageP2(img_orient, pathname_orient);
	
	iftDestroyAdjRel(&adj);
	iftDestroyKernel(&Kx);
	iftDestroyKernel(&Ky);
	
	//iftDestroyImage(&img_mag);
	*g_mag = img_mag;
	//iftDestroyImage(&img_orient);
	*g_orient = img_orient;
}

/*
 * ARTICLE!!!!
 * 
 * Tamanho de células com menor taxa de erros: 6x6 (n2 x m2 em pixels)
 * Tamanho de blocos com menor taxa de erros: 3x3 (n3 x m3 em células)
 */

double ddouble(int a){
	return (double)a;
}

double nzdouble(int a){
	return ((double)a)+0.00001;
}

iftHistogram *thirdStep_histogram(iftImage *img, iftImage *g_mag, iftImage *g_orient, iftImage* bb_img) {
	// Definicao da janela n1xm1 em torno de bb_img
	int n1 = bb_img->xsize;
	int m1 = bb_img->ysize;
	// Divisao da janela em celulas inteiras n2xm2
	int n2=6;
	int m2=6;

	// Gambi01: recortamos a imagem para ter um numero inteiro de celulas
	n1 = (n1/n2)*n2;
	m1 = (m1/m2)*m2;

	// Geracao de um histograma por celula, utilizando a orientacao
	int numCelulas = (n1*m1)/(n2*m2);
	int numBins = 9;

	// Aloca e limpa memoria para os histogramas.
	iftDataSet* histogramas = iftCreateDataSet(numCelulas, numBins);
	for(int i=0; i<numCelulas; i++){
		for(int j=0; j<numBins; j++){
			histogramas->sample[i].feat[j] = 0;
		}
		histogramas->sample[i].id = i;
	}

	for(int i=0; i<n1; i++){
		for(int j=0; j<m1; j++){
			int celula;
			int bin;
			int theta = 45;

			celula = (j/m2) + (m1/m2)*(i/n2);

			if(g_orient->val[i+n1*m1] == 0){
				bin = 0;
			} else {
				bin = g_orient->val[i+n1*m1]/theta;
			}

			if (celula < numCelulas && bin < numBins)
				histogramas->sample[celula].feat[bin]++;
			else
				printf("Erro ao gerar histogramas -> celula:%d, bin:%d.\n", celula, bin);
		}
	}


	printf ("Histograma Gerado!\n");
	// Normalizacao dos histogramas com base nos blocos
	iftDataSet* histogramasNormalizados = iftCreateDataSet(numCelulas, numBins);
	for(int i=0; i<numCelulas; i++){
		for(int j=0; j<numBins; j++){
			histogramasNormalizados->sample[i].feat[j] = 0.0;
		}
		histogramasNormalizados->sample[i].id = i;
	}
	printf ("Histograma Gerado!\n");

	for(int i=0; i<n1; i++){
		for(int j=0; j<m1; j++){
			int celula1 = (j/m2) + (m1/m2)*(i/n2);
			int celula2 = celula1 + 1;
			int celula3 = (j/m2) + (m1/m2)*(1 + (i/n2));
			int celula4 = celula3 + 1;
			int theta = 45;
			
			int bin1;
			if(g_orient->val[i+n1*m1] == 0){
				bin1 = 0;
			} else {
				bin1 = ((double)g_orient->val[i+n1*m1] - ((double)theta)/2.0)/((double)theta);
			}

			int bin2;
			if(g_orient->val[i+n1*m1] == 0){
				bin2 = 1;
			} else {
				bin2 = ((double)g_orient->val[i+n1*m1] + ((double)theta)/2.0)/((double)theta);g_orient->val[i+n1*m1]/theta;
			}

			int x = i/n2;
			int x1 = (ddouble(i)-0.5*ddouble(n1))/ddouble(n1);
			int x2 = (ddouble(i)+0.5*ddouble(n1))/ddouble(n1);
			int x3 = x1;
			int x4 = x1;
			int x5 = x2;
			int x6 = x2;
			int x7 = x3;
			int x8 = x4;
			int x9 = x6;
			int x10 = x5;
			int x11 = x3;
			int x12 = x4;
			int x13 = x6;
			int x14 = x5;

			int y = j/m2;
			int y1 = y;
			int y2 = y;
			int y3 = (ddouble(j)-0.5*ddouble(m1))/ddouble(m1);
			int y4 = (ddouble(j)+0.5*ddouble(m1))/ddouble(m1);
			int y5 = y3;
			int y6 = y4;
			int y7 = y3;
			int y8 = y4;
			int y9 = y6;
			int y10 = y7;
			int y11 = y7;
			int y12 = y8;
			int y13 = y12;
			int y14 = y11;

			int z = g_orient->val[i+n1*m1];
			int z1 = z;
			int z2 = z;
			int z3 = z;
			int z4 = z;
			int z5 = z;
			int z6 = z;
			int z7 = z+1;
			int z8 = z7;
			int z9 = z7;
			int z10 = z7;
			int z11 = z-1;
			int z12 = z11;
			int z13 = z11;
			int z14 = z11;

			double w = g_mag->val[i+n1*m1];
			double w1 = (w/nzdouble(x2-x1))*(nzdouble(x2-x));
			double w2 = (w/nzdouble(x2-x1))*(nzdouble(x-x1));
			double w3 = (w1/nzdouble(y3-y4))*(nzdouble(y1-y4));
			double w4 = (w1/nzdouble(y3-y4))*(nzdouble(y3-y1));
			double w5 = (w2/nzdouble(y5-y6))*(nzdouble(y2-y6));
			double w6 = (w2/nzdouble(y5-y6))*(nzdouble(y5-y2));
			double w7 = (w3/nzdouble(z11-z7))*(nzdouble(z11-z3));
			double w11 = (w3/nzdouble(y11-z7))*(nzdouble(z3-z7));
			double w8 = (w4/nzdouble(z12-z8))*(nzdouble(z12-z4));
			double w12 = (w4/nzdouble(z12-z8))*(nzdouble(z4-z8));
			double w10 = (w5/nzdouble(z14-z10))*(nzdouble(z14-z5));
			double w14 = (w5/nzdouble(z14-z10))*(nzdouble(z5-z10));
			double w9 = (w6/nzdouble(z13-z9))*(nzdouble(z13-z6));
			double w13 = (w6/nzdouble(z13-z9))*(nzdouble(z6-z9));

			if(celula1 < numCelulas){
				histogramasNormalizados->sample[celula1].feat[bin1]+=w8;
				histogramasNormalizados->sample[celula1].feat[bin2]+=w12;
			}
			if(celula2 < numCelulas){
				histogramasNormalizados->sample[celula2].feat[bin1]+=w9;
				histogramasNormalizados->sample[celula2].feat[bin2]+=w13;
			}
			if(celula3 < numCelulas){
				histogramasNormalizados->sample[celula3].feat[bin1]+=w7;
				histogramasNormalizados->sample[celula3].feat[bin2]+=w11;
			}
			if(celula4 < numCelulas){
				histogramasNormalizados->sample[celula4].feat[bin1]+=w10;
				histogramasNormalizados->sample[celula4].feat[bin2]+=w14;
			}
		}
	}

	// Concatenacao e normalizacao dos histogramas dentro dos blocos
	int n3 = 6;
	int m3 = 6;
	int numBlocos = numCelulas/(n3*m3);
	// Gambi02: jogando algumas celulas fora.
	numCelulas = numBlocos*(n3*m3);
	iftDataSet* histogramasConcatNormalizados = iftCreateDataSet(numBlocos, (n3*m3)*(n2*m2)*numBins);
	for(int i=0; i<numBlocos; i++){
		for(int j=0; j<(n3*m3)*(n2*m2)*numBins; j++){
			histogramasConcatNormalizados->sample[i].feat[j] = 0;
		}
		histogramasConcatNormalizados->sample[i].id = i;
	}

	// Concatena
	for(int i=0; i<numCelulas; i++){
		int bloco = i/(n3*m3);
		for(int j=0; j<numBins; j++){
			histogramasConcatNormalizados->sample[bloco].feat[i*numBins+j] += histogramasNormalizados->sample[i].feat[j];
		}
	}
	printf ("Primeira concatenação!\n");

	// Normaliza
	for (int i=0; i<numBlocos; i++){
		double meanGeo = 0.00001;
		for(int j=0; j<(n3*m3)*(n2*m2)*numBins; j++){
			meanGeo += histogramasConcatNormalizados->sample[i].feat[j]*histogramasConcatNormalizados->sample[i].feat[j];
		}
		meanGeo = sqrt(meanGeo);
		for(int j=0; j<(n3*m3)*(n2*m2)*numBins; j++){
			histogramasConcatNormalizados->sample[i].feat[j] = histogramasConcatNormalizados->sample[i].feat[j] / meanGeo;
		}
	}
	printf ("Segunda normalização!\n");

	// Concatena tudo para ter o HoG
	double hisSum = 0.0;
	iftHistogram *hog = iftCreateHistogram(numBlocos*(n3*m3)*(n2*m2)*numBins);
	for (int i=0; i<numBlocos; i++){
		for(int j=0; j<(n3*m3)*(n2*m2)*numBins; j++){
			hisSum += histogramasConcatNormalizados->sample[i].feat[j];
			hog->val[i*((n3*m3)*(n2*m2)*numBins) + j] = histogramasConcatNormalizados->sample[i].feat[j];
		}
	}
	printf ("HoG gerado:%lf!\n", hisSum);

	return hog;
}

int main() {
	int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

	iftImage *img = iftReadImageByExt("orig/0009.pgm");
	
	//iftImage *img_v1 = firstStep_normalize_v1(img);
	//iftWriteImageP2(img_v1, "result/0009_v1_r8_L4095.pgm");
	
	iftImage *img_v2 = firstStep_normalize_v2(img);
	//iftWriteImageP2(img_v2, "result/0009_v2_r5_K4000.pgm");
	

	iftImage *g_mag;
	iftImage *g_orient;
	secondStep_gradient_v2(img_v2, "result/0009_mymag_r3.pgm", "result/0009_myorient_r3.pgm", &g_mag, &g_orient);

	iftImage *candidates = selectCandidates(img);
	iftWriteImageP2(candidates, "result/0009_candidates.pgm");

	int numCandidates = iftMaximumValue(candidates);

        // Select positive and negative examples
        for (int j = 0; j < numCandidates ; j++) {
		char candidateName[100];
            // Get bounding box
		iftImage* bb_img = iftCreateBoundingBox2D(img, candidates, (j+1));
		sprintf(candidateName, "result/0009_bb_img_candidate_%d.pgm", j);
		iftWriteImageP2(bb_img, candidateName);

		iftHistogram *hog;
		hog = thirdStep_histogram(img, g_mag, g_orient, bb_img);
		iftWriteHistogram(hog, "result/0009_hog.pgm");
	}
	
    iftDestroyImage(&img);
    //iftDestroyImage(&img_v1);
	iftDestroyImage(&img_v2);
	
	MemDinFinal = iftMemoryUsed();
    if (MemDinInicial != MemDinFinal)
        printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
               MemDinInicial, MemDinFinal);
}

