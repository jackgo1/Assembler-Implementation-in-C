#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

#include <stdlib.h>

#include "common.h"

#define MAX_STRING_LENGTH 81

/* Inserts a macro into the macro linked list.
 *
 * @param list A pointer to the MacroLinkedList pointer.
 * @param macro A pointer to the macro to be inserted.
 * @return A pointer to the inserted NodeMacro or NULL on failure.
 */
NodeMacro *insert_macro_to_linked_list(MacroLinkedList **list, struct macro *macro);

/* Inserts a symbol into the symbol linked list.
 *
 * @param list A pointer to the SymbolLinkedList pointer.
 * @param symbol A pointer to the symbol to be inserted.
 * @return A pointer to the inserted SymbolNode or NULL on failure.
 */
SymbolNode *insert_symbol_to_linked_list(SymbolLinkedList **list, struct symbol *symbol);

/* Frees memory allocated for a symbol linked list and its elements.
 *
 * @param list A pointer to the SymbolLinkedList pointer.
 */
void free_symbol_linked_list(SymbolLinkedList **list);

/* Frees memory allocated for a macro linked list and its elements.
 *
 * @param list A pointer to the MacroLinkedList pointer.
 */
void free_macro_linked_list(MacroLinkedList **list);

/* Finds a symbol in the symbol linked list by its name.
 *
 * @param list A pointer to the SymbolLinkedList.
 * @param target_name The name of the symbol to find.
 * @return A pointer to the symbol if found, NULL otherwise.
 */
struct symbol* find_symbol_in_linked_list(const SymbolLinkedList *list, const char* target_name);

/* Finds a macro in the macro linked list by its name.
 *
 * @param list A pointer to the MacroLinkedList.
 * @param target_name The name of the macro to find.
 * @return A pointer to the macro if found, NULL otherwise.
 */
struct macro *find_macro_in_linked_list(const MacroLinkedList *list, const char *target_name);

/* Gets the number of elements in a macro linked list.
 *
 * @param list A pointer to the MacroLinkedList.
 * @return The number of elements in the linked list.
 */
size_t get_amount_of_elements_in_macro_linked_list(const MacroLinkedList *list);

/* Gets the number of elements in a symbol linked list.
 *
 * @param list A pointer to the SymbolLinkedList.
 * @return The number of elements in the linked list.
 */
size_t get_amount_of_elements_in_symbol_linked_list(const SymbolLinkedList *list);

/* Creates a new linked list for certain externs.
 *
 * @param name_of_extern The name of the extern.
 * @param address_of_extern The address of the extern.
 * @return A pointer to the created CertainExternLinkedList or NULL on failure.
 */
CertainExternLinkedList *new_linked_list_certain_extern(const char *name_of_extern, long address_of_extern);

/* Inserts certain extern data into the linked list.
 *
 * @param list A pointer to the CertainExternLinkedList pointer.
 * @param extern_data A pointer to the certain extern data to be inserted.
 * @return A pointer to the inserted CertainExternNode or NULL on failure.
 */
CertainExternNode *insert_certain_extern_to_linked_list(CertainExternLinkedList **list, struct certain_extern *extern_data);

/* Creates a new linked list for symbols not found.
 *
 * @param initial_data The initial data for symbols not found.
 * @return A pointer to the created SymbolsNotFoundLinkedList or NULL on failure.
 */
SymbolsNotFoundLinkedList *new_symbols_not_found_linked_list(const SymbolsNotFoundData *initial_data);

/* Inserts symbols not found data into the linked list.
 *
 * @param list A pointer to the SymbolsNotFoundLinkedList pointer.
 * @param data A pointer to the symbols not found data to be inserted.
 * @return A pointer to the inserted SymbolsNotFoundNode or NULL on failure.
 */
SymbolsNotFoundNode *insert_symbol_not_found_to_linked_list(SymbolsNotFoundLinkedList **list, const SymbolsNotFoundData *data);

/* Frees memory allocated for a symbols not found linked list and its elements.
 *
 * @param list A pointer to the SymbolsNotFoundLinkedList pointer.
 */
void free_symbols_not_found_linked_list(SymbolsNotFoundLinkedList **list);

/* Creates a new linked list for macros.
 *
 * @param initial_macro A pointer to the initial macro data.
 * @return A pointer to the created MacroLinkedList or NULL on failure.
 */
MacroLinkedList *new_linked_list_macro(const struct macro *initial_macro);

/* Creates a new linked list for symbols.
 *
 * @param initial_symbol A pointer to the initial symbol data.
 * @return A pointer to the created SymbolLinkedList or NULL on failure.
 */
SymbolLinkedList *new_symbol_linked_list(const struct symbol *initial_symbol);

/* Frees memory allocated for a certain extern linked list and its elements.
 *
 * @param list A pointer to the CertainExternLinkedList pointer.
 */
void free_certain_extern_linked_list(CertainExternLinkedList **list);

/* Gets the number of elements in a certain extern linked list.
 *
 * @param list A pointer to the CertainExternLinkedList.
 * @return The number of elements in the linked list.
 */
size_t get_amount_of_elements_in_certain_extern_linked_list(const CertainExternLinkedList *list);

/* Creates a new linked list for strings.
 *
 * @param initial_str The initial string data.
 * @return A pointer to the created StringLinkedList or NULL on failure.
 */
StringLinkedList *new_linked_list_string(const char *initial_str);

/* Inserts a string into the linked list.
 *
 * @param list A pointer to the StringLinkedList pointer.
 * @param str A pointer to the string to be inserted.
 * @return A pointer to the inserted StringNode or NULL on failure.
 */
StringNode *insert_string_to_linked_list(StringLinkedList **list, const char *str);

/* Finds a string in the linked list by its content.
 *
 * @param list A pointer to the StringLinkedList.
 * @param target_str The content of the string to find.
 * @return A pointer to the found StringNode or NULL if not found.
 */
StringNode *find_string_in_linked_list(const StringLinkedList *list, const char *target_str);

/* Frees memory allocated for a string linked list and its elements.
 *
 * @param list A pointer to the StringLinkedList pointer.
 */
void free_string_linked_list(StringLinkedList **list);

/* Finds an assembly instruction mapping by its name.
 *
 * @param instruction_name The name of the assembly instruction.
 * @return A pointer to the found asm_instruction_mapping or NULL if not found.
 */
struct asm_instruction_mapping *find_instruction_mapping(const char *instruction_name);

/* Finds an assembly directive mapping by its name.
 *
 * @param directive_name The name of the assembly directive.
 * @return A pointer to the found asm_directive_mapping or NULL if not found.
 */
struct asm_directive_mapping *find_directive_mapping(const char *directive_name);

#endif 
