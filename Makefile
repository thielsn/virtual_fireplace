#declare variable

CC = g++
CFLAGS = -O3 -Wall

LDFLAGS = -lpthread
CPPFLAGS = -I ./include

LIB_DIR = lib
LIB_OBJ = lib/librgbmatrix.a



all: lib_code fire

dist-clean: clean_fire clean_lib

clean: clean_fire
	

	
#library code
.PHONY: lib_code


lib_code:
	$(MAKE) -C $(LIB_DIR)

clean_lib:
	$(MAKE) -C $(LIB_DIR) clean
	
	
	
fire:
	$(CC) $(CFLAGS) fire.cc $(LIB_OBJ) $(CPPFLAGS) $(LDFLAGS)  -o fire
	
clean_fire:
	rm -f *.o fire
	
	
