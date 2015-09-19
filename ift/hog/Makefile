IFTFLAGS = -L../lib -lift -llapack -lblas -lm -I ../include

hog: hog.c
	$(CC) $(FLAGS) -std=c99 hog.c -o hog $(IFTFLAGS)

clean:
	rm hog

