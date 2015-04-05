#
# Makefile ESQUELETO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "mthread"
# 

CC=gcc
DEBUG="-g" # TODO tratar como opção do usuário 
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: mdata.o mthread.o libmthread.a

mthread.o: $(INC_DIR)/mthread.h $(SRC_DIR)/mthread.c
	$(CC) $(DEBUG) -c -o $(BIN_DIR)/mthread.o $(SRC_DIR)/mthread.c -Wall


mdata.o: $(INC_DIR)/mdata.h $(SRC_DIR)/mdata.c
	$(CC) $(DEBUG) -c -o $(BIN_DIR)/mdata.o $(SRC_DIR)/mdata.c -Wall

libmthread.a: mthread.o
	mkdir -p $(LIB_DIR) && ar rcs $(LIB_DIR)/libmthread.a $(BIN_DIR)/*.o
#regra2: #dependências para a regra2
#	$(CC) -o $(BIN_DIR)regra2 $(SRC_DIR)regra2.c -Wall

#regran: #dependências para a regran
#	$(CC) -o $(BIN_DIR)regran $(SRC_DIR)regran.c -Wall

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~


