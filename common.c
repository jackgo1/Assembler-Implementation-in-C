#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

CertainExternNode *insert_certain_extern_to_linked_list(CertainExternLinkedList **list, struct certain_extern *extern_data);
CertainExternLinkedList *new_linked_list_certain_extern(const char *name_of_extern, long address_of_extern);
void free_certain_extern_linked_list(CertainExternLinkedList **list);
void free_symbol_linked_list(SymbolLinkedList **list);

const char * string_type_of_symbol[6] = {
     "extern symbol", /* [symbol_extern] */
     "entry symbol", /* [symbol_entry] */
     "code symbol", /* [symbol_code] */
     "data symbol", /* [symbol_data] */
     "entry code symbol", /* [symbol_entry_code] */
     "entry data symbol" /* [symbol_entry_data] */
};


/*
 * Creates a new object file.
 *
 * This function initializes a new object_file structure with default values.
 * It creates an empty linked list of certain external symbols and sets the
 * instruction counter (IC) and data counter (DC) to zero.
 *
 * @return An initialized object_file structure.
 */
struct object_file assembler_new_object_file() {
    struct object_file obj = {0}; 
    
    /* Create an empty linked list of certain external symbols */
    obj.name_and_addresses_certain_extern = new_linked_list_certain_extern("", 0); 
    /* Initialize instruction counter (IC) and data counter (DC) to zero */
    obj.IC = 0;
    obj.DC = 0;
    
    /* Return the initialized object_file structure */
    return obj; 
}

/*
 * Deletes an object file and frees associated memory.
 *
 * This function frees memory associated with an object_file structure,
 * including the linked lists of certain external symbols and symbols.
 *
 * @param obj_file A pointer to the object_file structure to be deleted.
 */
void assembler_delete_object_file(struct object_file *obj_file) {  
    /* Free memory for the linked list of certain external symbols */
    free_certain_extern_linked_list(&(obj_file->name_and_addresses_certain_extern));
    /* Free memory for the linked list of symbols */
    free_symbol_linked_list(&obj_file->table_of_symbols);
}

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
void add_external_symbol(CertainExternLinkedList **extern_list, const char *name_of_extern, long address_of_extern) {
    struct certain_extern *new_extern;
    
    /* Check if the extern symbol list pointer is NULL */
    if (extern_list == NULL) {
        return;
    }

    
    if (*extern_list == NULL) {
        /* Create a new linked list for extern symbols */
        *extern_list = new_linked_list_certain_extern(name_of_extern, address_of_extern);
        if (*extern_list == NULL) {
            fprintf(stderr, "wasn't able to create an external symbol list\n");
            return;
        }
    }

    /* Allocate memory for the new external symbol structure */
    new_extern = (struct certain_extern *)malloc(sizeof(struct certain_extern));
    if (new_extern == NULL) {
        fprintf(stderr, "wasn't able to allocate memory for new external symbol\n");
        return;
    }
    /* Copy the provided name of the extern symbol in to the structure */
    strncpy(new_extern->name_of_extern, name_of_extern, LABEL_MAX_LENGTH);
    /* Ensure null-termination */
    new_extern->name_of_extern[LABEL_MAX_LENGTH] = '\0'; 

    /* Set the address of the extern symbol in the structure */ 
    new_extern->address_of_extern = address_of_extern;

    /* Insert the new extern symbol into the linked list */
    insert_certain_extern_to_linked_list(extern_list, new_extern);
}


