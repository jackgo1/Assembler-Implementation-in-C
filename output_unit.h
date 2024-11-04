#ifndef __OUTPUT_UNIT_H_
#define __OUTPUT_UNIT_H_
#include "common.h"



#define FILE_EXTENSION_EXT ".ext"
#define FILE_EXTENSION_ENT ".ent"
#define FILE_EXTENSION_OB ".ob"
#define BASE64 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

/*
 * Outputs the object file data to relevent files.
 *
 * This function takes the name of the output files and a pointer to the object file data.
 * It generates and outputs the code and data sections to the .ob file, certain externs
 * to the .ext file, and entry symbols to the .ent file if necessary.
 *
 * @param name_of_were_to_output The base name of the output files.
 * @param obj_file A pointer to the object file data.
 */
void output(char * name_of_were_to_output, const struct object_file * obj_file);


#endif

