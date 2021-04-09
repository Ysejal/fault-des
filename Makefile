## Variables ..................................................................:

CC = gcc
CFLAGS = -g3 -Wall
LDFLAGS = -lm
LIBS = -L./lib -lm
INCLUDES =  -I./lib/include

## Structure du projet ........................................................:

EXEC = bin/des-app
SRC_PATH = src/
OBJ_PATH = build/
INC_PATH = include/

SRC = $(shell find $(SRC_PATH)*.c)
OBJ = $(SRC:$(SRC_PATH)%.c=$(OBJ_PATH)%.o)

## Autres .....................................................................:

ARGS =

## Lancement ..................................................................:

run : compil 
	@./$(EXEC) $(ARGS)

## Compilation ................................................................:

compil : $(EXEC)

$(EXEC) : $(OBJ)
	@$(CC) $^ $(INCLUDES) -o $(EXEC) $(LDFLAGS) $(LIBS)

$(OBJ_PATH)%.o : $(SRC_PATH)%.c $(INC_PATH)%.h
	@$(CC) -c $< -o $@ $(CFLAGS)

## Nettoyage ..................................................................:

clean :
	$(info Suppression de $(EXEC) et de $(OBJ) :)
	rm -f $(EXEC)
	rm -f $(OBJ)

## Debugger ...................................................................:

debug : compil
	$(info Debbugage avec $@ :)
	gdb --args ./$(EXEC) $(ARGS)

## Archivage ..................................................................:

tar :
	tar -zcvf ../CHIKAR_Soufiane-LEFEBVRE_Theo.tar.gz *