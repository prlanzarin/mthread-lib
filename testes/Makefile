#
# Makefile para programas teste (homemade trademark)
#

CC=gcc
LIB_DIR=../lib

all: simple_test exemplo3f teste_vetor_mutex mandel_atv_mutex

simple_test: simple_test.c $(LIB_DIR)/libmthread.a
	$(CC) -o simple_test simple_test.c -L$(LIB_DIR) -lmthread -Wall

exemplo3f: exemplo3f.c $(LIB_DIR)/libmthread.a
	$(CC) -o exemplo3f exemplo3f.c -L$(LIB_DIR) -lmthread -Wall

teste_vetor_mutex: teste_vetor_mutex.c $(LIB_DIR)/libmthread.a
	$(CC) -o teste_vetor_mutex teste_vetor_mutex.c -L$(LIB_DIR) -lmthread -Wall

mandel_atv_mutex: mandel_atv_mutex.c $(LIB_DIR)/libmthread.a
	$(CC) -o mandel_atv_mutex mandel_atv_mutex.c -L$(LIB_DIR) -lmthread -Wall

rc_adder: rc_adder.c $(LIB_DIR)/libmthread.a
	$(CC) -o rc_adder rc_adder.c -L$(LIB_DIR) -lmthread -Wall

mlock: mlock.c $(LIB_DIR)/libmthread.a
	$(CC) -o mlock mlock.c -L$(LIB_DIR) -lmthread -Wall

clean:
	rm -rf simple_test exemplo3f teste_vetor_mutex mandel_atv_mutex rc_adder mlock*~

