CC=gcc
LD=gcc
CCFLAGS=-pedantic -Wall -g
LDFLAGS=
SRC=\
	main.c\
	string.c
OBJ=$(SRC:.c=.o)
OUT=prog.out
CHILD=child
SRC_CHILD=child.c
OBJ_CHILD=$(SRC_CHILD:.c=.o)

.SUFFIXES: .c .o

start: $(OUT) $(CHILD)
	./$(OUT)

$(OUT): $(OBJ)
	${LD} ${LDFLAGS} -o $(OUT) $(OBJ) -lm

$(CHILD): $(OBJ_CHILD)
	${LD} ${LDFLAGS} -o $(CHILD) $(OBJ_CHILD) -lm

play: $(OUT) $(CHILD)
	valgrind ./$(OUT)

main.o: $(SRC)

child.o: child.c

.c.o: 
	${CC} ${CCFLAGS} -c $< -o $@

clear:
	rm -f $(OBJ) $(OUT)