make mySelectCandidates
make myExtractFeatures
make iftDesignClassifier
make myDetectPlate

../bin/mySelectCandidates ../../images/train/orig ../../images/train/cand

../bin/mySelectCandidates ../../images/test/orig ../../images/test/cand

../bin/myExtractFeatures ../../images/train/orig ../../images/train/label ../../images/train/cand ../../data.opf

../bin/iftDesignClassifier ../../data.opf ../../svm

../bin/myDetectPlate ../../images/test/orig ../../images/test/label ../../images/test/cand ../../svm ../../images/test/output
