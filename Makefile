# Makefile for lll

BINARY=llli
DEL_CMD=rm
CC=gcc
COMPILE_FLAGS= -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -pedantic -D_GLIBCXX_DEBUG -std=c99 -ggdb -O0
OBJS=	lll_interpreter.o   \
	lll_types.o         \
	lll_symtable.o      \
	lll_utils.o         \
	lll_base_funcs.o    \
	lll_eval.o          \
	lll_print.o         \
	lll_special_forms.o \
	lll_read.o

lll_interpreter: $(OBJS)
	$(CC) $(OBJS) -o $(BINARY)

lll_special_forms.o: lll_special_forms.h lll_special_forms.c
	$(CC) $(COMPILE_FLAGS) -c lll_special_forms.c -o lll_special_forms.o

lll_print.o: lll_print.h lll_print.c
	$(CC) $(COMPILE_FLAGS) -c lll_print.c -o lll_print.o

lll_read.o: lll_read.c lll_read.h
	$(CC) $(COMPILE_FLAGS) -c lll_read.c -o lll_read.o

lll_eval.o: lll_eval.c lll_eval.h
	$(CC) $(COMPILE_FLAGS) -c lll_eval.c -o lll_eval.o 

lll_interpreter.o: lll_interpreter.c lll_interpreter.h
	$(CC) $(COMPILE_FLAGS) -c lll_interpreter.c -o lll_interpreter.o

lll_base_funcs.o: lll_base_funcs.c lll_base_funcs.h
	$(CC) $(COMPILE_FLAGS) -c lll_base_funcs.c -o lll_base_funcs.o

lll_types.o: lll_types.h lll_types.c
	$(CC) $(COMPILE_FLAGS) -c lll_types.c -o lll_types.o

lll_symtable.o: lll_symtable.c lll_symtable.h
	$(CC) $(COMPILE_FLAGS) -c lll_symtable.c -o lll_symtable.o

lll_utils.o: lll_utils.c lll_utils.h
	$(CC) $(COMPILE_FLAGS) -c lll_utils.c -o lll_utils.o

clean:
	$(DEL_CMD) *.o
	$(DEL_CMD) $(BINARY)