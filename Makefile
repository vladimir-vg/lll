# Makefile for lll

COMPILE_FLAGS= -Wall -std=c99 -g
OBJS= lll_interpreter.o lll_types.o lll_symtable.o lll_utils.o \
	lll_base_funcs.o lll_eval.o

lll_interpreter: $(OBJS)
	gcc $(OBJS) -o llli

lll_eval.o: lll_eval.c lll_eval.h
	gcc $(COMPILE_FLAGS) -c lll_eval.c -o lll_eval.o 

lll_interpreter.o: lll_interpreter.c lll_interpreter.h
	gcc $(COMPILE_FLAGS) -c lll_interpreter.c -o lll_interpreter.o

lll_base_funcs.o: lll_base_funcs.c lll_base_funcs.h
	gcc $(COMPILE_FLAGS) -c lll_base_funcs.c -o lll_base_funcs.o

lll_types.o: lll_types.h lll_types.c
	gcc $(COMPILE_FLAGS) -c lll_types.c -o lll_types.o

lll_symtable.o: lll_symtable.c lll_symtable.h
	gcc $(COMPILE_FLAGS) -c lll_symtable.c -o lll_symtable.o

lll_utils.o: lll_utils.c lll_utils.h
	gcc $(COMPILE_FLAGS) -c lll_utils.c -o lll_utils.o

clean:
	rm *.o
	rm llli