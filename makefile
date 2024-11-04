CFLAGS = -g -Wall -ansi -pedantic 

all: assembler.o common.o lexer.o linked_list.o main.o output_unit.o preprocessor.o
	@gcc $(CFLAGS) assembler.o common.o lexer.o linked_list.o main.o output_unit.o preprocessor.o -o assembler -lm
assembler.o: assembler.c assembler.h
	@gcc $(CFLAGS) -c assembler.c 
common.o: common.c common.h
	@gcc $(CFLAGS) -c common.c 
lexer.o: lexer.c lexer.h
	@gcc $(CFLAGS) -c lexer.c 
linked_list.o: linked_list.c linked_list.h
	@gcc $(CFLAGS) -c linked_list.c 	
main.o: main.c assembler.h
	@gcc $(CFLAGS) -c main.c 			
output_unit.o: output_unit.c output_unit.h
	@gcc $(CFLAGS) -c output_unit.c 
preprocessor.o: preprocessor.c preprocessor.h
	@gcc $(CFLAGS) -c preprocessor.c 	

	
clean: assembler.o common.o lexer.o linked_list.o main.o output_unit.o preprocessor.o assembler
	rm ./assembler.o ./common.o ./lexer.o ./linked_list.o ./main.o ./output_unit.o ./preprocessor.o ./assembler