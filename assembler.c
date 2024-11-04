#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"


#define MAX_LENGTH_OF_LINE 81 
#define BEGINNING_ADDRESS 100
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"  

/*
 * This function prints a warning message with formatted output.
 * It includes the file name, line number, and the provided formatted message.
 *
 * @param name_of_file The name of the file where the warning occurred.
 * @param number_of_line The line number in the file where the warning occurred.
 * @param fmt The format string for the warning message.
 * @param ... Additional arguments for formatting the warning message.
 */
static void warning_fmt(const char * name_of_file,int number_of_line, const char * fmt,...){
    va_list vl;
    va_start(vl,fmt);
    printf("%s:%d: " ANSI_COLOR_YELLOW "warning: " ANSI_COLOR_RESET,name_of_file, number_of_line);
    vprintf(fmt,vl);
    printf("\n");
    va_end(vl);
}

/*
 * This function prints an error message with formatted output.
 * It includes the file name, line number, and the provided formatted message.
 *
 * @param name_of_file The name of the file where the error occurred.
 * @param number_of_line The line number in the file where the error occurred.
 * @param fmt The format string for the error message.
 * @param ... Additional arguments for formatting the error message.
 */
static void error_fmt(const char * name_of_file,int number_of_line, const char * fmt,...){
    va_list vl;
    va_start(vl,fmt);
    printf("%s:%d: " ANSI_COLOR_RED "error: " ANSI_COLOR_RESET, name_of_file,number_of_line);
    vprintf(fmt,vl);
    printf("\n");
    va_end(vl);
}

/*
 * Handles the processing of instructions.
 *
 * This function generates machine words for instructions found in the AST,
 * updates the relevant data structures in the object_file, handles symbol definitions
 * and warnings, generates errors when appropriate, and processes labels as operands.
 * It also identifies and handles missing symbols that were not defined but referenced.
 *
 * @param ast The Abstract Syntax Tree for the current line.
 * @param object A pointer to the object_file structure containing symbol and address information.
 * @param number_of_the_line The current line number in the am file.
 * @param were_to_fill_in_symbol_table A pointer to the list of missing symbols.
 * @param find_symbol A pointer to a symbol structure for symbol search.
 * @param local_symbol A pointer to a local symbol structure for creating new symbols.
 */
void process_ast_instruction(mmn14_ast ast, struct object_file *object, int number_of_the_line, SymbolsNotFoundLinkedList **were_to_fill_in_symbol_table, struct symbol **find_symbol, struct symbol *local_symbol) {
    unsigned int machine_word = 0;
    int i = 0;
    struct symbols_that_were_not_found_at_first symbol_not_found = {0};  
    unsigned int extern_address = 0;
    unsigned int current_machine_word_that_inserted = 0;
    
    /* Generate the machine word */
    machine_word = ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operand_opt[1] << 2;
    machine_word |= ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operand_opt[0] << 9;
    machine_word |= ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_opt << 5;
    /* Store the generated machine word in the code image at the current IC */
    object->code_image[object->IC].code_word = machine_word;
    object->IC++; 
    
    /* Handle specific instruction types that have one or two operands */
    if (ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_opt <= mmn14_ast_instruction_jsr && ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_opt >= mmn14_ast_instruction_mov) {
        if (ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operand_opt[1] == mmn14_ast_operand_opt_operand_register && ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operand_opt[0] == mmn14_ast_operand_opt_operand_register) {
            /* Handle the case if both operands are registers */
            machine_word = ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operands[1].register_number << 2;
            machine_word |= ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operands[0].register_number << 7;
            object->code_image[object->IC].code_word = machine_word;
            object->IC++; 
        } else {
            
            for (i = 0; i < 2; i++) {
                switch (ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operand_opt[i]) {
                    case mmn14_ast_operand_opt_constant_number:
                        /* Handle the case that the operand is a constant number */
                        machine_word = ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operands[i].constent_number << 2;
                        object->code_image[object->IC].code_word = machine_word;
                        object->IC++; 
                        break;
                    case mmn14_ast_operand_opt_operand_register:
                        /* Handle the case that the operand is a register */
                        machine_word = ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operands[i].register_number << (7 - (i * 5));
                        object->code_image[object->IC].code_word = machine_word;
                        object->IC++; 
                        break;
                    case mmn14_ast_operand_opt_operand_label:
                        /* Handle the case that the operand is a label */
                        *find_symbol = find_symbol_in_linked_list((object->table_of_symbols), ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operands[i].label);
                        /* Checks if the symbol was found or is of type 'symbol_entry' */
                        if (*find_symbol && ((*find_symbol)->type_of_symbol != symbol_entry)) {
                            /* Extract address from symbol and shift left by 2 bits */
                            machine_word = (*find_symbol)->address_of_symbol << 2;
                            if ((*find_symbol)->type_of_symbol == symbol_extern) {
                                /* If the type of symbol is extern */
                                /* Set the least significant bit to 1 if symbol is external */
                                machine_word |= 1;
                                extern_address = object->IC + BEGINNING_ADDRESS;
                                /* Add external symbol to the list of externals */
                                add_external_symbol(&(object->name_and_addresses_certain_extern), (*find_symbol)->name_of_symbol, extern_address);
                            } else {
                                /* Set the second least significant bit to 1 if symbol is internal */
                                machine_word |= 2;
                            }
                        }
                        object->code_image[object->IC].code_word = machine_word;
                        
                        current_machine_word_that_inserted = object->code_image[object->IC].code_word;  
                        object->IC++; 
                        /* Check if the symbol was not found or if the symbol type is 'symbol_entry' */
                        if (!*find_symbol || (*find_symbol && (*find_symbol)->type_of_symbol == symbol_entry)) {
                            strcpy(symbol_not_found.name_of_symble, ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operands[i].label);
                            /* Save the current machine word that was being inserted */
                            symbol_not_found.machine_word = current_machine_word_that_inserted;
                            /* Save the number of the line where the symbol was called */
                            symbol_not_found.line_it_was_called = number_of_the_line;
                            /* Calculate the address */
                            symbol_not_found.address_of_calling = object->IC + BEGINNING_ADDRESS + 1;
                            /* Insert the symbol_not_found in to the linked list */
                            insert_symbol_not_found_to_linked_list(were_to_fill_in_symbol_table, &symbol_not_found);
                        }
                        break;
                    case mmn14_ast_operand_opt_no_operand:
                        /* This handles the case that theres no operand */
                        break;
                }
            }
        }
    } else {
       /* No change needed for mmn14_ast_instruction_rts or mmn14_ast_instruction_stop */
    }
    
}

/* Handles the processing of directives within the Abstract Syntax Tree (AST).
 *
 * This function processes the directives found in the AST, including .string, .data, .entry, and .extern.
 * It updates the relevant data structures in the object_file, handles symbol definitions and warnings,
 * and generates errors when appropriate. It also processes and stores strings and data values in the
 * data_image of the object_file.
 *
 * @param ast The Abstract Syntax Tree for the current line.
 * @param object A pointer to the object_file structure containing symbol and address information.
 * @param name_of_am_file The name of the am file being compiled.
 * @param number_of_the_line The current line number in the am file.
 * @param find_symbol A pointer to a symbol structure for symbol search.
 * @param local_symbol A pointer to a local symbol structure for creating new symbols.
 */
void process_ast_directive(mmn14_ast ast, struct object_file *object, const char *name_of_am_file, int number_of_the_line, struct symbol **find_symbol, struct symbol *local_symbol) {
    unsigned int machine_word = 0;
    int i = 0; /* Initialize a loop counter */
    char *str = NULL;
    
    /* Check if the directive is missing a label for .data or .string directives */
    if ((ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt == mmn14_ast_directive_data ||
         ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt == mmn14_ast_directive_string) && ast.name_of_label[0] == '\0')
    {
        /* Generate a warning message */
        warning_fmt(name_of_am_file, number_of_the_line, "The '%s' directive should have a label.", ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt == mmn14_ast_directive_data ? ".data" : ".string");
        /* Exit the function */
        return;
    }
    /* Switch statement based on the directive option */
    switch (ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt)
    {
    
    case mmn14_ast_directive_string:
        /* Handle .string directive */
        str = ast.directive_or_instruction.mmn14_ast_directive.directive_operand.string;
        for (i = 0; str[i] != '\0'; i++)
        {
            machine_word = str[i];
            /* Store the machine_word in data_image */
            object->data_image[object->DC].data_word = machine_word;
            /* Increment the Data Counter (DC) */
            (object->DC)++;
        }
        /*  Reset machine_word to 0 */ 
        machine_word = 0;
        /* Store the final 0 value in data_image */
        object->data_image[object->DC].data_word = machine_word;
        /* Increment the Data Counter (DC) */
        (object->DC)++;
        break;
    case mmn14_ast_directive_data:
        
        for (i = 0; i < ast.directive_or_instruction.mmn14_ast_directive.directive_operand.data.num_of_numbers; i++)
        {
            object->data_image[object->DC].data_word = ast.directive_or_instruction.mmn14_ast_directive.directive_operand.data.data[i];
            (object->DC)++;
        }
        break;
    case mmn14_ast_directive_extern:  case mmn14_ast_directive_entry:
         /* Handle .extern and .entry directives */
        *find_symbol = find_symbol_in_linked_list(object->table_of_symbols, ast.directive_or_instruction.mmn14_ast_directive.directive_operand.name_of_label);
        /* Check if the symbol is found in the symbol table */
        if ((*find_symbol))
        {

            if (ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt == mmn14_ast_directive_entry)
            {
                /* Check if the directive is of type .entry */
                if ((*find_symbol)->type_of_symbol == symbol_entry || (*find_symbol)->type_of_symbol == symbol_entry_code || (*find_symbol)->type_of_symbol == symbol_entry_data)
                {
                    /* Generate a warning if the symbol is already defined as an entry symbol */
                    warning_fmt(name_of_am_file, number_of_the_line, "The label '%s': '%s' was defined already in line: '%d'.", (*find_symbol)->name_of_symbol, string_type_of_symbol[(*find_symbol)->type_of_symbol], (*find_symbol)->line_of_declaration);
                }else if((*find_symbol)->type_of_symbol == symbol_extern){
                    /* Generate an error if the symbol is defined as extern and attempted to be redefined as entry */
                    error_fmt(name_of_am_file, number_of_the_line, "The label '%s': '%s' was defined already in line: '%d'.", (*find_symbol)->name_of_symbol, string_type_of_symbol[(*find_symbol)->type_of_symbol], (*find_symbol)->line_of_declaration);

                }else{ /* In this case its symbol_code or symbol_data */
                    /* Change the symbol type to entry_code or entry_data based on its current type (symbol_code or symbol_data) */
                    (*find_symbol)->type_of_symbol = ((*find_symbol)->type_of_symbol == symbol_code) ? symbol_entry_code : symbol_entry_data;
                }
            }else{ /* In this case its an extern */
                    /* Handle .extern directive */
                    if((*find_symbol)->type_of_symbol == symbol_extern){
                        /* Generate a warning if the symbol is already defined as an extern symbol */
                        warning_fmt(name_of_am_file, number_of_the_line, "The label '%s': '%s' was defined already in line: '%d'.", (*find_symbol)->name_of_symbol, string_type_of_symbol[(*find_symbol)->type_of_symbol], (*find_symbol)->line_of_declaration);
                    }else{
                       /* Generate an error if the symbol is already defined and not as an extern symbol */
                       error_fmt(name_of_am_file, number_of_the_line, "The label '%s': '%s' was defined already in line: '%d'.", (*find_symbol)->name_of_symbol, string_type_of_symbol[(*find_symbol)->type_of_symbol], (*find_symbol)->line_of_declaration);

                    }
            }
        }
        else
        {
            /* Handle the case when the symbol is not found in the symbol table */

            /* Copy the name of the symbol */
            strcpy(local_symbol->name_of_symbol, ast.directive_or_instruction.mmn14_ast_directive.directive_operand.name_of_label);
            /* Set the type of the symbol based on the directive option */
            local_symbol->type_of_symbol = ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt;
            local_symbol->address_of_symbol = 0; 
            /* Save the line number where the symbol is declared */
            local_symbol->line_of_declaration = number_of_the_line;
            /* Insert the new created symbol in to the symbol table */
            insert_symbol_to_linked_list(&(object->table_of_symbols), local_symbol);

           
        }
        break;
    }
    
}

/*
 * Handles the processing of the symbol table, updating addresses and counting entry symbols.
 *
 * This function iterates through the symbol table, updating the addresses of defined symbols
 * and counting the number of entry symbols. It calculates the final addresses for data symbols
 * and entry data symbols. It also identifies and handles entry symbols that were declared
 * but not defined in the assembly.
 *
 * @param table_of_symbols A pointer to the linked list of symbols.
 * @param object A pointer to the object_file structure containing symbol and address information.
 * @param name_of_am_file The name of the am file being compiled.
 * @param error_d A pointer to the error flag that indicates the success of the compilation.
 */
void handle_symbol_table_process(SymbolLinkedList *table_of_symbols, struct object_file *object, const char *name_of_am_file, int *error_d) {
    size_t index;
    struct symbol *current_symbol;
    SymbolNode *current_node = table_of_symbols->head; /* Start at the head of the symbol table */

    /* Iterate through the symbol table */
    for (index = 0; index < table_of_symbols->size_of_linked_list && current_node != NULL; index++) {
        /* Get the symbol data from the current node */
        current_symbol = (struct symbol *)current_node->symbol_data; 

        if (current_symbol->type_of_symbol == symbol_entry) {
            /* If the symbol is of type 'symbol_entry' (declared as entry but not defined) */
            
            
        } else if (current_symbol->type_of_symbol >= symbol_entry_code) {
            /* In this case a entry symbol was successfully defined in the assembly */
            object->number_of_entries++;
        }

        if (current_symbol->type_of_symbol == symbol_entry_data || current_symbol->type_of_symbol == symbol_data) {
            /* If the symbol is of type 'symbol_entry_data' or 'symbol_data' */
            current_symbol->address_of_symbol += object->IC + BEGINNING_ADDRESS;
        }
        /* Continue to the next symbol in the linked list */
        current_node = current_node->next; 
    }
}

/*
 * Handles symbols that were missing at first.
 *
 * This function processes the list of missing symbols by retrieving their information, finding their
 * corresponding symbols in the symbol table, and updating their machine words with correct addresses.
 * If the missing symbol is an external symbol, it adds the symbol to the external symbol list.
 * If the symbol is not found or is an 'entry' symbol, an error is generated.
 *
 * @param were_to_fill_in_symbol_table A pointer to the list of missing symbols.
 * @param object A pointer to the object_file structure containing symbol and address information.
 * @param name_of_am_file The name of the am file being compiled.
 * @param error_d A pointer to the error flag that indicates the success of the compilation.
 * @param number_of_the_line The current line number in the am file.
 * @param find_symbol A pointer to a symbol structure for symbol search.
 */
void handle_missing_symbols(SymbolsNotFoundLinkedList *were_to_fill_in_symbol_table, struct object_file *object, const char *name_of_am_file, int *error_d, int number_of_the_line, struct symbol *find_symbol) {
    struct symbols_that_were_not_found_at_first *current_symbol;
    /* Store the size of the 'were_to_fill_in_symbol_table' list */
    size_t length_were_to_fill_in_symbol_table = were_to_fill_in_symbol_table->size_of_linked_list;
    size_t index = 0;

    /* Iterate through the list of missing symbols */
    while (index < length_were_to_fill_in_symbol_table) {
        /* Retrieve the current missing symbol from the list */
        current_symbol = (struct symbols_that_were_not_found_at_first *)were_to_fill_in_symbol_table->head->data;
        
        if (current_symbol) {
            /* Check if the machine_word is set */
            if (current_symbol->machine_word) {
                /* Find the symbol in the object's symbol table */
                find_symbol = find_symbol_in_linked_list(object->table_of_symbols, current_symbol->name_of_symble);
                
                if (find_symbol && (find_symbol->type_of_symbol != symbol_entry)) {
                    /* If the symbol is found and not an entry symbol */
                    current_symbol->machine_word = find_symbol->address_of_symbol << 2;

                    if (find_symbol->type_of_symbol == symbol_extern) {
                        /* Set the 'external' */
                        current_symbol->machine_word |= 1;
                        /* Add the symbol into the external symbol list */
                        add_external_symbol(&(object->name_and_addresses_certain_extern), find_symbol->name_of_symbol, current_symbol->address_of_calling);
                    } else {
                        current_symbol->machine_word |= 2;
                    }
                    
                } else {
                    /* If the symbol is not found or is an 'entry' symbol, generate an error */
                    error_fmt(name_of_am_file, number_of_the_line, "The label: '%s' was called in line: '%d' but was not defined in the file.", current_symbol->name_of_symble, current_symbol->line_it_was_called);
                    /* Reset the error flag */
                    *error_d = 0;
                }
            } else {
                /* In case memory allocation for 'machine_word' failed, terminate with an error */
                fprintf(stderr, "Memory allocation for current_symbol->machine_word failed\n");
                exit(1);
            }
            }
            /* Continue to the next node in the 'were_to_fill_in_symbol_table' list */
            were_to_fill_in_symbol_table->head = were_to_fill_in_symbol_table->head->next;
            index++;
    }

    
}

/*
 * This function compiles the given am file.
 *
 * The function reads the am file line by line, creates an Abstract Syntax Tree (AST) for each line,
 * and processes the instructions and directives. It handles symbol table management, missing symbols,
 * and other aspects of the compilation process.
 *
 * @param am_file A pointer to the am file.
 * @param object A pointer to the object_file structure.
 * @param name_of_am_file The name of the am file being compiled.
 * @return 1 if the compilation process finishes successfully, 0 if errors are encountered.
 */
static int compilation_function(FILE * am_file, struct object_file * object, const char * name_of_am_file)
{
    /* Buffer to hold a line */
    char line_buffer[MAX_LENGTH_OF_LINE + 1] = {0}; 
    /* Structure to store the Abstract Syntax Tree for each line */
    mmn14_ast ast; 
    struct symbol local_symbol = {0};
    /* Pointer to symbol for symbol search */
    struct symbol * find_symbol = NULL;
    /* List to store missing symbols */
    SymbolsNotFoundLinkedList *were_to_fill_in_symbol_table = NULL; 
    /* Saves the number of the line */
    int number_of_the_line = 1;
    /* This is a error flag, to know if the compilation finished succesfuly , if error_d == 1than fnished succesfuly, if error_d == 0 than didnt finish succesfuly */
    int error_d = 1; 
     
    /* Iterate through each line in the am file */
     while(fgets(line_buffer, sizeof(line_buffer), am_file)) 
     {
           /* Get the Abstract Syntax Tree (AST) for the current line using the lexer */
           ast = get_ast_lexer(line_buffer); 
           /* check if the line is empty or just a newline character */
            if (line_buffer[0] == '\0' || (line_buffer[0] == '\n' && line_buffer[1] == '\0')) {
                continue;  /* Skip this iteration */
            }
            /* Check for syntax errors in the AST */
           if (ast.syntax_error[0] != '\0') 
           {
              /* Print the syntax error and update the error flag */
              error_fmt(name_of_am_file, number_of_the_line, "%s", ast.syntax_error);
              number_of_the_line++;
              error_d = 0; 
              continue;
           }
           /* Check if the line has a label */
           if (ast.name_of_label[0] != '\0') 
           {
                /* Copy the label name into the local symbol */
                strcpy(local_symbol.name_of_symbol, ast.name_of_label); 
                /* Search for the symbol in the object's symbol table */
                find_symbol = find_symbol_in_linked_list(object->table_of_symbols, ast.name_of_label);
                /* Process the instruction lines */
                if (ast.mmn14_ast_options == mmn14_ast_instruction)
                {
                    if (find_symbol) 
                    {
                        if (find_symbol->type_of_symbol != symbol_entry) 
                        {
                           /* Error: Label was already defined */
                           error_fmt(name_of_am_file, number_of_the_line, "The label '%s': '%s' was defined already in line: '%d'.", find_symbol->name_of_symbol, string_type_of_symbol[find_symbol->type_of_symbol], find_symbol->line_of_declaration);
                           /* Update error flag */
                           error_d = 0;
                        }else{ 
                             
                            /* Update the entry symbol's address */
                            find_symbol->address_of_symbol = object->IC + BEGINNING_ADDRESS; 
                        }
                    }else{ 
                        /* Insert the new code symbol into the symbol table */
                        local_symbol.type_of_symbol = symbol_code; 
                        local_symbol.address_of_symbol = object->IC + BEGINNING_ADDRESS; 
                        local_symbol.line_of_declaration = number_of_the_line;

                        insert_symbol_to_linked_list(&(object->table_of_symbols), &local_symbol );
                    }
                }else if(ast.mmn14_ast_options == mmn14_ast_directive){ 
                     /* Process directive lines */
                     if (ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt == mmn14_ast_directive_entry ||
                         ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt == mmn14_ast_directive_extern)
                     {
                        /* Entries and externs have no meaning here */
                     }else{
                        if (find_symbol) 
                        {
                            /* Checks if string or data directives */
                           if (ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt == mmn14_ast_directive_string ||
                                 ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt == mmn14_ast_directive_data)
                           {
                               if (find_symbol->type_of_symbol != symbol_entry)
                               {
                                  /* Error: Label was already defined */
                                  error_fmt(name_of_am_file, number_of_the_line, "The label '%s': '%s' was defined already in line: '%d'.", find_symbol->name_of_symbol, string_type_of_symbol[find_symbol->type_of_symbol], find_symbol->line_of_declaration);
                                  /* Update error flag */
                                  error_d = 0;
                               }else{ 
                                  /* Update the entry symbol's type */
                                  find_symbol->type_of_symbol = symbol_entry_data; 
                                  /* Update the entry symbol's address */
                                  find_symbol->address_of_symbol = object->DC; 
                                  /* Line of declaration */
                                  find_symbol->line_of_declaration = number_of_the_line; 
                                }
                            }
                        }else{

                            if (ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt == mmn14_ast_directive_string ||
                                  ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt == mmn14_ast_directive_data) 
                            {
                                /* If string or data directives */

                                /* Update the symbol type */
                                local_symbol.type_of_symbol = symbol_data; 
                                /* Update the symbol address */
                                local_symbol.address_of_symbol = (unsigned int)object->DC; 
                                /* Line thats declared */
                                local_symbol.line_of_declaration = number_of_the_line;
                                /* Insert the new data symbol into the symbol table */
                                insert_symbol_to_linked_list(&(object->table_of_symbols), &local_symbol);
                            }
                       }
                    }
                }
            }
        /* Process the AST options */
        switch (ast.mmn14_ast_options){
            case mmn14_ast_instruction:
                /* Process instruction AST */
                process_ast_instruction(ast, object, number_of_the_line, &were_to_fill_in_symbol_table, &find_symbol, &local_symbol);

            break;
            case mmn14_ast_directive:
                /* Process directive AST */
                process_ast_directive(ast, object, name_of_am_file, number_of_the_line, &find_symbol, &local_symbol);
            
            break;
            case mmn14_ast_syntax_error:
                
            break;
        }
        /* Continue to the next line */
        number_of_the_line++; 
     }
    /* Handle the symbol table */
    handle_symbol_table_process((object->table_of_symbols), object, name_of_am_file, &error_d);
    /* Handle missing symbols */
    handle_missing_symbols(were_to_fill_in_symbol_table, object, name_of_am_file, &error_d, number_of_the_line, find_symbol);
    /* Free memory used for the missing symbols list */
    free_symbols_not_found_linked_list(&were_to_fill_in_symbol_table);
    
    return error_d; 
} /* END OF compilation_function */

/*
 * This function takes the number of input files and their names, 
 * iterates through each file, preprocesses the file and compiles it with using the compilation function,
 * and outputs relevant files if compilation is successful.
 *
 * @param amount_of_files The number of input files.
 * @param name_of_file An array of pointers to file names.
 * @return 0 if the assembly process completes successfully.
 */
int assembler(int amount_of_files, char ** name_of_file){
    int i; /* Loop counter */
    const char * am_name_of_file;
    FILE * am_file; /* Pointer to the am file */
    struct object_file current_object_file; 
    
   /* Iterate through the list of input files */
   for(i = 0; i < amount_of_files; i++){
       /* Checks if the current file name is NULL */
       if (name_of_file[i] == NULL)
        {
           /* If the current file name is NULL than skip to the next iteration */
           continue;
        }
   }
    /* Iterate through the list of input files again */
    for(i = 0; i < amount_of_files; i++){
        /* Checks if the current file name is NULL */
        if (name_of_file[i] == NULL)
        {
           continue;
        }
        /* Preprocess the am file name */
        am_name_of_file = file_preprocessor(name_of_file[i]); 
        /* Checks if preprocessing was successful */
        if (am_name_of_file)
        {
            /* If was successful, than open the am file for reading */
            am_file = fopen(am_name_of_file, "r");
            /* Checks if the file was opened successfully */
            if (am_file)
            {
                /* Create a new object file structure */
                current_object_file = assembler_new_object_file();
                /* Compile the am file with using the compilation function */
                if (compilation_function(am_file, &current_object_file, am_name_of_file) == 1) 
                {
                     /* Output the relevent files */
                    output(name_of_file[i], &current_object_file);
                }
                /* Close the am file */
                fclose(am_file);
                /* Delete the object file structure */
                assembler_delete_object_file(&current_object_file);
            }

            
        }

    }
    return 0;
}


