#
# Makefile para programas teste (homemade trademark)
#

CC=gcc
LIB_DIR=../lib

all: simple_test exemplo3f teste_vetor_mutex mandel_atv_mutex

simple_test: simple_test.c $(LIB_DIR)/libmthread.a
	$(CC) -o simple_test simple_test.c -L$(LIB_DIR) -lmthread -Wall

teste_vetor_mutex: teste_vetor_mutex.c $(LIB_DIR)/libmthread.a
	$(CC) -o teste_vetor_mutex teste_vetor_mutex.c -L$(LIB_DIR) -lmthread -Wall

mandel_atv_mutex: mandel_atv_mutex.c $(LIB_DIR)/libmthread.a
	$(CC) -o mandel_atv_mutex mandel_atv_mutex.c -L$(LIB_DIR) -lmthread -Wall

mwait: mwait.c $(LIB_DIR)/libmthread.a
	$(CC) -o mwait mwait.c -L$(LIB_DIR) -lmthread -Wall

mmutex_init: mmutex_init.c $(LIB_DIR)/libmthread.a
	$(CC) -o mmutex_init mmutex_init.c -L$(LIB_DIR) -lmthread -Wall

mlock: mlock.c $(LIB_DIR)/libmthread.a
	$(CC) -o mlock mlock.c -L$(LIB_DIR) -lmthread -Wall

munlock: munlock.c $(LIB_DIR)/libmthread.a
	$(CC) -o munlock munlock.c -L$(LIB_DIR) -lmthread -Wall

myield: myield.c $(LIB_DIR)/libmthread.a
	$(CC) -o myield myield.c -L$(LIB_DIR) -lmthread -Wall

mcreate: mcreate.c $(LIB_DIR)/libmthread.a
	$(CC) -o mcreate mcreate.c -L$(LIB_DIR) -lmthread -Wall

clean:
	rm -rf simple_test exemplo3f teste_vetor_mutex mandel_atv_mutex mwait mlock munlock myield mcreate mmutex_init *~

