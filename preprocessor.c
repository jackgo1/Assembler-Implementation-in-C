#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <libgen.h>
#include <string.h>
#include "preprocessor.h"


#define file_extension_as ".as"
#define file_extension_am ".am"
#define MAX_LENGTH_OF_MACRO 31
#define MAX_LENGTH_OF_LINE 81
#define SPACE_CHARS " \t\n\f\r\v"
#define SKIP_SPACE(str) while(*str && isspace(*str)) str++
#define SKIP_SPACE_REVERSE(str,end) while(*str && isspace(*str) && end != str) str++


typedef char* line_of_code;

/*
 * Create a new macro structure and initialize its feilds.
 *
 * This function allocates memory for a new macro structure, sets its name, and initializes
 * the linked list to store lines of code associated with the macro.
 *
 * @param macro_name The name of the macro being created.
 * @return A pointer to the newly created macro structure, or NULL on memory allocation error.
 */
struct macro *create_macro(const char *macro_name) {
    struct macro *new_macro = (struct macro *)malloc(sizeof(struct macro));
    if (!new_macro) {
        fprintf(stderr, "wasn't able to allocate memory for new macro\n");
        return NULL;
    }
    /* Set the macro name */
    strncpy(new_macro->name_of_macro, macro_name, MAX_LENGTH_OF_MACRO - 1);
    new_macro->name_of_macro[MAX_LENGTH_OF_MACRO - 1] = '\0';

    /* Initialize the linked list to store lines of code */
    new_macro->lines_of_code = new_linked_list_string(""); 
    free(new_macro);

    return new_macro;
}

char *my_implementation_for_strdup(const char *str) {
    char *new_str = (char *)malloc(strlen(str) + 1);
    if (new_str != NULL) {
        strcpy(new_str, str);
    }
    return new_str;
}

enum preprocessor_line_recognition{
    empty_line,
    definition_of_a_macro,
    line_in_the_macro,
    end_of_macro,
    calling_a_macro,
    incurrect_calling_of_macro, 
    line_with_none_of_the_above,
    incurrect_definition_of_a_macro,
    incurrect_definition_of_a_endmacro,
    macro_exists_already_its_redefinetion,
    unknown_line

};

/*
 * Recognizes the type of preprocessor line.
 *
 * This function analyzes a preprocessor line and determines its type.
 * It checks for macro definitions, end of macro definitions, calling a macro, and more opions that are possible
 *
 * @param line The preprocessor line to be recognized.
 * @param in_macro Flag indicating if currently inside a macro.
 * @param table_of_macros A linked list of macros.
 * @param name_of_macro A pointer to store the name of the recognized macro (if applicable).
 * @param table_with_names_of_macros A linked list of names of defined macros.
 * @return The recognized type of preprocessor line.
 */
enum preprocessor_line_recognition recegnize_a_line(char *line, int in_macro, MacroLinkedList *table_of_macros, char **name_of_macro, StringLinkedList *table_with_names_of_macros){
    char *str_in_start_of_line = line;
    char *semi_colon;
    char *tmp;
    StringNode *current_node;
    char *cleaned_line;
    
    /* Skip leading spaces */
    SKIP_SPACE(str_in_start_of_line);
    
    /* Check for empty line */
    if (*str_in_start_of_line == '\0') {
        return empty_line;
    }
    
    /* Remove comments from the line */
    semi_colon = strchr(line, ';');
    if (semi_colon) {
        *semi_colon = '\0';
    }
    /* Skip spaces again */
    SKIP_SPACE(line);

    if (*line == '\0') {
        return empty_line;
    }
    
    if (in_macro == 0 && table_with_names_of_macros != NULL) { 
        /* Clean the line and remove newline character */
        cleaned_line = my_implementation_for_strdup(line); 
        if (cleaned_line != NULL) {
            size_t len = strlen(cleaned_line);
            if (len > 0 && cleaned_line[len - 1] == '\n') {
                cleaned_line[len - 1] = '\0'; 
            }
            /* Check if cleaned line matches any defined macro names */
            current_node = table_with_names_of_macros->head;
            while (current_node != NULL) {
                if (*name_of_macro != NULL && strcmp(cleaned_line, current_node->data) == 0) {
                    free(cleaned_line); 
                    return calling_a_macro;
                }
                current_node = (StringNode *)current_node->next;
            }
            
            free(cleaned_line); 
        }
   }


    /* Check if the end of a macro definition */
    tmp = strstr(line, "endmcro");
    if (tmp) {
        if (in_macro == 0) {
            return incurrect_definition_of_a_endmacro;
        }

        SKIP_SPACE_REVERSE(tmp, str_in_start_of_line);

        if (tmp != str_in_start_of_line) {
            return incurrect_definition_of_a_endmacro;
        }

        tmp += strlen("endmcro");
        SKIP_SPACE(tmp);

        if (*tmp != '\0') {
            return incurrect_definition_of_a_endmacro;
        }

        return end_of_macro;
    }
    /* Check if the definition of a macro */
    tmp = strstr(line, "mcro");
    if (tmp) {
        SKIP_SPACE_REVERSE(tmp, str_in_start_of_line);

        if (tmp != str_in_start_of_line) {
            return incurrect_definition_of_a_macro;
        }

        tmp += strlen("mcro");
        SKIP_SPACE(tmp);
        /* Store the name of the macro */
       *name_of_macro = tmp;
       line = tmp;
        tmp = strpbrk(line, SPACE_CHARS);

        if (!tmp) {
            return incurrect_definition_of_a_macro;
        }

        *tmp = '\0';
        SKIP_SPACE(tmp);
        
    
        /* Check for valid definition of a macro */
        if (*tmp != '\0') {
            return incurrect_definition_of_a_macro;
        }

        if (strlen(tmp) > MAX_LENGTH_OF_MACRO) {
            return incurrect_definition_of_a_macro;
        }

        *name_of_macro = (char *)my_implementation_for_strdup(line);
        /* Check if macro name is alphanumeric */
        while (*tmp) {
            if (!isalnum(*tmp)) {
                return incurrect_definition_of_a_macro;
            }
            tmp++;
        }
            /* Check if macro name already exists */
            if (find_string_in_linked_list(table_with_names_of_macros, *name_of_macro)) {
             return macro_exists_already_its_redefinetion;
            }
            /* Insert macro name in to the table */
            insert_string_to_linked_list(&table_with_names_of_macros, *name_of_macro);
            if (!table_with_names_of_macros) {
                fprintf(stderr, "wasn't able to insert macro name into table_with_names_of_macros\n");
                exit(1); 
            }

        return definition_of_a_macro;
    }
    /* Check if in macro */
    if (in_macro == 1) {
        return line_in_the_macro;
    }
    
    if (in_macro == 0)
    {
        return line_with_none_of_the_above;
    }
    
    return unknown_line;
}

/*
 * Prepares a filename by adding the specified extension.
 *
 * This function takes a base filename and an extension, and generates a new string
 * containing the filename with the extension appended. It allocates memory for the new
 * string and handles memory management of the input filename.
 *
 * @param name_of_file The base filename to be prepared.
 * @param extension The extension to be added to the filename.
 * @return A dynamically allocated string containing the prepared filename with the extension,
 *         or NULL if there's a memory allocation error.
 */
char* prepare_filename(char* name_of_file, const char* extension) {
    char* file_name;
    char* prepared_name;

    file_name = malloc(strlen(name_of_file) + 1);
    if (file_name == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        return NULL;
    }
    /* Copy the input filename to the allocated memory */
    strcpy(file_name, name_of_file);
    strcat(file_name, extension);

    prepared_name = malloc(strlen(file_name) + strlen(extension) + 1);
    if (prepared_name == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        free(file_name); 
        return NULL;
    }
    /* Copy the filename and extension to the prepared name */
    strcpy(prepared_name, file_name);

    free(file_name); 
    return prepared_name;
}

/*
 * Opens a file with the specified mode.
 *
 * This function open a file with the given filename using the provided mode.
 * If the file cannot be opened, an error message is printed to stderr.
 *
 * @param filename The name of the file to be opened.
 * @param mode The mode in which the file should be opened (e.g., "r" for read, "w" for write).
 * @return A pointer to the opened file, or NULL if the file could not be opened.
 */
FILE* open_file(const char* filename, const char* mode) {
    FILE* file = fopen(filename, mode);

    if (file == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", filename);

        return NULL;
    }

    return file;
}

/*
 * This function reads the content of the source assembly file, processes each line
 * and writes the modified lines to an output am file.
 * The preprocessor recognizes macros, expands macros when called, and handles various
 * preprocessor line types.
 *
 * @param name_of_file The name of the source assembly file to be preprocessed.
 * @return A pointer to the name of the generated modified assembly file, or NULL on error.
 */
const char * file_preprocessor(char * name_of_file) {
    char line_buffer[MAX_LENGTH_OF_LINE + 1] = {0};
    enum preprocessor_line_recognition pre_line_rec;
    char* as_name_of_file;
    char* am_name_of_file;
    FILE* as_file;
    FILE* am_file;
    int i = 0;
    
    struct macro_node *current_macro;

    int in_macro = 0;

    struct macro *macro;

    StringLinkedList *table_with_names_of_macros = NULL;
    MacroLinkedList *table_of_macros = NULL;
    char cleaned_macro_name[MAX_LENGTH_OF_MACRO] = {0}; 

    const char *ptr;
    char * name_of_macro = NULL;
    StringNode *line_node;
    
    /* Prepare the file names */
    as_name_of_file = prepare_filename(name_of_file, file_extension_as);
    am_name_of_file = prepare_filename(name_of_file, file_extension_am);
    
    /* Open input and output files */
    as_file = open_file(as_name_of_file, "r");
    am_file = open_file(am_name_of_file, "w");

    if (as_file == NULL || am_file == NULL) {
        if (as_file) fclose(as_file);
        if (am_file) fclose(am_file);
        free(as_name_of_file);
        free(am_name_of_file);
        return NULL;
    }
    
    /* Process every line of the input .as file */
    while (fgets(line_buffer, MAX_LENGTH_OF_LINE, as_file)) { 
        pre_line_rec = recegnize_a_line(line_buffer, in_macro, table_of_macros, &name_of_macro, table_with_names_of_macros);

        switch (pre_line_rec) {
            case empty_line:

                break;

            case definition_of_a_macro:
                /* Create and manage macro definitions */
                macro = create_macro(name_of_macro);
                if (macro == NULL){
                    fprintf(stderr, "Memory allocation error.\n");

                    return NULL;
                }

                insert_macro_to_linked_list(&table_of_macros, macro);
                insert_string_to_linked_list(&table_with_names_of_macros, name_of_macro);
                in_macro = 1;

                break;

            case line_in_the_macro:
                /* Collect lines that are in a macro */
                insert_string_to_linked_list(&(macro->lines_of_code), my_implementation_for_strdup(line_buffer));
                
                break;

            case end_of_macro:
                /* End of a macro definition */
                in_macro = 0;
                
                macro = NULL;
                break;
            case calling_a_macro:
            {
                /* Expand and include macro content */
                ptr = line_buffer; 
                 
                i = 0;
                
                while (*ptr && isspace(*ptr)) {
                    ptr++;
                }

                
                
                while (*ptr && !isspace(*ptr) && i < MAX_LENGTH_OF_MACRO - 1) {
                    cleaned_macro_name[i++] = *ptr++;
                }
                cleaned_macro_name[i] = '\0';  

                /* Find and expand the macro */
                current_macro = table_of_macros->head;
                while (current_macro) {
                    if (strcmp(((struct macro *)(current_macro->data))->name_of_macro, cleaned_macro_name) == 0) {
                        line_node = ((struct macro *)(current_macro->data))->lines_of_code->head;
                        while (line_node) {
                            fputs(line_node->data, am_file);
                            line_node = (StringNode *)line_node->next;
                        }
                        break; 
                    }
                    current_macro = (struct macro_node *)current_macro->next;
                }
            }
            break;
            case line_with_none_of_the_above:

                    fputs(line_buffer, am_file);

                break;

            default:
    
                break;
        }
    }

    /* Close files and clean memory */
    fclose(as_file);
    fclose(am_file);
    free(as_name_of_file);
    free_macro_linked_list(&table_of_macros);
    free_string_linked_list(&table_with_names_of_macros); 
    free(macro);
    
    


    return am_name_of_file;
}
