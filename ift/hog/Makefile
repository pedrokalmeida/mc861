IFTFLAGS    = -L../lib -L../libsvm -lift -llapack -lblas -lm -lsvm -lstdc++ -I ../include -I../libsvm
FLAGS = -std=c11

all: $@.c

$@.c: $@.c 
	$(CC) $(FLAGS) $@.c -o $@ $(IFTFLAGS)

clean:
	rm ../bin/*

