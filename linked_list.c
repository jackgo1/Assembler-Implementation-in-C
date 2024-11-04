#include <stdio.h>
#include <string.h>
#include "linked_list.h"

/*
 * Creates a new symbol linked list and initializes it with the given initial symbol.
 *
 * @param initial_symbol The initial symbol to be added to the linked list.
 * @return A pointer to the newly created SymbolLinkedList, or NULL if memory allocation fails.
 */
SymbolLinkedList *new_symbol_linked_list(const struct symbol *initial_symbol) {
    SymbolLinkedList *list = (SymbolLinkedList *)malloc(sizeof(SymbolLinkedList));
    if (list == NULL) {
        fprintf(stderr, "wasn't able to allocate memory for new string list\n");
        return NULL;
    }
    /* Allocate memory for the head node */
    list->head = (SymbolNode *)malloc(sizeof(SymbolNode));
    if (list->head == NULL) {
        fprintf(stderr, "wasn't able to allocate memory for new node\n");
        free(list);
        return NULL;
    }
    /* Allocate memory for the initial symbol data and copy it */
    list->head->symbol_data = malloc(sizeof(struct symbol));
    memcpy(list->head->symbol_data, initial_symbol, sizeof(struct symbol));

    /* Set the tail to the head since there's only one node */
    list->tail = list->head;
    /* Set the next pointer of the head to NULL */
    list->head->next = NULL;
    /* Initialize the size of the linked list to 1 */
    list->size_of_linked_list = 1;
    
     /* Return the new  linked list that was created */
    return list;
}

/*
 * Inserts a new symbol into a symbol linked list.
 *
 * @param list A pointer to the pointer of the SymbolLinkedList.
 * @param symbol The symbol to be inserted into the linked list.
 * @return A pointer to the newly inserted SymbolNode, or NULL if insertion fails.
 */
SymbolNode *insert_symbol_to_linked_list(SymbolLinkedList **list, struct symbol *symbol) {
    SymbolNode *new_node;
    
    if (!symbol) {
        return NULL;
    }

    if (!*list) {
        /* If the linked list doesn't exist, create a new one with the given symbol */
        *list = new_symbol_linked_list(symbol);
        if (!*list) {
            /* Return NULL if creating the linked list failed */
            return NULL;
        }
        /* Return the head of the newly created linked list */
        return (*list)->head;  
    } else {
        /* If the linked list already exists, insert a new node with the given symbol */
        /* Allocate memory for the new node */
        new_node = (SymbolNode *)malloc(sizeof(SymbolNode));
        if (!new_node) {
            return NULL;
        }
        /* Allocate memory for the symbol data in the new node and copy the data */
        new_node->symbol_data = malloc(sizeof(struct symbol));
        if (!new_node->symbol_data) {
            free(new_node);
            return NULL;
        }
        memcpy(new_node->symbol_data, symbol, sizeof(struct symbol));
        
        /* Update pointers to insert the new node at the end of the linked list */
        (*list)->tail->next = new_node; /* Update next pointer of the current tail node */
        (*list)->tail = (*list)->tail->next; /* Update the tail pointer to point to the new node */
        new_node->next = NULL; /* Set next pointer of new node to NULL */

        (*list)->size_of_linked_list++; /* Increment the size of the linked list */

        return new_node;
    }
}

/*
 * Creates a new linked list to store symbols not found.
 *
 * @param initial_data Initial data for the head node of the linked list.
 * @return A pointer to the newly created SymbolsNotFoundLinkedList, or NULL on failure.
 */
SymbolsNotFoundLinkedList *new_symbols_not_found_linked_list(const SymbolsNotFoundData *initial_data) {
    /* Allocate memory for the linked list */
    SymbolsNotFoundLinkedList *list = (SymbolsNotFoundLinkedList *)malloc(sizeof(SymbolsNotFoundLinkedList));
    if (!list) { 
        fprintf(stderr, "wasn't able to allocate memory for new symbols_not_found list\n");
        return NULL;
    }
    /* Allocate memory for the head node */
    list->head = (SymbolsNotFoundNode *)malloc(sizeof(SymbolsNotFoundNode));
    if (list->head == NULL) {
        fprintf(stderr, "wasn't able to allocate memory for new node\n");
        free(list);
        return NULL;
    }
    /* Allocate memory for the initial data and copy it */
    list->head->data = malloc(sizeof(SymbolsNotFoundData));
    if (!list->head->data) {
        free(list->head); 
        free(list);
        return NULL;
    }

    memcpy(list->head->data, initial_data, sizeof(SymbolsNotFoundData));
    
    list->tail = list->head; /* Set the tail to the head since there's only one node */
    list->head->next = NULL;  /* Set the next pointer of the head node to NULL */
    list->size_of_linked_list = 1; /* Initialize the size of the linked list to 1 */

    /* Return the new linked list that was created */
    return list;
}

/*
 * Inserts data for symbols not found into the "SymbolsNotFound" linked list.
 *
 * @param list A pointer to the pointer of the SymbolsNotFoundLinkedList.
 * @param data The data to be inserted into the linked list.
 * @return A pointer to the newly inserted SymbolsNotFoundNode, or NULL on failure.
 */
SymbolsNotFoundNode *insert_symbol_not_found_to_linked_list(SymbolsNotFoundLinkedList **list, const SymbolsNotFoundData *data) {
    SymbolsNotFoundNode *new_node;
    if (!data) {
        /* If the data is NULL than return NULL */
        return NULL;
    }

    if (!*list) {
        /* If the linked list doesn't exist, create a new one with the given data */
        *list = new_symbols_not_found_linked_list(data);
        if (!*list) {
            /* Return NULL if creating the linked list failed */
            return NULL;
        }
        /* Return the head of the newly created linked list */
        return (*list)->head;  
    } else {
       /* If the linked list already exists, insert a new node with the given data */

       /* Allocate memory for the new node */
       new_node = (SymbolsNotFoundNode *)malloc(sizeof(SymbolsNotFoundNode));
        if (!new_node) {
            return NULL;
        }
        /* Allocate memory for the data in the new node and copy the data */
        new_node->data = (SymbolsNotFoundData *)malloc(sizeof(SymbolsNotFoundData));
        if (!new_node->data) {
            free(new_node);
            return NULL;
        }
        memcpy(new_node->data, data, sizeof(SymbolsNotFoundData));
        
        
        (*list)->tail->next = new_node; /* Update next pointer of the current tail node */
        (*list)->tail = (*list)->tail->next;/* Update the tail pointer to point to the new node */
        new_node->next = NULL; /* Set next pointer of new node to NULL */
        
        /* Increment the size of the linked list */
        (*list)->size_of_linked_list++;
        
        /* Return the new node that was inserted */
        return new_node;  
    }
}



/* Create a new linked list to store macros.
 *
 * @param initial_macro Initial macro data for the head node of the linked list.
 * @return A pointer to the newly created MacroLinkedList, or NULL on failure.
 */
MacroLinkedList *new_linked_list_macro(const struct macro *initial_macro) {
    /* Allocate memory for the linked list */
    MacroLinkedList *list = (MacroLinkedList *)malloc(sizeof(MacroLinkedList));
    if (list == NULL) {
        fprintf(stderr, "wasn't able to allocate memory for new macro list\n");
        return NULL;
    }
    /* Allocate memory for the head node */
    list->head = (NodeMacro *)malloc(sizeof(NodeMacro));
    if (list->head == NULL) {
        fprintf(stderr, "wasn't able to allocate memory for new node\n");
        free(list);
        return NULL;
    }
    /* Allocate memory for the initial macro data and copy it */
    list->head->data = malloc(sizeof(struct macro));
    if (list->head->data == NULL) {
        fprintf(stderr, "wasn't able to allocate memory for new macro data\n");
        free(list->head);
        free(list);
        return NULL;
    }

    memcpy(list->head->data, initial_macro, sizeof(struct macro));

    list->tail = list->head; /* Set the tail to the head since there's only one node */
    list->head->next = NULL; /* Set the next pointer of the head node to NULL */
    list->size_of_linked_list = 1; /* Initialize the size of the linked list to 1 */
    
    /* Return the new linked list that was created */
    return list;
}

/* Insert a macro into the "MacroLinkedList".
 *
 * @param list A pointer to the pointer of the MacroLinkedList.
 * @param macro The macro to be inserted into the linked list.
 * @return A pointer to the newly inserted NodeMacro, or NULL on failure.
 */
NodeMacro *insert_macro_to_linked_list(MacroLinkedList **list, struct macro *macro) {
    NodeMacro *new_node = (NodeMacro *)malloc(sizeof(NodeMacro));
    if (!new_node) {
        return NULL;
    }
    /* Allocate memory for the macro data in the new node and copy the data */
    new_node->data = malloc(sizeof(struct macro));
    if (!new_node->data) {
        free(new_node);
        return NULL;
    }
    
    memcpy(new_node->data, macro, sizeof(struct macro));
    /* Set next pointer of new node to NULL */
    new_node->next = NULL;

    if (!*list) {
        /* If the linked list doesn't exist, create a new one with the given macro */
        *list = new_linked_list_macro(macro);
        if (!*list) {
            free(new_node->data);
            free(new_node);
            return NULL;
        }
        /* Return the head of the newly created linked list */
        return (*list)->head;  
    } else {
        /* If the linked list already exists, insert a new node with the given macro */
        (*list)->tail->next = new_node; /* Update next pointer of the current tail node */
        (*list)->tail = (*list)->tail->next;  /* Update the tail pointer to point to the next */

        (*list)->size_of_linked_list++; /* Increment the size of the linked list */
        
        /* Return the new node that was inserted */
        return new_node;  
    }
}



/* Create a new linked list to store certain extern data.
 *
 * @param name_of_extern Name of the extern symbol.
 * @param address_of_extern Address of the extern symbol.
 * @return A pointer to the newly created CertainExternLinkedList, or NULL on failure.
 */
CertainExternLinkedList *new_linked_list_certain_extern(const char *name_of_extern, long address_of_extern) {
    /* Allocate memory for the linked list */
    CertainExternLinkedList *list = (CertainExternLinkedList *)malloc(sizeof(CertainExternLinkedList));
    if (!list) {
        fprintf(stderr, "wasn't able to allocate memory for new certain_extern list\n");
        return NULL;
    }
    /* Allocate memory for the head node */
    list->head = (CertainExternNode *)malloc(sizeof(CertainExternNode));
    if (!list->head) {
        fprintf(stderr, "wasn't able to allocate memory for new node\n");
        free(list);
        return NULL;
    }
    /* Allocate memory for the initial extern data and copy it */
    list->head->data = malloc(sizeof(struct certain_extern));
    if (!list->head->data) {
        fprintf(stderr, "wasn't able to allocate memory for new extern data\n");
        free(list->head);
        free(list);
        return NULL;
    }
    /* Copy the name and address in to the new node's data */
    strncpy(list->head->data->name_of_extern, name_of_extern, LABEL_MAX_LENGTH); 
    list->head->data->name_of_extern[LABEL_MAX_LENGTH] = '\0';  
    list->head->data->address_of_extern = address_of_extern; 
    
    list->tail = list->head; /* Set the tail to the head since there's only one node */
    list->head->next = NULL; /* Set the next pointer of the head node to NULL */
    list->size_of_linked_list = 1; /* Initialize the size of the linked list to 1 */

    /* Return the new linked list that was created */
    return list;
}

/* Insert certain extern data into the "CertainExternLinkedList".
 *
 * @param list A pointer to the pointer of the CertainExternLinkedList.
 * @param extern_data The certain extern data to be inserted into the linked list.
 * @return A pointer to the newly inserted CertainExternNode, or NULL on failure.
 */
CertainExternNode *insert_certain_extern_to_linked_list(CertainExternLinkedList **list, struct certain_extern *extern_data) {
    CertainExternNode *new_node;
    if (!extern_data) {
        return NULL;
    }

    if (!*list) {
        /* If the linked list doesn't exist, create a new one with the given extern data */
        *list = new_linked_list_certain_extern(extern_data->name_of_extern, extern_data->address_of_extern);
        if (!*list) {
            return NULL;
        }
        /* Return the head of the newly created linked list */
        return (*list)->head;  
    } else {
        /* If the linked list already exists, insert a new node with the given extern data */

        /* Allocate memory for the new node */
        new_node = (CertainExternNode *)malloc(sizeof(CertainExternNode));
        if (!new_node) {
            return NULL;
        }
        /* Allocate memory for the extern data in the new node and copy the data */
        new_node->data = malloc(sizeof(struct certain_extern));
        if (!new_node->data) {
            fprintf(stderr, "wasn't able to allocate memory for new extern data\n");
            free(new_node);
            return NULL;
        }

        memcpy(new_node->data, extern_data, sizeof(struct certain_extern));

        (*list)->tail->next = new_node; /* Update next pointer of the current tail node */
        (*list)->tail = (*list)->tail->next; /* Update the tail pointer to point to the next */
        new_node->next = NULL; /* Set next pointer of new node to NULL */

        (*list)->size_of_linked_list++; /* Increment the size of the linked list */
        
        /* Return the new node that was inserted */
        return new_node;
    }
}


/* Create a new linked list to store strings.
 *
 * @param initial_str The initial string to be stored in the linked list.
 * @return A pointer to the newly created StringLinkedList, or NULL on failure.
 */
StringLinkedList *new_linked_list_string(const char *initial_str) { 
    /* Allocate memory for the linked list */
    StringLinkedList *list = (StringLinkedList *)malloc(sizeof(StringLinkedList));
    if (!list) {
        fprintf(stderr, "wasn't able to allocate memory for new string list\n");
        return NULL;
    }
    /* Allocate memory for the head node */
    list->head = (StringNode *)malloc(sizeof(StringNode));
    if (!list->head) {
        fprintf(stderr, "wasn't able to allocate memory for the head node\n");
        free(list);
        return NULL;
    }

    /* Copy the initial string into the data field of the head node */
    strncpy(list->head->data, initial_str, MAX_STRING_LENGTH); 
    list->head->data[MAX_STRING_LENGTH - 1] = '\0'; 

    list->tail = list->head; /* Set the tail to the head since there's only one node */
    list->head->next = NULL; /* Set the next pointer of the head node to NULL */
    list->size_of_linked_list = 1; /* Initialize the size of the linked list to 1 */
    
    /* Return the new linked list that was created */
    return list;
}

/* Insert a string into the "StringLinkedList".
 *
 * @param list A pointer to the pointer of the StringLinkedList.
 * @param str The string to be inserted into the linked list.
 * @return A pointer to the newly inserted StringNode, or NULL on failure.
 */
StringNode *insert_string_to_linked_list(StringLinkedList **list, const char *str) {
    StringNode *new_node;
    
    if (!str) {
        return NULL;
    }

    if (!*list) {
        /* If the linked list doesn't exist, create a new one with the given string */
        *list = new_linked_list_string(str);
        if (!*list) {
            return NULL;
        }
        /* Return the head of the newly created linked list */
        return (*list)->head;  
    } else {
        /* If the linked list already exists, insert a new node with the given string */

        /* Allocate memory for the new node */
        new_node = (StringNode *)malloc(sizeof(StringNode));
        if (!new_node) {
            return NULL;
        }
        
        /* Copy the input string into the data field of the new node */
        strncpy(new_node->data, str, MAX_STRING_LENGTH); 
        new_node->data[MAX_STRING_LENGTH - 1] = '\0'; 

        (*list)->tail->next = new_node; /* Update next pointer of the current new node */
        (*list)->tail = (*list)->tail->next; /* Update the tail pointer to point to the next */
        new_node->next = NULL; /* Set next pointer of new node to NULL */
        
        /* Increment the size of the linked list */
        (*list)->size_of_linked_list++;
        
        /* Return the new node that was inserted */
        return new_node;
    }
}


/* Find a symbol in the "SymbolLinkedList".
 *
 * @param list The SymbolLinkedList to search in.
 * @param target_name The name of the symbol to search for.
 * @return A pointer to the symbol data if found, or NULL if not found or if inputs are NULL.
 */
struct symbol* find_symbol_in_linked_list(const SymbolLinkedList *list, const char* target_name) {
    SymbolNode *current;
    struct symbol *symbol_data;

    /* Return NULL if either the list or target_name is NULL */
    if (!list || !target_name) return NULL;

    /* Start from the head of the linked list */
    current = list->head;
    while (current) {
        /* Get the symbol data of the current node */
        symbol_data = (struct symbol*)current->symbol_data; 
        /* Compare the name of the symbol with the target_name */
        if (strcmp(symbol_data->name_of_symbol, target_name) == 0) {
            /* Return the symbol data if found */
            return symbol_data; 
        }
        current = current->next; /* Continue to the next node in the linked list */
    }
    
    /* Return NULL if the symbol with the target_name was not found */
    return NULL; 
}


/* Find a macro in the "MacroLinkedList".
 *
 * @param list The MacroLinkedList to search in.
 * @param target_name The name of the macro to search for.
 * @return A pointer to the macro data if found, or NULL if not found or if inputs are NULL.
 */
struct macro *find_macro_in_linked_list(const MacroLinkedList *list, const char *target_name) {
    NodeMacro *current;
    struct macro *macro_data;

    /* Return NULL if either the list or target_name is NULL */
    if (!list || !target_name) return NULL;
    
    /* Start from the head of the linked list */
    current = list->head;
    while (current) {
        /* Get the macro data of the current node */
        macro_data = (struct macro *)current->data;
        /* Compare the name of the macro with the target_name */
        if (strcmp(macro_data->name_of_macro, target_name) == 0) {
            /* Return the macro data if found */
            return macro_data;  
        }
        /* Continue to the next node in the linked list */
        current = current->next;
    }
    /* Return NULL if the macro with the target_name was not found */
    return NULL;  
}

/* Find a string in the "StringLinkedList".
 *
 * @param list The StringLinkedList to search in.
 * @param target_str The string to search for.
 * @return A pointer to the StringNode containing the string if found, or NULL if not found or if inputs are NULL.
 */
StringNode *find_string_in_linked_list(const StringLinkedList *list, const char *target_str) {
    StringNode *current; 

    /* Return NULL if either the list or target_str is NULL */
    if (!list || !target_str) return NULL;
    /* Start from the head of the linked list */
    current = list->head;
    while (current) {
        /* Compare the current node's data with the target_str */
        if (current->data[0] != '\0' && target_str != NULL && strcmp(current->data, target_str) == 0) {
            /* Return the current node if the string matches */
            return current; 
        }
        current = current->next;
    }

    /* Return NULL if the target_str was not found in the linked list */
    return NULL;  
}

/* Free the memory of a "StringLinkedList".
 *
 * @param list A pointer to the StringLinkedList pointer.
 */
void free_string_linked_list(StringLinkedList **list) {
    StringNode *current;
    StringNode *temp;
    if (list == NULL || *list == NULL) {
        return;
    }
     /* Start from the head of the linked list */
    current = (*list)->head;
    while (current != NULL) {
        /* Store the current node in a temporary variable */
        temp = current; 
        current = current->next;
        /* Free the memory allocated for the current node */
        free(temp);        
    }
    /* Free the memory allocated for the linked list */
    free(*list);  
    /* Set the input pointer to NULL to indicate that the linked list is freed */ 
    (*list) = NULL;
}

/* Free the memory of a "CertainExternLinkedList".
 *
 * @param list A pointer to the CertainExternLinkedList pointer.
 */

void free_certain_extern_linked_list(CertainExternLinkedList **list) {
    CertainExternNode *current;
    CertainExternNode *temp;
    if (list == NULL || *list == NULL) {
        return;
    }
    /* Start from the head of the linked list */
    current = (*list)->head;
    while (current != NULL) {
        /* Store the current node in a temporary variable */
        temp = current;
        current = current->next;
        /* Free the memory allocated for the certain extern data */
        free(temp->data);
        /* Free the memory allocated for the current node */ 
        free(temp);
    }
    
    /* Free the memory allocated for the linked list */
    free(*list);
    /* Set the input pointer to NULL to indicate that the linked list is freed */
    (*list) = NULL;
}

/* Free the memory of a "MacroLinkedList".
 *
 * @param list A pointer to the MacroLinkedList pointer.
 */
void free_macro_linked_list(MacroLinkedList **list) {
    NodeMacro *current;
    NodeMacro *temp;
    if (list == NULL || *list == NULL) {
        return;
    }
    /* Start from the head of the linked list */
    current = (*list)->head;
    while (current != NULL) {
        /* Store the current node in a temporary variable */
        temp = current;
        current = current->next;
        /* Free the memory allocated for the lines_of_code linked list within the macro */
        free_string_linked_list(&temp->data->lines_of_code); 
        /* Free the memory allocated for the macro data */
        free(temp->data);
        /* Free the memory allocated for the current node */
        free(temp);
    }
    /* Free the memory allocated for the linked list */
    free(*list);
    /* Set the input pointer to NULL to indicate that the linked list is freed */
    (*list) = NULL;
}


/* Free the memory of a "SymbolLinkedList".
 *
 * @param list A pointer to the SymbolLinkedList pointer.
 */
void free_symbol_linked_list(SymbolLinkedList **list) {
    SymbolNode *current;
    SymbolNode *temp;
    if (list == NULL || *list == NULL) {
        return;
    }
    /* Start from the head of the linked list */
    current = (*list)->head;
    while (current != NULL) {
        /* Store the current node in a temporary variable */
        temp = current;
        current = current->next;
        /* Free the memory allocated for the symbol data */
        free(temp->symbol_data); 
        /* Free the memory allocated for the current node */
        free(temp);
    }
    /* Free the memory allocated for the linked list */
    free(*list);
    /* Set the input pointer to NULL to indicate that the linked list is freed */
    (*list) = NULL;
}
/* Free the memory of a "SymbolsNotFoundLinkedList".
 *
 * @param list A pointer to the SymbolsNotFoundLinkedList pointer.
 */
void free_symbols_not_found_linked_list(SymbolsNotFoundLinkedList **list) {
    SymbolsNotFoundNode *current;
    SymbolsNotFoundNode *temp;
    if (list == NULL || *list == NULL) {
        return;
    }
    /* Start from the head of the linked list */
    current = (*list)->head;
    while (current != NULL) {
        /* Store the current node in a temporary variable */
        temp = current;
        current = current->next;
        /* Free the memory allocated for the symbols not found data */
        free(temp->data); 
        /* Free the memory allocated for the current node */
        free(temp);
    }
    /* Free the memory allocated for the linked list */
    free(*list);
    /* Set the input pointer to NULL to indicate that the linked list is freed */
    (*list) = NULL;
}

/* Get the amount of elements in the "MacroLinkedList".
 *
 * @param list A pointer to the MacroLinkedList.
 * @return The number of elements in the linked list.
 */
size_t get_amount_of_elements_in_macro_linked_list(const MacroLinkedList *list) {
    return list->size_of_linked_list;
}
/* Get the amount of elements in the "SymbolLinkedList".
 *
 * @param list A pointer to the SymbolLinkedList.
 * @return The number of elements in the linked list.
 */
size_t get_amount_of_elements_in_symbol_linked_list(const SymbolLinkedList *list) {
    return list->size_of_linked_list;
}
/* Get the amount of elements in the "CertainExternLinkedList".
 *
 * @param list A pointer to the CertainExternLinkedList.
 * @return The number of elements in the linked list.
 */
size_t get_amount_of_elements_in_certain_extern_linked_list(const CertainExternLinkedList *list) {
    return list->size_of_linked_list;
}






