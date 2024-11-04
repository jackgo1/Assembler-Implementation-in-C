/*
 * Author: Yaakov Goldberg
 * ID: 314907593
 */

#include "assembler.h"

/*
 * The main function of the program.
 *
 * This function is the entry point of the program.
 * 
 * @param argc The number of command-line arguments.
 * @param argv An array of strings representing the command-line arguments.
 * @return The exit status of the program, typically 0 for successful execution.
 */
int main(int argc, char ** argv){
    return assembler(argc-1,argv+1);
}
