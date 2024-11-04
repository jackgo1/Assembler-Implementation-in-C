#ifndef __COMMON_H_
#define __COMMON_H_

#include <stddef.h>

#define MAX_STRING_LENGTH 81
#define MAX_LENGTH_OF_MACRO 31
#define LABEL_MAX_LENGTH 31
#define MEMORY_SIZE 1024 

#define SPACE_CHARS " \t\n\f\r\v"
#define SKIP_SPACE(str) while(*str && isspace(*str)) str++
#define SKIP_SPACE_REVERSE(str,end) while(*str && isspace(*str) && end != str) str++

/* Type Definitions for Linked Lists and Data Structures */
typedef struct macro_node NodeMacro;
typedef struct macro_linked_list MacroLinkedList;
typedef struct symbols_that_were_not_found_at_first SymbolsNotFoundData;
typedef struct certain_extern_node CertainExternNode;
typedef struct symbols_not_found_node SymbolsNotFoundNode;
typedef struct symbol_not_found_linked_list SymbolsNotFoundLinkedList;
typedef struct string_node StringNode;
typedef struct symbol_node SymbolNode;
typedef struct certain_extern_linked_list CertainExternLinkedList;
typedef struct symbol_linked_list SymbolLinkedList;
typedef struct string_linked_list StringLinkedList;

struct symbol;
struct certain_extern;
struct macro;

struct symbol {  
    enum { /* Represents different types of symbols */
        symbol_extern, 
        symbol_entry, 
        symbol_code, 
        symbol_data,
        symbol_entry_code, 
        symbol_entry_data 
    }type_of_symbol;
    unsigned int address_of_symbol; /* The address of the symbol */
    unsigned int line_of_declaration; /* The line where the symbol was declared */
    char name_of_symbol[LABEL_MAX_LENGTH + 1]; /* The name of the symbol */
};

/* Represents a node in the macro linked list */
struct macro_node{
    struct macro *data; /* The data of the node */
    struct macro_node *next; /* The next node in the linked list */
 };

/* Represents a linked list of macros */
struct macro_linked_list{
    NodeMacro *head; /* The head node of the linked list */
    NodeMacro *tail; /* The tail node of the linked list */
    size_t size_of_linked_list; /* The size of the linked list */
};

/* Represents a node in the symbols not found linked list */
struct symbols_not_found_node {
    SymbolsNotFoundData *data; /* The data of the node */
    struct symbols_not_found_node *next; /* The next node in the linked list */
};

/* Represents a linked list of symbols not found */
struct symbol_not_found_linked_list{
    SymbolsNotFoundNode *head; /* The head node of the linked list */
    SymbolsNotFoundNode *tail; /* The tail node of the linked list */
    size_t size_of_linked_list; /* The size of the linked list */
};




/* Represents a node in the string linked list */
struct string_node {
    char data[MAX_STRING_LENGTH]; /* The string data in the node */
    struct string_node *next; /* The next node in the linked list */
};
/* Represents a linked list of strings */
struct string_linked_list {
    StringNode *head; /* The head node of the linked list */
    StringNode *tail; /* The tail node of the linked list */
    size_t size_of_linked_list; /* The size of the linked list */
};

/* Represents a node in the symbol linked list */
struct symbol_node {
    struct symbol *symbol_data; /* The symbol data in the node */
    struct symbol_node *next; /* The next node in the linked list */
};
/* Represents a linked list of symbols */
struct symbol_linked_list {
    SymbolNode *head; /* The head node of the linked list */
    SymbolNode *tail; /* The tail node of the linked list */
    size_t size_of_linked_list; /* The size of the linked list */
};

/* Represents a node in the certain extern linked list */
struct certain_extern_node { 
    struct certain_extern *data; /* The data of the node */
    struct certain_extern_node *next; /* The next node in the linked list */
};
/* Represents a linked list of certain externs */
struct certain_extern_linked_list {
    struct certain_extern_node *head; /* The head node of the linked list */
    struct certain_extern_node *tail; /* The tail node of the linked list */
    size_t size_of_linked_list; /* The size of the linked list */
};




/* A array containing string representations of symbol types */
extern const char * string_type_of_symbol[6];

/* Represents a data word */
typedef struct data_w { 
    unsigned int data_word: 12; 
} data_w;

/* Represents a code word */
typedef struct code_w { 
    unsigned int code_word: 12; 
} code_w;

/* Represents symbols that were not found at first */
struct symbols_that_were_not_found_at_first {
    char name_of_symble[LABEL_MAX_LENGTH + 1]; /* The name of the symbol */
    int line_it_was_called; /* The line where the symbol was called */
    unsigned int machine_word; /* The machine word */
    unsigned int address_of_calling; /* The address of calling */
};

/* Represents a object file */
struct object_file {
    code_w code_image[MEMORY_SIZE]; /* Contains the code image of the file */
    data_w data_image[MEMORY_SIZE]; /* Contains the data image of the file */
    long IC; /* The Instruction Counter */
    long DC; /* The Data Counter */
    CertainExternLinkedList *name_and_addresses_certain_extern; /* A Linked list of certain externs */
    SymbolLinkedList *table_of_symbols; /* A Linked list of symbols */
    int number_of_entries; /* the number of entry symbols */
};

/* Represents a certain extern */
struct certain_extern {
    char name_of_extern[LABEL_MAX_LENGTH + 1]; /* The name of the extern */
    long address_of_extern; /* The address of the extern */
};

/* Represents a macro */
struct macro {
    char name_of_macro[MAX_LENGTH_OF_MACRO + 1]; /* The name of the macro */
    StringLinkedList *lines_of_code; /* Linked list of lines of code in the macro */
};


/*
 * Deletes an object file and frees associated memory.
 *
 * This function frees memory associated with an object_file structure,
 * including the linked lists of certain external symbols and symbols.
 *
 * @param obj_file A pointer to the object_file structure to be deleted.
 */
void assembler_delete_object_file(struct object_file * obj_file);


/*
 * Adds a new external symbol to the linked list of certain extern symbols.
 *
 * This function creates a new extern symbol with the provided name and address,
 * and inserts it into the linked list of certain extern symbols. If the list does not
 * exist, a new list is created. 
 *
 * @param extern_list A pointer to a pointer to the linked list of certain external symbols.
 * @param name_of_extern The name of the external symbol to be added.
 * @param address_of_extern The address associated with the external symbol.
 */
void add_external_symbol(CertainExternLinkedList **extern_list, const char *name_of_extern, long address_of_extern);

/*
 * Creates a new object file.
 *
 * This function initializes a new object_file structure with default values.
 * It creates an empty linked list of certain external symbols and sets the
 * instruction counter (IC) and data counter (DC) to zero.
 *
 * @return An initialized object_file structure.
 */
struct object_file assembler_new_object_file();



#endif
