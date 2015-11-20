from os import listdir
from os.path import isfile, join
from random import shuffle
import re
import numpy as np
from sklearn import svm
import sklearn.preprocessing as skpre
from sklearn.neighbors import KNeighborsClassifier
import sklearn.decomposition as deco
from sklearn.cross_validation import train_test_split
from sklearn.naive_bayes import BernoulliNB
from scipy.spatial.distance import cosine
from skimage.feature import hog

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
   #image = []
   imageMatrix = read_pgm(filename)
   
#   for imageVector in imageMatrix:
#      for pixel in imageVector:
#         image.append(float(pixel))

#   return image
   return hog(imageMatrix, orientations=8, pixels_per_cell=(5, 4), cells_per_block=(1, 1))

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

def get_score(expected, predictions):
	result = 0
	for i in range(len(predictions)):
		if predictions[i] == expected[i]:
			result += 1
	return result/float(len(expected))

def holdOutCross(dataTrain, dataTest, trainFunc, testMethod, hipergrid):
	best_score = -1
	for H in hipergrid:
		trained = trainFunc(dataTrain, H)
		score = testMethod(trained, dataTest)
		if score > best_score:
			best_score = score
			bestHiper = H
	return bestHiper

def trainFuncBNB(trainData, hiper):
	bnb = BernoulliNB(alpha=hiper)
	bnb.fit(trainData[0], trainData[1])
	return bnb

def testFuncBNB(trained, testData):
	predictions = trained.predict(testData[0])
	return get_score(testData[1], predictions)

def frange(x, y, jump):
	while x < y:
		yield x
		x += jump

def hipergridBNB(a, b, pace):
	for alpha in frange(a, b, pace):
		yield alpha

def computeBNB(trainData, testData):
	bestHiperBNB = holdOutCross(trainData, testData, trainFuncBNB, testFuncBNB, hipergridBNB(1e-6, 1e0, 0.01))
	
	trained = trainFuncBNB(trainData, bestHiperBNB)
	score = testFuncBNB(trained, testData)
	print "BNB accuracy: %3f" % (score)

def trainFuncSVMRBF(trainData, hiper):
	svc = svm.SVC(kernel='rbf', C=hiper[0], gamma=hiper[1])
	svc.fit(trainData[0], trainData[1])
	return svc

def testFuncSVMRBF(trained, testData):
	predictions = trained.predict(testData[0])
	return get_score(testData[1], predictions)

def hipergridSVMRBF(a, b, pace):
	for c in frange(a, b, pace):
		for gama in frange(a, b, pace):
			yield [c, gama]

def computeSVMRBF(trainData, testData):
	bestHiperRBFOVO = holdOutCross(trainData, testData, trainFuncSVMRBF, testFuncSVMRBF, hipergridSVMRBF(1e-3, 1e4, 1000))
	
	trained = trainFuncSVMRBF(trainData, bestHiperRBFOVO)
	score = testFuncSVMRBF(trained, testData)
	print "SVMRBF accuracy: %3f" % (score)

def trainFuncKNN(trainData, hiper):
	knn = KNeighborsClassifier(n_neighbors=hiper, metric=cosine)
	knn.fit(trainData[0], trainData[1])
	return knn

def testFuncKNN(trained, testData):
	predictions = trained.predict(testData[0])
	return get_score(testData[1], predictions)

def computeKNN(trainData, testData):
	bestKIDF = holdOutCross(trainData, testData, trainFuncKNN, testFuncKNN, range(1, 17, 2))
	
	trained = trainFuncKNN(trainData, bestKIDF)
	score = testFuncKNN(trained, testData)
	print "KNN accuracy: %3f" % (score)

def getImages():
   digitsImagesNormalized = getImagesFromDir(digitsPath)
   lettersImagesNormalized = getImagesFromDir(lettersPath)

   digitsImagesNormalized = [skpre.scale(digitsImagesNormalized[0]), digitsImagesNormalized[1]]
   lettersImagesNormalized = [skpre.scale(lettersImagesNormalized[0]), lettersImagesNormalized[1]]

   allImages = []
   for i in digitsImagesNormalized[0]:
      allImages.append(i)

   for i in lettersImagesNormalized[0]:
      allImages.append(i)

   # Divide em teste e treino.
   # Calcula PCA - Reducao de dimensionalidade dos dados. :)
   pca = computePCA(allImages)
   digitstransformedData = pca.transform(digitsImagesNormalized[0])
   letterstransformedData = pca.transform(lettersImagesNormalized[0])

   dtrainDataTF, dtestDataTF, dclassesTrainTF, dclassesTestTF = train_test_split(digitstransformedData, digitsImagesNormalized[1], train_size=0.65)

   ltrainDataTF, ltestDataTF, lclassesTrainTF, lclassesTestTF = train_test_split(letterstransformedData, lettersImagesNormalized[1], train_size=0.65)
   
   return [[dtrainDataTF, dclassesTrainTF], [dtestDataTF, dclassesTestTF]], [[ltrainDataTF, lclassesTrainTF], [ltestDataTF, lclassesTestTF]]

def main():
   imagesDigits, imagesLetters = getImages()
   method = computeKNN
   # computeBNB      #BNB accuracy: 0.869702 | 0.580392
   # computeSVMRBF   #SVMRBF accuracy: 0.968603 | 0.847059
   # computeKNN      #KNN accuracy: 0.959184 | 0.882353
   print "Digits:",
   method(imagesDigits[0], imagesDigits[1])
   print "Letters:",
   method(imagesLetters[0], imagesLetters[1])

main()
