#
# Makefile para programas teste (homemade trademark)
#

CC=gcc
LIB_DIR=../lib

all: simple_test exemplo3f teste_vetor_mutex

simple_test: simple_test.c $(LIB_DIR)/libmthread.a
	$(CC) -o simple_test simple_test.c -L$(LIB_DIR) -lmthread -Wall

exemplo3f: exemplo3f.c $(LIB_DIR)/libmthread.a
	$(CC) -o exemplo3f exemplo3f.c -L$(LIB_DIR) -lmthread -Wall

teste_vetor_mutex: teste_vetor_mutex.c $(LIB_DIR)/libmthread.a
	$(CC) -o teste_vetor_mutex teste_vetor_mutex.c -L$(LIB_DIR) -lmthread -Wall


clean:
	rm -rf simple_test exemplo3f teste_vetor_mutex *~

