from os import listdir
from os.path import isfile, join
from random import shuffle
import re
import numpy as np
from sklearn import svm
import sklearn.preprocessing as skpre
from sklearn.neighbors import KNeighborsClassifier
import sklearn.decomposition as deco

digitsPath = "./gray-digits.20x16.0.0/"
lettersPath = "./gray-letters.20x16.0.0/"

def read_pgm(filename, byteorder='>'):
    """Return image data from a raw PGM file as numpy array.

    Format specification: http://netpbm.sourceforge.net/doc/pgm.html

    """
    with open(filename, 'rb') as f:
        buffer = f.read()
    try:
        header, width, height, maxval = re.search(
            b"(^P5\s(?:\s*#.*[\r\n])*"
            b"(\d+)\s(?:\s*#.*[\r\n])*"
            b"(\d+)\s(?:\s*#.*[\r\n])*"
            b"(\d+)\s(?:\s*#.*[\r\n]\s)*)", buffer).groups()
    except AttributeError:
        raise ValueError("Not a raw PGM file: '%s'" % filename)
    return np.frombuffer(buffer,
                            dtype='u1' if int(maxval) < 256 else byteorder+'u2',
                            count=int(width)*int(height),
                            offset=len(header)
                            ).reshape((int(height), int(width)))


def getImage(filename):
   image = []
   imageMatrix = read_pgm(filename)
   for imageVector in imageMatrix:
      for pixel in imageVector:
         image.append(float(pixel))

   return image

def getImageCategory(imageDir, imageName):
   if imageDir == digitsPath:
      return int(imageName[5]) - int('0')
   else:
      return int(imageName[5]) - int('0') + 10

def getImagesFromDir (imageDir):
	images = []
	imagesCategory = []
	for image in listdir(imageDir):
		if isfile(join(imageDir,image)):
			imageList = getImage(imageDir+image)
			images.append(imageList)
			imagesCategory.append(getImageCategory(imageDir, image))
	return [images, imagesCategory]

def computeSVM(trainData, testData):
   classifier = svm.SVC(gamma=10)

   classifier.fit(trainData[0], trainData[1])
   preds = classifier.predict(testData[0])
   accuracy = np.where(preds==testData[1], 1, 0).sum() / float(len(testData[0]))
   print "SVM accuracy: %3f" % (accuracy)


def computeKnn (trainData, testData):
	resultsK = []
	resultsAc = []
	for n in [1, 3, 5, 11, 17, 21]:
		neigh = KNeighborsClassifier(n_neighbors=n)
		neigh.fit(trainData[0], trainData[1])
		preds = neigh.predict(testData[0])
		import collections
		print collections.Counter(preds)
		accuracy = np.where(preds==testData[1], 1, 0).sum() / float(len(testData[0]))
		print "Neighbors: %d, Accuracy: %3f" % (n, accuracy)
		resultsK.append(n)
		resultsAc.append(accuracy)
	return [[resultsK, resultsAc], neigh]

# Calcula o pca dos dados.
def computePCA(data):
	pca = deco.PCA()
	pca.fit(data)
	# Pegaremos dimensoes ate obter 95% de representatividade.
	soma = 0
	for dim in range(len(pca.explained_variance_ratio_)):
		soma += pca.explained_variance_ratio_[dim]
		if soma > 0.95:
			break
	
	print "Dimensao reduzida de %d para %d" % (len(pca.explained_variance_ratio_), dim)
	
	return deco.PCA(n_components=dim).fit(data)

def getImages():
   digitsImages = getImagesFromDir(digitsPath)
   lettersImages = getImagesFromDir(lettersPath)

   images = [
      digitsImages[0] + lettersImages[0],
      digitsImages[1] + lettersImages[1]
      ]

   index = range(len(images[0]))
   normalizedImages = skpre.scale(images[0])
   # Divide em teste e treino.
   shuffle(index)
   imageVectors = []
   imageCategory = []
   for i in index:
      imageVectors.append(normalizedImages[i])
      imageCategory.append(images[1][i])

   imagesTest = [
         imageVectors[:len(imageVectors)/2],
         imageCategory[len(imageCategory)/2:]
      ]

   imagesTrain = [
         imageVectors[:len(imageVectors)/2],
         imageCategory[len(imageCategory)/2:]
      ]

   # Calcula PCA - Reducao de dimensionalidade dos dados. :)
   pca = computePCA(imagesTrain[0])
   transformedTrainData = pca.transform(imagesTrain[0])
   transformedTestData = pca.transform(imagesTest[0])

   return [[transformedTrainData, imagesTrain[1]], [transformedTrainData, imagesTrain[1]]]


def main():
   #digitsImages = getImagesFromDir(digitsPath)
   #print digitsImages[0][0], digitsImages[1][0]

   #results = computeKnn (digitsImages, digitsImages)[0]
   images = getImages()
   #results = computeKnn (images[0], images[1])[0]
   computeSVM(images[0], images[1])
   print "Encerrando"



main()
