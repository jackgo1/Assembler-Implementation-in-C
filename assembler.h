#ifndef __ASSEMBLER_H_
#define __ASSEMBLER_H_

#include "common.h"
#include "lexer.h"
#include "preprocessor.h"
#include "output_unit.h"
#include "linked_list.h"

#define MAX_LENGTH_OF_LINE 81 
#define BEGINNING_ADDRESS 100
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define LABEL_MAX_LENGTH 31 
#define MAX_LENGTH_OF_MACRO 31

/*
 * This function takes the number of input files and their names, 
 * iterates through each file, preprocesses the file and compiles it with using the compilation function,
 * and outputs relevant files if compilation is successful.
 *
 * @param amount_of_files The number of input files.
 * @param name_of_file An array of pointers to file names.
 * @return 0 if the assembly process completes successfully.
 */
int assembler(int amount_of_files, char ** name_of_file);

#endif
