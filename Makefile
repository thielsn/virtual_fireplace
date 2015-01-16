
all: fire

fire:
	g++ fire.cc lib/librgbmatrix.a -I ./include -lpthread -o fire
	
clean:
	rm -f *.o fire
	
