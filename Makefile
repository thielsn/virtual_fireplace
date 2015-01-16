#declare variable

CC=g++
CFLAGS=-Wall

LIB_DIR = lib
LIB_OBJ = lib/librgbmatrix.a



all: lib_code fire

clean: clean_fire clean_lib
	

	
#library code
.PHONY: lib_code


lib_code:
	$(MAKE) -C $(LIB_DIR)

clean_lib:
	$(MAKE) -C $(LIB_DIR) clean
	
	
	
fire:
	$(CC) $(CFLAGS) fire.cc $(LIB_OBJ) -I ./include -lpthread -o fire
	
clean_fire:
	rm -f *.o fire
	
	
