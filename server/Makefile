CFLAGS += -I ./include -I ../common/include
LDFLAGS = -Llib -lservModule -lsqlite3

PREFIX ?= ./bin
LIB = ./lib
DATA = ./data
LOG = ./log

PROGRAM_NAME = server
LIB_NAME = servModule

EXCLUDE = ./src/DSServer.c
SRC = $(filter-out $(EXCLUDE), $(wildcard ./src/*.c)) $(wildcard ../common/src/*.c)

all: shared_lib clean
	@gcc ${CFLAGS} ./src/DSServer.c -o ${PROGRAM_NAME} ${LDFLAGS}

shared_lib:
	@gcc ${CFLAGS} -fPIC -shared -o lib${LIB_NAME}.so $(SRC)
	@mkdir -p ${LIB}
	@mv lib${LIB_NAME}.so ${LIB}
	
install:
	@mkdir -p ${LOG}
	@mkdir -p ${DATA}
	@mkdir -p ${PREFIX}
	@mv ${PROGRAM_NAME} ${PREFIX}

clean:	
	@rm -rf ./src/*.o
	
distclean: clean
	@rm -rf *.log
	
uninstall:
	@rm -rf ${PREFIX} ${LIB} ${DATA} ${LOG} 
