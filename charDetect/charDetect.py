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
   classifier = svm.SVC(gamma=0.002)

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
   digitsImages = getImagesFromDir(digitsPath)
   lettersImages = getImagesFromDir(lettersPath)

   images = [
      digitsImages[0] + lettersImages[0],
      digitsImages[1] + lettersImages[1]
      ]

   index = range(len(images[0]))
   normalizedImages = skpre.scale(images[0])
   # Divide em teste e treino.
   # Calcula PCA - Reducao de dimensionalidade dos dados. :)
   pca = computePCA(normalizedImages)
   transformedData = pca.transform(normalizedImages)

   trainDataTF, testDataTF, classesTrainTF, classesTestTF = train_test_split(transformedData, images[1], train_size=0.65)
   
   return [[trainDataTF, classesTrainTF], [testDataTF, classesTestTF]]


def main():
   #digitsImages = getImagesFromDir(digitsPath)
   #print digitsImages[0][0], digitsImages[1][0]

   #results = computeKnn (digitsImages, digitsImages)[0]
   images = getImages()
   #results = computeKnn (images[0], images[1])[0]#old
   #computeSVM(images[0], images[1])#SVM accuracy: 0.670404
   #computeBNB(images[0], images[1])#BNB accuracy: 0.454036
   #computeSVMRBF(images[0], images[1])#SVMRBF accuracy: 0.780269
   #computeKNN(images[0], images[1]) #KNN accuracy: 0.757848
   print "Encerrando"



main()
