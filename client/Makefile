CFLAGS += -I ./include -I ../Common/include
LDFLAGS = -Llib -lCliModule -lsqlite3

PREFIX ?= ./bin
LIB = ./lib
DATA = ./data
LOG = ./log

PROGRAM_NAME = Client
LIB_NAME = CliModule

EXCLUDE = ./src/DSClient.c
SRC = $(filter-out $(EXCLUDE), $(wildcard ./src/*.c)) $(wildcard ../Common/src/*.c)

all: shared_lib clean
	@gcc ${CFLAGS} ./src/DSClient.c -o ${PROGRAM_NAME} ${LDFLAGS}

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
	
distclean:
	@rm -rf ${DATA} ${LOG}
	
uninstall:
	@rm -rf ${PREFIX} ${LIB} ${DATA} ${LOG} 
