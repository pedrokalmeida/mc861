import sys
import glob
import cv2
import numpy as np

#segmenta caracteres

def segmenta_char1(img, largura, altura):
    char1 = img[0:altura, 0:largura/6]
    return  char1;

def segmenta_char2(img, largura, altura):    
    char2 = img[0:altura, largura/6:2*largura/6]
    return  char2;
    
def segmenta_char3(img, largura, altura):    
    char3 = img[0:altura, 2*largura/6:3*largura/6]
    return  char3;

def segmenta_char4(img, largura, altura):    
    char4 = img[0:altura, (3*largura/6)-5:(4*largura/6)-5]
    return  char4;

def segmenta_char5(img, largura, altura):    
    char5 = img[0:altura, (4*largura/6)-5:(5*largura/6)-5]
    return  char5;

def segmenta_char6(img, largura, altura):    
    char6 = img[0:altura, (5*largura/6)-5:largura]
    return  char6;
    
def segmenta_char7(img, largura, altura):    
    char7 = img[0:altura, 5*largura/6:largura]
    return  char7;
        

# remove borda preta [0,0,0]
def remove_borda(img):
    largura = img.shape[1]
    altura = img.shape[0]
    menor_x = largura
    maior_x = 0
    menor_y = altura
    maior_y = 0
    for y in range(0,altura):
        for x in range(0,largura):
            if img[y,x][1]>0:
                if x < menor_x:
                    menor_x = x
                if x > maior_x:
                    maior_x = x
                if y < menor_y:
                    menor_y = y
                if y > maior_y:
                    maior_y = y
    crop_img = img[menor_y:maior_y, menor_x:maior_x]
    return crop_img

# ajusta orientacao das letras das placas
def ajusta_imagem(img_path):
    img = cv2.imread(img_path)
    rows = img.shape[0]
    cols = img.shape[1]

    pts1 = np.float32([[0,0],[cols,0],[0,rows],[cols,rows]])
    pts2 = np.float32([[30,50],[cols+30,0],[40,rows+50],[cols+40,rows]])

    M = cv2.getPerspectiveTransform(pts1,pts2)
    dst = cv2.warpPerspective(img,M,(500,500))
    return dst

# main
def main():
    # sem argumentos necessarios
    if (len(sys.argv) < 3):
        print "informe o diretorio de input e de output"
        exit()
    input_dir, output_dir = sys.argv[1], sys.argv[2]
    file_list = glob.glob(input_dir + "*.pgm")
    
    for image in file_list:
        ajustada = ajusta_imagem(image)
        cortada = remove_borda(ajustada)
        cv2.imwrite(output_dir+(image[len(input_dir):]), cortada)
        largura = cortada.shape[1]
        altura = cortada.shape[0]
        char1 = segmenta_char1(cortada, largura, altura)
        char2 = segmenta_char2(cortada, largura, altura)
        char3 = segmenta_char3(cortada, largura, altura)
        char4 = segmenta_char4(cortada, largura, altura)
        char5 = segmenta_char5(cortada, largura, altura)
        char6 = segmenta_char6(cortada, largura, altura)
        char7 = segmenta_char7(cortada, largura, altura)
        cv2.imwrite(output_dir+"c1_"+(image[len(input_dir):]), char1)
        cv2.imwrite(output_dir+"c2_"+(image[len(input_dir):]), char2)
        cv2.imwrite(output_dir+"c3_"+(image[len(input_dir):]), char3)
        cv2.imwrite(output_dir+"c4_"+(image[len(input_dir):]), char4)
        cv2.imwrite(output_dir+"c5_"+(image[len(input_dir):]), char5)
        cv2.imwrite(output_dir+"c6_"+(image[len(input_dir):]), char6)
        cv2.imwrite(output_dir+"c7_"+(image[len(input_dir):]), char7) 
        print "imagem " + image + " completada com sucesso."
if __name__ == "__main__":
    main()