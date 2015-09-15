all: libsvm.a
	@echo
	@echo "libsvm.a built..."
	@echo "DONE."

libsvm.a: svm.cpp
	$(CC) -O3 -Wall  -fPIC -fopenmp -pthread -pedantic -c svm.cpp -o libsvm.o
	ar csr libsvm.a libsvm.o

clean:
	rm -f libsvm.o; rm -f libsvm.a;
