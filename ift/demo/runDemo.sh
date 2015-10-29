# Builds everything
make mySelectCandidates
make myExtractFeatures
make iftDesignClassifier
make myDetectPlate
make myOutputImage

# Gera as imagens candidatas para o conjunto de treino.
../bin/mySelectCandidates < inputs/1_input_selectTrain.txt

# Gera as imagens candidatas para o conjunto de teste.
../bin/mySelectCandidates < inputs/4_input_selectTest.txt

# Gera os vetores de caracteristicas para as imagens de treino.
../bin/myExtractFeatures < inputs/2_input_features.txt

# Gera o classificador.
../bin/iftDesignClassifier < inputs/3_input_classfier.txt

# Usa o classificador nas imagens de teste e calcula a precisão! o/
../bin/myDetectPlate < inputs/5_input_detect.txt

# Corrige as imagens geradas, removendo candidatos a mais e preenchendo buracos.
../bin/myOutputImage < inputs/input_correctPlate.txt
