#ifndef __PREPROCESSOR_H_
#define __PREPROCESSOR_H_

#include "common.h"
#include "linked_list.h"


#define file_extension_as ".as"
#define file_extension_am ".am"
#define MAX_LENGTH_OF_MACRO 31
#define MAX_LENGTH_OF_LINE 81



#define MAX_LENGTH_OF_MACRO 31

/*
 * This function reads the content of the source assembly file, processes each line
 * and writes the modified lines to an output am file.
 * The preprocessor recognizes macros, expands macros when called, and handles various
 * preprocessor line types.
 *
 * @param name_of_file The name of the source assembly file to be preprocessed.
 * @return A pointer to the name of the generated modified assembly file, or NULL on error.
 */
const char* file_preprocessor(char* name_of_file);

/*
 * Create a new macro structure and initialize its feilds.
 *
 * This function allocates memory for a new macro structure, sets its name, and initializes
 * the linked list to store lines of code associated with the macro.
 *
 * @param macro_name The name of the macro being created.
 * @return A pointer to the newly created macro structure, or NULL on memory allocation error.
 */
struct macro *create_macro();

/*
 * This function creates a new copy of the input string and allocates memory for it.
 *
 * @param str The input null-terminated string to be duplicated.
 * @return A pointer to the newly allocated string copy, or NULL if memory allocation fails.
 * @note The caller is responsible for freeing the allocated memory using `free()`.
 */
char *my_implementation_for_strdup(const char *str); 


#endif
