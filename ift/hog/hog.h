#include "ift.h"

#define HOG_K 2000
#define HOG_L 4095
#define HOG_r_normalize 5.0
#define HOG_r_gradient 3.0

// tamanho janela de detecção (em pixels)
#define HOG_N1 120
#define HOG_M1 60

// tamanho célula (em pixels)
#define HOG_N2 12
#define HOG_M2 12

// tamanho bloco (em células)
#define HOG_N3 2
#define HOG_M3 2

#define DEBUG_ON 1

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

iftImage *normalize(iftImage *orig) {
	return firstStep_normalize_v2(orig);
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

void secondStep_gradient_v2(iftImage *img, iftImage **g_mag, iftImage **g_orient) {	
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
	iftDestroyAdjRel(&adj);
	iftDestroyKernel(&Kx);
	iftDestroyKernel(&Ky);
	*g_mag = img_mag;
	*g_orient = img_orient;
}

void gradient(iftImage *img, iftImage **g_mag, iftImage **g_orient) {
	secondStep_gradient_v2(img, g_mag, g_orient);
}

void celulas_adjacentes(int i, int j, int *celula1, int *celula2, int *celula3, int *celula4) {
	int celula =  i/HOG_N2 + (j/HOG_N2)*(HOG_N1/HOG_N2);
	int x_centro = ((celula % (HOG_N1/HOG_N2)) * HOG_N2 + HOG_N2/2);
	int y_centro = ((celula /(HOG_N1/HOG_N2)) * HOG_M2 + HOG_M2/2);
	if(i < x_centro) {
		if(j < y_centro) {
			(*celula4) = celula;
			(*celula3) = (*celula4) - 1;
			(*celula2) = (*celula4) - (HOG_N1/HOG_N2);
			(*celula1) = (*celula2) - 1;

			if(((*celula2) % (HOG_N1/HOG_N2)) == 0)
				(*celula1) = -1;
			if(((*celula4) % (HOG_N1/HOG_N2)) == 0)
				(*celula3) = -1;

		} else {
			
			(*celula2) = celula;
			(*celula1) = (*celula2) - 1;			
			(*celula4) = (*celula2) + (HOG_N1/HOG_N2);
			(*celula3) = (*celula4) - 1;
			
			if(((*celula2) % (HOG_N1/HOG_N2)) == 0)
				(*celula1) = -1;
			if(((*celula4) % (HOG_N1/HOG_N2)) == 0)
				(*celula3) = -1;
		}
	} else {
		if(j < y_centro) {
			(*celula3) = celula;
			(*celula4) = (*celula3) + 1;
			(*celula1) = (*celula3) - (HOG_N1/HOG_N2);
			(*celula2) = (*celula1) + 1;

			if(((*celula2) % (HOG_N1/HOG_N2)) == 0)
				(*celula2) = -1;	
			if(((*celula4) % (HOG_N1/HOG_N2)) == 0)
				(*celula4) = -1;	
		} else {
			(*celula1) = celula;
			(*celula2) = (*celula1) + 1;
			(*celula3) = (*celula1) + (HOG_N1/HOG_N2);
			(*celula4) = (*celula3) + 1;

			if(((*celula2) % (HOG_N1/HOG_N2)) == 0)
				(*celula2) = -1;		
			if(((*celula4) % (HOG_N1/HOG_N2)) == 0)
				(*celula4) = -1;	
		}
	}
}

int coordenada_centro_eixox( int celulaA, int celulaB) {
	if(celulaA < 0) {
		if(celulaB < 0)
			return 0;
		return (celulaB % (HOG_N1/HOG_N2)) * HOG_N2 + HOG_N2/2;
	} 
	return (celulaA % (HOG_N1/HOG_N2)) * HOG_N2 + HOG_N2/2;	
}

int coordenada_centro_eixoy(int celulaA, int celulaB) {
	if(celulaA < 0) {
		if(celulaB < 0)
			return 0;
		return ((celulaB /(HOG_N1/HOG_N2)) * HOG_M2 + HOG_M2/2);
	}
	return ((celulaA /(HOG_N1/HOG_N2)) * HOG_M2 + HOG_M2/2);
}

iftFeatures *thirdStep_histogram(iftImage *g_mag, iftImage *g_orient, int x0, int y0) {
	// Gera um histograma por célula, considerando 9 orientações(=bins)
	int numCelulas = (HOG_N1*HOG_M1)/(HOG_N2*HOG_M2);
	int numBins = 9;

	iftHistogram **histogramas = malloc(sizeof(iftHistogram)*numCelulas);    
	for(int i=0; i<numCelulas; i++) {
		histogramas[i] = iftCreateHistogram(numBins);
		for(int j=0; j<numBins; j++) {
			histogramas[i]->val[j] = 0.0;
		}
	}
	
	int theta = 45;
		
	for(int i=0; i<HOG_N1; i++){
		for(int j=0; j<HOG_M1; j++){
			int x = i+x0;
			int y = j+y0;
			
			iftVoxel v = iftGetVoxelCoord(g_mag, x + y*g_mag->xsize);
			if(!iftValidVoxel(g_mag, v)) {
				continue;
			}
			int z = g_orient->val[x + y*g_mag->xsize];
			
			//if(DEBUG_ON)
				//printf("x=%d, y=%d, p=%d, val=%d\n", x, y, x+y*g_mag->xsize, z);
	
			int bin1;
			if(g_mag->val[x + y*g_mag->xsize] == 0) {
				bin1 = 0;
			} else {
				if(((int)(z - theta/2.0)) < 0) { 
					bin1 = 8;
				} else {
					bin1 = ((int)(z - theta/2.0)/theta + 1) % 9;
				}
			}

			int bin2;
			if(g_mag->val[x + y*g_mag->xsize] == 0) {
				bin2 = 0;
			} else {
				bin2 = ((int)(z + theta/2.0)/theta + 1);
				if(bin2 == 9)
					bin2 = 1;
			}
						
			int z1 = bin1*theta - theta/2.0;
			int z2 = bin2*theta - theta/2.0;
					
			int celula1, celula2, celula3, celula4;	
			celulas_adjacentes(i, j, &celula1, &celula2, &celula3, &celula4);

			int x_q1q3 = coordenada_centro_eixox(celula1, celula3);
			int x_q2q4 = coordenada_centro_eixox(celula2, celula4);
			int y_q1q2 = coordenada_centro_eixoy(celula1, celula2);
			int y_q3q4 = coordenada_centro_eixoy(celula3, celula4);
			
			double w = g_mag->val[x + y*g_mag->xsize];
		
			x = i;
			y = j;
		
			double w1 = w*(x_q2q4-x)/HOG_N2;
			if(w1 < 0.0) 
				w1 = 0.0;
			double w2 = w*(x-x_q1q3)/HOG_N2;
			if(w2 < 0.0) 
				w2 = 0.0;
				
			double w3 = w1*(y-y_q1q2)/HOG_M2;
			double w4 = w1*(y_q3q4-y)/HOG_M2;
			double w5 = w2*(y-y_q1q2)/HOG_M2;
			double w6 = w2*(y_q3q4-y)/HOG_M2;
			
			//if(DEBUG_ON)
				//printf("pesos: w=%.1f, w1=%.1f, w2=%.1f, w3=%.1f, w4=%.1f, w5=%.1f, w6=%.1f\n", w, w1, w2, w3, w4, w5, w6);
			
			int dif_z = z2 - z;
			if(dif_z < 0)
				dif_z += 360;
			double w7 = w3*(dif_z)/theta;
			double w8 = w4*(dif_z)/theta;
			double w9 = w6*(dif_z)/theta;
			double w10 = w5*(dif_z)/theta;
			
			dif_z = z - z1;
			if(dif_z < 0)
				dif_z += 360;
			double w11 = w3*(dif_z)/theta;
			double w12 = w4*(dif_z)/theta;
			double w13 = w6*(dif_z)/theta;
			double w14 = w5*(dif_z)/theta;
						
			if(celula1 >=0 && celula1 < numCelulas) {
				histogramas[celula1]->val[bin1]+=w8;
				histogramas[celula1]->val[bin2]+=w12;
			}
			if(celula2 >=0 && celula2 < numCelulas) {
				histogramas[celula2]->val[bin1]+=w9;
				histogramas[celula2]->val[bin2]+=w13;
			}
			if(celula3 >=0 && celula3 < numCelulas) {
				histogramas[celula3]->val[bin1]+=w7;
				histogramas[celula3]->val[bin2]+=w11;
			}
			if(celula4 >=0 && celula4 < numCelulas) {
				histogramas[celula4]->val[bin1]+=w10;
				histogramas[celula4]->val[bin2]+=w14;
			}
			/*
			if(DEBUG_ON) {
				printf("x1x3=%d, x2x4=%d, y1y2=%d, y3y4=%d, z1=%d, z2=%d\n", x_q1q3, x_q2q4, y_q1q2, y_q3q4, z1, z2);
				printf("células: %d %d %d %d\n", celula1, celula2, celula3, celula4);
				printf("pesos: (%.1f %.1f %.1f %.1f) (%.1f %.1f %.1f %.1f)\n", w8, w9, w7, w10, w12, w13, w14, w11);
				for(int k=0; k<numCelulas; k++) {
					for(int b=0; b<numBins; b++) {
						printf("%.1f ", histogramas[k]->val[b]);
					}
					printf("\n");
				}
			}*/
		}	
	}	
	/*if(DEBUG_ON) {
		for(int i=0; i<numCelulas; i++) {
			printf("Histograma celula %d: ", i);
			float s = 0;
			for(int b=0; b<numBins; b++) {
				printf("%.2f ", histogramas[i]->val[b]);
				s += histogramas[i]->val[b];
			}
			printf(" ===> s=%.1f\n", s);
		}
	}*/
	
	//Concatena
	int numBlocos = (HOG_N1/HOG_N2-HOG_N3+1)*(HOG_M1/HOG_M2-HOG_M3+1);
	iftDataSet *histogramasConcatNormalizados = iftCreateDataSet(numBlocos, HOG_N3*HOG_M3*numBins);

	int numCelulasHorizontal = (HOG_N1/HOG_N2);
	int numBlocosHorizontal = (HOG_N1/HOG_N2-HOG_N3+1);
	
	float *soma = malloc(sizeof(float)*numBlocos);
	for(int k=0; k<numBlocos; k++) {
		soma[k] = 0.0;
		for(int i=0; i<HOG_N3; i++) {
			int celula = (k/numBlocosHorizontal)*numCelulasHorizontal + i*numCelulasHorizontal + k%(numCelulasHorizontal-HOG_N3+1);
			for(int j=0; j<HOG_M3; j++) {
				//printf("Bloco %d, feat %d, celula %d \n", k, i*HOG_N3*numBins+j*numBins, celula+j);
				for(int b=0; b<numBins; b++) {
					histogramasConcatNormalizados->sample[k].feat[i*HOG_N3*numBins+j*numBins+b] = histogramas[celula+j]->val[b];
					soma[k] += histogramas[celula+j]->val[b];
				}
			}
		}
	}
	
	/*if(DEBUG_ON) {
		for(int i=0; i<numBlocos; i++) {
			printf("bloco %d %1.f- soma = %.1f\n", i, soma[i],sqrt(soma[i]));
		}
	}*/
	
	for(int i=0; i<numCelulas; i++) {
		iftDestroyHistogram(&histogramas[i]);
	}
	free(histogramas);
	
	//Normaliza e obtém hog
	iftFeatures *hog = iftCreateFeatures(numBlocos*HOG_N3*HOG_M3*numBins);
	for(int k=0; k<numBlocos; k++) {
		for(int i=0; i<HOG_N3*HOG_M3*numBins; i++) {
			hog->val[k*histogramasConcatNormalizados->nfeats + i] = 
				histogramasConcatNormalizados->sample[k].feat[i]/(sqrt(soma[k]) + 0.000001);
		}
	}
	free(soma);	
	iftDestroyDataSet(&histogramasConcatNormalizados);
	return hog;
}

iftFeatures *hog(iftImage *g_mag, iftImage *g_orient, int x, int y) {
	
	int x0 = x - HOG_N1/2;
	int xn = x0 + HOG_N1;
	int y0 = y - HOG_M1/2;
	int yn = y0 + HOG_M1;
	
	//if(x0 >= 0 && xn < g_mag->xsize && y0 >= 0 && yn < g_mag->ysize)
		return thirdStep_histogram(g_mag, g_orient, x0, y0); 
	
	int numBlocos = (HOG_N1/HOG_N2-HOG_N3+1)*(HOG_M1/HOG_M2-HOG_M3+1);
	iftFeatures *hog_zero = iftCreateFeatures(numBlocos*HOG_N3*HOG_M3*9);
	for(int i=0; i<hog_zero->n; i++) {
		hog_zero->val[i] = 0.0;
	}
	return hog_zero;
}

int countNumPixelsCandidates(iftImage *candImg) {
	int num = 0;
	for(int i=0; i<candImg->xsize; i+=3) {
	for(int j=0; j<candImg->ysize; j+=3) {
		int p = i + j*candImg->xsize;
	//for (int p = 0; p < candImg->n; ++p) {
        if (candImg->val[p] > 0) {
            num++;
		}
    }}
    return num;	
}

iftVoxel iftCreateBoundingBox2D(iftImage *label, int val, iftImage *img_mag, iftImage *img_orient, iftImage **out_mag, iftImage **out_orient) {
    int minX, minY, maxX, maxY;
    iftVoxel u;
    int p, i, j, origp;
    minX = INFINITY_INT;
    minY = INFINITY_INT;
    maxX = -1;
    maxY = -1;
    // Find bounding box
    for (p=0; p < label->n; p++) {
        if (label->val[p] == val) {
            u = iftGetVoxelCoord(label,p);
            if (u.x < minX)
                minX = u.x;
            else if (u.x > maxX)
                maxX = u.x;

            if (u.y < minY)
                minY = u.y;
            else if (u.y > maxY)
                maxY = u.y;
        }
    }
    
    int bordaX = HOG_N1/2;
    int bordaY = HOG_M1/2;
	
	u = iftGetVoxelCoord(img_mag, minY*img_mag->xsize+minX);
	minX = minX-bordaX;
    maxX = maxX+bordaX;
    minY = minY-bordaY;
    maxY = maxY+bordaY;
    
    if(minX < 0)
		minX = 0;
	if(maxX > img_mag->xsize) 
		maxX = img_mag->xsize;
	if(minY < 0)
		minY = 0;
	if(maxY > img_mag->ysize) 
		maxY = img_mag->ysize;
    
    iftImage *bb_mag = iftCreateImage((maxX-minX+1), (maxY-minY+1), img_mag->zsize);
	iftImage *bb_orient = iftCreateImage((maxX-minX+1), (maxY-minY+1), img_mag->zsize);
	
    // Copy pixel values		
    for (i=minY; i < (maxY+1); i++) {
        for (j=minX; j < (maxX+1); j++) {
            origp = i * img_mag->xsize + j;
            p = (i-minY) * (maxX-minX+1) + (j-minX);
            bb_mag->val[p] = img_mag->val[origp];
            bb_orient->val[p] = img_orient->val[origp];
        }
    }
    (*out_mag) = bb_mag;
    (*out_orient) = bb_orient;
    

	return u;
}

