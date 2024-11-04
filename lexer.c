#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include "lexer.h"


/* Enum that defines the possible results of label validation in the lexer */
enum valid_label_lexer{
    label_is_valid, /* Label is valid */
    first_char_is_not_letter, /* First character of label is not a letter */
    label_has_char_that_not_letter_or_number, /* Label contains a character that is not a letter or number */
    label_is_longer_than_supposed /* Label is longer than the allowed maximum length */
};

/* Linked lists that store instruction and directive names */
 StringLinkedList *instruction_list = NULL;
 StringLinkedList *directive_list = NULL;

int linked_list_initialized = 0;
/* Structure that maps instruction information */
struct asm_instruction_mapping{
    const char * name_of_instruction; /* Name of the instruction */
    int number_of_instruction; /* Number of the instruction */
    /* Immediate -- 1, Label -- 3, Register -- 5*/
    const char * options_of_source_operand; /* Options for the source operand */
    const char * options_of_destination_operand; /* Options for the destination operand */
} asm_instruction_mapping[16] = { 
    {"mov", mmn14_ast_instruction_mov , "Immediate-Label-Register",  "Label-Register"},
    {"cmp", mmn14_ast_instruction_cmp, "Immediate-Label-Register",  "Immediate-Label-Register"},
    {"add", mmn14_ast_instruction_add, "Immediate-Label-Register",  "Label-Register"},
    {"sub", mmn14_ast_instruction_sub, "Immediate-Label-Register",  "Label-Register"},
    {"lea", mmn14_ast_instruction_lea, "Label",  "Label-Register"},

    {"not", mmn14_ast_instruction_not, NULL,  "Label-Register"},
    {"clr", mmn14_ast_instruction_clr, NULL,  "Label-Register"},
    {"inc", mmn14_ast_instruction_inc, NULL,  "Label-Register"},
    {"dec", mmn14_ast_instruction_dec, NULL,  "Label-Register"},
    {"jmp", mmn14_ast_instruction_jmp, NULL,  "Label-Register"},
    {"bne", mmn14_ast_instruction_bne, NULL,  "Label-Register"},
    {"red", mmn14_ast_instruction_red, NULL,  "Label-Register"},
    {"prn", mmn14_ast_instruction_prn, NULL,  "Immediate-Label-Register"},
    {"jsr", mmn14_ast_instruction_jsr, NULL,  "Label-Register"},

    {"rts", mmn14_ast_instruction_rts, NULL,  NULL},
    {"stop", mmn14_ast_instruction_stop, NULL,  NULL}

};
/* Structure that maps directive information */
struct asm_directive_mapping{
    const char * name_of_directive; /* Name of the directive */
    int number_of_directive; /* Number of the directive */
}asm_directive_mapping[4] = {
    {"data", mmn14_ast_directive_data},
    {"string", mmn14_ast_directive_string},
    {"extern", mmn14_ast_directive_extern},
    {"entry", mmn14_ast_directive_entry},
};

/*
 * Find and return the instruction mapping structure based on the given instruction name.
 *
 * The function searches through the array of instruction mappings to find the one that
 * matches the provided instruction name. If a match is found, the corresponding
 * instruction mapping structure is returned.
 *
 * @param instruction_name The name of the instruction to search for.
 * @return A pointer to the matching instruction mapping structure, or NULL if not found.
 */
struct asm_instruction_mapping *find_instruction_mapping(const char *instruction_name) {
    int i;
    for (i = 0; i < 16; i++) {
        /* Compare the given instruction_name with every entry in asm_instruction_mapping */
        if (strcmp(asm_instruction_mapping[i].name_of_instruction, instruction_name) == 0) {
            /* Return the matching instruction mapping */
            return &asm_instruction_mapping[i]; 
        }
    }
    /* Return NULL if didnt find */
    return NULL;
}

/*
 * Find and return the directive mapping structure based on the given directive name.
 *
 * The function searches through the array of directive mappings to find the one that
 * matches the provided directive name. If a match is found, the corresponding
 * directive mapping structure is returned.
 *
 * @param directive_name The name of the directive to search for.
 * @return A pointer to the matching directive mapping structure, or NULL if not found.
 */
struct asm_directive_mapping *find_directive_mapping(const char *directive_name) {
    int i;
    for (i = 0; i < 4; i++) {
        /* Compare the given directive_name with every entry in asm_directive_mapping */
        if (strcmp(asm_directive_mapping[i].name_of_directive, directive_name) == 0) {
            return &asm_directive_mapping[i];
        }
    }
    /* Return NULL if didnt find */
    return NULL;
}


static char parse_operand(char * str_describing_operands, char ** label, int * constent_number, int * register_number);
static enum valid_label_lexer label_valid_lexer(const char * label);

/*
 * Inserts instruction and directive names into linked lists.
 *
 * This function initializes and populates linked lists with the names of instructions
 * and directives from the corresponding mapping arrays. It iterates through the arrays
 * of instruction and directive mappings and inserts each name into its respective linked list.
 * This function should only be called once to initialize the linked lists.
 */
static void insert_d_i_linked_list() {
    int i;
    /* Checks if the linked lists are already initialized */
    if (linked_list_initialized) {
        fprintf(stderr, "Linked list is already initialized\n");
        /* Theres no need to proceed if is already initialized */
        return;
    }

    /* Insert instruction names into instruction_list*/
    /* Iterate through the array of instruction mappings */
    for (i = 0; i < 16; i++) {
        const char *instruction_name = asm_instruction_mapping[i].name_of_instruction;
        /* Insert every instruction name in to the instruction linked list */
        insert_string_to_linked_list(&instruction_list, instruction_name);
    }
    /* Insert directive names into directive_list */
    /* Iterate through the array of directive mappings */
    for (i = 0; i < 4; i++) {
        const char *directive_name = asm_directive_mapping[i].name_of_directive;
        /* Insert every directive name in to the directive linked list */
        insert_string_to_linked_list(&directive_list, directive_name);
    }
    /* Update the flag that would tell us if the linked lists are initialized */
    linked_list_initialized = 1;
}

/* Reports a syntax error in the assembly code and updates AST */
static void report_syntax_error_and_return_ins(mmn14_ast * ast, const char * error_message, const char * instruction) {
    sprintf(ast->syntax_error, "instruction: '%s' %s", instruction, error_message);
    /* Set the AST mmn14_ast_options to indicate that a syntax error has occurred */
    ast->mmn14_ast_options = mmn14_ast_syntax_error;
    return;  
}

/*
 * Handles the parsing and processing of a single operand in an instruction.
 *
 * This function parses the given operand string using the parse_operand function,
 * and based on the parsing result, it updates the relevant fields in the Abstract Syntax Tree (AST).
 * It handles various cases such as unknown operands, out-of-range numbers, missing operands, and
 * unsupported operand options. It also updates the operand option in the AST to match the parsed operand type.
 *
 * @param ast A pointer to the Abstract Syntax Tree for the current line.
 * @param operand_str The operand string to be parsed and processed.
 * @param operand_index The index of the operand being processed.
 * @param ins_mapping A pointer to the instruction mapping structure for the current instruction.
 * @return The result of the operand parsing and processing, indicating the success or specific issue encountered.
 */
static char handle_single_operand(mmn14_ast* ast, char* operand_str, int operand_index, struct asm_instruction_mapping* ins_mapping) {
    char options_of_certain_operand;
    /* Parse the given operand using the parse_operand function */
    options_of_certain_operand = parse_operand(operand_str, 
                                               &ast->directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operands[operand_index].label,
                                               &ast->directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operands[operand_index].constent_number,
                                               &ast->directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operands[operand_index].register_number);
    /* Handle different cases based on the parsing result of the operand */
    if (options_of_certain_operand == 'U') {
        /* If operand parsing is unknown, report an error and return 'U' */
        report_syntax_error_and_return_ins(ast, "operand is unknown", ins_mapping->name_of_instruction);
        return 'U';
    } else if (options_of_certain_operand == 'C') {
        /* If operand is a number out of range, report an error and return 'C' */
        report_syntax_error_and_return_ins(ast, "number out of range", ins_mapping->name_of_instruction);
        return 'C';
    } else if (options_of_certain_operand == 'W' && ins_mapping->number_of_instruction <= mmn14_ast_instruction_jsr) {
        report_syntax_error_and_return_ins(ast, "expected operand and there's none", ins_mapping->name_of_instruction);
        return 'W';
    } else if (options_of_certain_operand != 'I' && options_of_certain_operand != 'L' && options_of_certain_operand != 'R' && ins_mapping->number_of_instruction <= mmn14_ast_instruction_jsr) {
        report_syntax_error_and_return_ins(ast, "option for operand doesn't exist", ins_mapping->name_of_instruction);
        return options_of_certain_operand;
    }
    /*  Update AST based on the parsed operand type */
    if (options_of_certain_operand == 'I') {
        /* If the operand is a constant number, set the operand option to constant number */
        ast->directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operand_opt[operand_index] = mmn14_ast_operand_opt_constant_number;
    } else if (options_of_certain_operand == 'L') {
        /* If the operand is an operand label, set the operand option to operand label */
        ast->directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operand_opt[operand_index] = mmn14_ast_operand_opt_operand_label;
    } else {
        /* If the operand is neither constant number or operand label, set the operand option to operand register */
        ast->directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_operand_opt[operand_index] = mmn14_ast_operand_opt_operand_register;
    }
    return options_of_certain_operand;
}

/*
 * Handles the parsing and processing of instruction operands.
 *
 * This function parses and processes the operands described in the given operand string,
 * based on the provided instruction mapping. It checks the expected number of operands,
 * the presence of a comma, and validates the operand options against the expected options
 * for both destination and source operands. It delegates operand handling to the
 * handle_single_operand function and ensures that parsing errors are reported accurately.
 *
 * @param ast A pointer to the Abstract Syntax Tree for the current line.
 * @param str_describing_operands The operand string describing the operands to be parsed and processed.
 * @param ins_mapping A pointer to the instruction mapping structure for the current instruction.
 */
static void instructon_operands_parsing(mmn14_ast * ast, char * str_describing_operands, struct asm_instruction_mapping * ins_mapping) {
    char * comma = NULL;
    int num_operands; /* Number of operands the instruction expects */
    const char * expected_options;
    char result;
    int i;

    /* Find the comma character if it exists in the operand string */
    if (str_describing_operands) {
        comma = strchr(str_describing_operands, ',');
    }
    /* Checks if the instruction is expected to have one operand and handle the case */
    if (!comma && ins_mapping->options_of_destination_operand && ins_mapping->number_of_instruction <= mmn14_ast_instruction_lea &&
    ins_mapping->number_of_instruction >= mmn14_ast_instruction_rts) {
    report_syntax_error_and_return_ins(ast, "instruction expects one operand", ins_mapping->name_of_instruction);
    return;
    }


    if (comma) {
        /* Checks if there is another comma after the first one */
        if (strchr(comma + 1, ',')) {
            /* If there is another comma than report an error */
            report_syntax_error_and_return_ins(ast, "not valid comma", ins_mapping->name_of_instruction);
            return;
        } else if (!ins_mapping->options_of_source_operand) { 
            /* If there's a second operand but the instruction should have only one than report an error */ 
            report_syntax_error_and_return_ins(ast, "there's two operands when instruction has only one operand", ins_mapping->name_of_instruction);
            return;
        }
        *comma = '\0';
    } else {
        if (ins_mapping->options_of_source_operand) {  
            /* If a second operand is expected but theres  no comma than report an error */
            report_syntax_error_and_return_ins(ast, "the instruction should have a comma", ins_mapping->name_of_instruction);
            return;
        }
    }
    /* Determine the number of operands based on the presence of a comma in the operand string */
    num_operands = 1;
    if (comma) { 
        num_operands = 2;  
    }
    /* Iterate through the expected operands */
    for (i = 0; i < num_operands; ++i) {
        if (i == 0) {
            expected_options = ins_mapping->options_of_destination_operand; 
        } else {
            expected_options = ins_mapping->options_of_source_operand;
        }
        /* Parse and validate the current operand using the handle_single_operand function */
        result = handle_single_operand(ast, str_describing_operands, i, ins_mapping);

        
        if (result == 'U' || result == 'C' || result == 'W') {
            return;
        } else {
            /* Check if the parsed operand matches the expected operand options */
            if (!strchr(expected_options, result)) {
                return;
            }
        }
        /* If a comma is present, move the operand string pointer to the next operand */
        if (comma) {
            str_describing_operands = comma + 1;
        }
    }
}
 
/* This function reports a syntax error in the context of a directive*/
static void report_syntax_error_and_return_dir(mmn14_ast * ast, const char * error_message, const char * directive) {
    /* Construct the syntax error message including the directive name */
    sprintf(ast->syntax_error, "directive: '%s' %s", directive, error_message);
    /* Update the AST to indicate a syntax error */
    ast->mmn14_ast_options = mmn14_ast_syntax_error;
    return;  
}

/*
 * Handles the parsing and processing of a string directive operand.
 *
 * This function extracts the string content from the provided string, checks for the presence
 * of opening and closing quotation marks, verifies the absence of unexpected characters,
 * and updates the AST with the parsed string content for the specified directive.
 *
 * @param ast A pointer to the Abstract Syntax Tree (AST) for the current line.
 * @param str The string containing the operand content.
 * @param dir_mapping A pointer to the directive mapping structure for the corresponding directive.
 */
static void handle_string(mmn14_ast * ast, char * str, struct asm_directive_mapping * dir_mapping){
    char * opening_quotation_mark;
    char * closing_quotation_mark;
    
    /* Find the opening quotation mark character */
    opening_quotation_mark = strchr(str, '"');

    if (!opening_quotation_mark){
        report_syntax_error_and_return_dir(ast, "is missing opening quotation mark.", dir_mapping->name_of_directive);
        return;
    }

    opening_quotation_mark++;

    /* Find the closing quotation mark character */
    closing_quotation_mark = strchr(opening_quotation_mark, '"');

    if (!closing_quotation_mark){
        report_syntax_error_and_return_dir(ast, "is missing closing quotation mark.", dir_mapping->name_of_directive);
        return;
    }

    *closing_quotation_mark = '\0';
    closing_quotation_mark++;
    SKIP_SPACE(closing_quotation_mark);
    
    /* Checks for unexpected characters after the closing quotation mark*/
    if (*closing_quotation_mark != '\0'){
        report_syntax_error_and_return_dir(ast, "contains unexpected characters after the string.", dir_mapping->name_of_directive);
        return;
    }
    
    /* Update the AST with the parsed string content */
    ast->directive_or_instruction.mmn14_ast_directive.directive_operand.string = opening_quotation_mark;
}

/*
 * Handles the parsing and processing of a data directive operand.
 *
 * This function parses a comma-separated list of numbers from the provided string, verifies
 * their validity, and updates the AST with the parsed numbers for the specified directive.
 *
 * @param ast A pointer to the Abstract Syntax Tree (AST) for the current line.
 * @param str The string containing the operand content.
 * @param dir_mapping A pointer to the directive mapping structure for the corresponding directive.
 */
static void handle_data(mmn14_ast * ast, char * str, struct asm_directive_mapping * dir_mapping){
    char * comma;
    int current_number;
    int num_of_numbers = 0;

    do {
        /* Find the comma character in the string */
        comma = strchr(str, ',');
        if (comma)
            *comma = '\0';
        /* Parse the operand and handle different cases */
        switch(parse_operand(str, NULL, &current_number, NULL)){
            case 'I':
                /* Operand is a valid integer */
                ast->directive_or_instruction.mmn14_ast_directive.directive_operand.data.data[num_of_numbers] = current_number;
                num_of_numbers++;
                ast->directive_or_instruction.mmn14_ast_directive.directive_operand.data.num_of_numbers = num_of_numbers;
                break;
            case 'C':
                /* Operand is a number out of range */
                report_syntax_error_and_return_dir(ast, "contains out of range number.", dir_mapping->name_of_directive);
                return;
            case 'W':
                /* Operand is whitespace where a number is expected */
                report_syntax_error_and_return_dir(ast, "contains whitespace where a number is expected.", dir_mapping->name_of_directive);
                return;
            default:
                report_syntax_error_and_return_dir(ast, "contains an unexpected operand: A number was expected.", dir_mapping->name_of_directive);
                return;
        }
        
        /* Move the string pointer to the next part after the comma */
        if (comma)
            str = comma + 1;
        else
            break;

    } while (1);
}

/*
 * Handles the parsing of operands for directives.
 *
 * This function parses and processes operands for different types of directives.
 * It checks the type of directive and calls the appropriate handler functions
 * for parsing the operands based on the directive's requirements.
 *
 * @param ast Pointer to the Abstract Syntax Tree structure for the current line.
 * @param str_describing_operands The string describing the operands for the directive.
 * @param dir_mapping Pointer to the directive mapping structure for the current directive.
 */
static void directive_operands_parsing(mmn14_ast * ast, char * str_describing_operands, struct asm_directive_mapping * dir_mapping){
    
    /* Check if the directive is an entry or extern */
    if (dir_mapping->number_of_directive == mmn14_ast_directive_entry || dir_mapping->number_of_directive == mmn14_ast_directive_extern){
        /*  Parse operand and handle the case for entry or extern directive */
        if (parse_operand(str_describing_operands, &ast->directive_or_instruction.mmn14_ast_directive.directive_operand.name_of_label, NULL, NULL) != 'L'){
            report_syntax_error_and_return_dir(ast, "contains an invalid operand.", dir_mapping->name_of_directive);
            return;
        }
    }
    /* Check if the directive is a string */
    if (dir_mapping->number_of_directive == mmn14_ast_directive_string){
        /* Handle string directive */
        handle_string(ast, str_describing_operands, dir_mapping);
    }/* Check if the directive is data */
    else if (dir_mapping->number_of_directive == mmn14_ast_directive_data){
        /* Handle data directive */
        handle_data(ast, str_describing_operands, dir_mapping);
    }
}

/*
 * Parses and validates a number thats in a specified range.
 *
 * This function checks whether the parsed
 * number is within the specified range. It handles error cases such as overflow,
 * underflow, invalid characters, and range violations.
 *
 * @param str The string to parse as a number.
 * @param pointer_to_end Pointer to store the position where the parsing ended, or NULL.
 * @param number Pointer to store the parsed number.
 * @param min Minimum value allowed for the parsed number.
 * @param max Maximum value allowed for the parsed number.
 * @return 0 if parsing and validation are successful, otherwise an error code:
 *         -1 if parsing failed, -2 if range is invalid, -3 if number is out of range.
 */
static int number_parsing(const char * str, char ** pointer_to_end, long * number, long min, long max) {
    char *temp_pointer_to_end;
    errno = 0; 

    *number = strtol(str, &temp_pointer_to_end, 10);

    if ((errno == ERANGE && (*number == LONG_MAX || *number == LONG_MIN)) || (errno != 0 && *number == 0)) {
        return -2;
    }

    if (temp_pointer_to_end == str) {
        return -1;
    }

    /* Skip white spaces */
    while (isspace((unsigned char)*temp_pointer_to_end)) {
        temp_pointer_to_end++;
    }

    /* Check if the string was fully consumed, return error is string wasnt fully consumed */
    if (*temp_pointer_to_end != '\0') {
        return -1;
    }

    /* Check if number is within the given range */
    if (*number < min || *number > max) {
        return -3;
    }

    /* If the caller cares about where the number ended, give them that information */
    if (pointer_to_end != NULL) {
        *pointer_to_end = temp_pointer_to_end;
    }

    return 0;
}

/*
 * Validates the format of a label.
 *
 * This function checks whether a given label a valid label,
 * including rules about the length and characters used. It returns an
 * enumeration indicating whether the label is valid or the specific issue found.
 *
 * @param label The label to be validated.
 * @return An enumeration indicating the validity status of the label.
 */
static enum valid_label_lexer label_valid_lexer(const char * label) {
    int num_of_chars = 0;

    /* Checks if first character is a letter */
    if (!isalpha(*label)) {
        return first_char_is_not_letter;
    }

    num_of_chars++;
    label++;

    /* The characters after the first character must be alphanumeric (letter or number) */
    while (*label && isalnum((unsigned char)*label)) {
        num_of_chars++;
        label++;
    }

    /* Checks if the string was fully consumed and returns error if not */
    if (*label != '\0') {
        return label_has_char_that_not_letter_or_number;
    }

    /* Checks if length of label is within the maximum length */
    if (num_of_chars > LABEL_MAX_LENGTH) {
        return label_is_longer_than_supposed;
    }

    return label_is_valid;
}

/*
 * Parse and categorize an operand string into various operand types.
 *
 * This function analyzes the provided operand string to determine its type and extract
 * relevant information if applicable, such as labels, constants, or register numbers.
 * It handles different operand formats, such as labels, constants, and register references.
 *
 * @param operand_str The operand string to be parsed.
 * @param label Pointer to store the extracted label if applicable, or NULL.
 * @param constant Pointer to store the extracted constant value if applicable, or NULL.
 * @param reg_number Pointer to store the extracted register number if applicable, or NULL.
 * @return A character indicating the operand type:
 *         'L' for label, 'I' for constant, 'R' for register, 'C' for constant out of range,
 *         'U' for unknown or invalid operand, 'W' for whitespace or missing operand.
 */
static char parse_operand(char * operand_str, char ** label, int * constant, int * reg_number){
    char * t;
    long number;
    int parsing_result;
    char * search_spaces;

    /* Check if the operand string is NULL */
    if (operand_str == NULL)
    {
        return 'W';
    }
    
    /* Skip white spaces*/
    SKIP_SPACE(operand_str);
    
    /* Check if the operand string is empty after skipping spaces */
    if (*operand_str == '\0')
    {
        return 'W';
    }
    /* Check if the operand is a register reference */
    if (*operand_str == '@')
    {
        if(*(operand_str + 1) == 'r')
        {    
            /* Check for invalid register format */
            if (*(operand_str + 2) == '+' || *(operand_str + 2) == '-')
            {
                return 'U';
            }
            /* Try to pars the register number */
            parsing_result = number_parsing(operand_str + 2, NULL, &number, MIN_NUM_OF_REGISTER, MAX_NUM_OF_REGISTER);

            if (parsing_result != 0)
            {
                return 'U';
            }
            
            if (reg_number)
            {
                *reg_number = (int)number;
            }

            return 'R';
        }

        return 'U';
    }
    /* Check if the operand starts with a letter */
    if (isalpha(*operand_str))
    {
        
        t = search_spaces = strpbrk(operand_str, SPACE_CHARS);
        if (search_spaces)
        {
            /* Terminate at the space */
            *search_spaces = '\0';
            search_spaces++;
            SKIP_SPACE(search_spaces);
            /* Check for unexpected characters after the label */
            if (*search_spaces != '\0')
            {
               *t = ' ';
               return 'U';
            }
        }
        /* Validate the label format */
        if (label_valid_lexer(operand_str) != label_is_valid)
        {
            return 'U';
        }

        if (label)
        {
            *label = operand_str;
        }

        return 'L';
    }
    /* Try to parse the operand as a number */
    parsing_result = number_parsing(operand_str, NULL, &number, MIN_NUMBER, MAX_NUMBER);
    if (parsing_result < -2)
    {
        return 'C'; /* out of range */
    }
    else if(parsing_result == 0)
    {
        if (constant)
        {
            *constant = (int)number;
        }
        /* Constant operand */
        return 'I';
    }

    return 'U';
}

/*
 * Generate a mmn14_ast structure to represent the parsed logical line.
 * This function processes a logical line, identifies labels, instructions, and directives,
 * and constructs a mmn14_ast structure to encapsulate the parsed information.
 *
 * @param logical_line The logical line being parsed.
 * @return A mmn14_ast structure capturing the parsed logical line.
 */
mmn14_ast get_ast_lexer(char * logical_line){
    mmn14_ast ast = {0};
    enum valid_label_lexer lable1;
    struct asm_instruction_mapping * ins_mapping = NULL;
    struct asm_directive_mapping * dir_mapping = NULL;
    char * p1;
    char * p2;
    if (!linked_list_initialized)
    {
        /* Ensure instruction and directive linked lists are initialized */
        insert_d_i_linked_list(); 
    }
    /* Remove newline characters */
    logical_line[strcspn(logical_line, "\r\n")] = 0;
    /* Skip whitespace */
    SKIP_SPACE(logical_line);

    /* Look for the first colon */
    p1 = strchr(logical_line, ':');
    if (p1)
    {
        /* Search for a second colon */
        p2 = strchr(p1 + 1, ':');
        if (p2)
        {
            strcpy(ast.syntax_error, "there's ':' twice in the line");
            ast.mmn14_ast_options = mmn14_ast_syntax_error;
            return ast;
        }
        /* Terminate the label string */
        (*p1) = '\0';
        switch(lable1 = label_valid_lexer(logical_line)){
            case label_is_valid:
                 /* Store the label */
                 strcpy(ast.name_of_label, logical_line);
            break;
            case first_char_is_not_letter:
                 sprintf(ast.syntax_error, "the label '%s' starts with a char thats not a letter.",logical_line);
                 ast.mmn14_ast_options = mmn14_ast_syntax_error;
                 return ast;
            break;
            case label_has_char_that_not_letter_or_number:
                 sprintf(ast.syntax_error, "the label '%s' has a char thats not a letter or a number.",logical_line);
                 ast.mmn14_ast_options = mmn14_ast_syntax_error;
                 return ast;
            break;
            case label_is_longer_than_supposed:
                 sprintf(ast.syntax_error, "the label '%s' is longer than the maximum length wich is %d.", logical_line,LABEL_MAX_LENGTH);
                 ast.mmn14_ast_options = mmn14_ast_syntax_error;
                 return ast;
            break;
        }
        if (lable1 != label_is_valid)
        {
            ast.mmn14_ast_options = mmn14_ast_syntax_error;
            return ast;
        }
        /* Move the logical line pointer past the label */
        logical_line = p1+1;
        /* Skip any spaces after the label */
        SKIP_SPACE(logical_line);
    }


    if (*logical_line == '\0' && ast.name_of_label[0] != '\0'){
        sprintf(ast.syntax_error, "there's only the label '%s' in the line.", ast.name_of_label);
        ast.mmn14_ast_options = mmn14_ast_syntax_error;
        return ast;
    }


    p1 = strpbrk(logical_line, SPACE_CHARS);
    if (p1)
    {
        *p1 = '\0';
        p1++;
        SKIP_SPACE(p1);
    }

    if (*logical_line == '.')
    {
        /* Find the directive mapping */
        dir_mapping = find_directive_mapping(logical_line + 1);
        if (!dir_mapping){
           sprintf(ast.syntax_error, "the directive '%s' is unknown.", logical_line + 1);
           return ast;
        }
        ast.mmn14_ast_options = mmn14_ast_directive;
        ast.directive_or_instruction.mmn14_ast_directive.mmn14_ast_directive_opt = dir_mapping->number_of_directive;
        /* Parse directive operands */
        directive_operands_parsing(&ast ,p1, dir_mapping);
        return ast;
    }
    /* Find the instruction mapping */
    ins_mapping = find_instruction_mapping( logical_line);

    if (!ins_mapping)
    {
        sprintf(ast.syntax_error, " '%s' is unknown.", logical_line);
        ast.mmn14_ast_options = mmn14_ast_syntax_error;
        return ast;
    }
    ast.mmn14_ast_options = mmn14_ast_instruction;
    
    ast.directive_or_instruction.mmn14_ast_instruction.mmn14_ast_instruction_opt = ins_mapping->number_of_instruction;
    /* Parse instruction operands */
    instructon_operands_parsing(&ast ,p1, ins_mapping);

    return ast;

}

/*
 * Deallocate memory and clean up linked lists used for instruction and directive names.
 *
 * This function performs the following tasks:
 * - Resets the linked_list_initialized flag to indicate that the linked lists are no longer initialized.
 * - Frees the memory associated with the instruction_list linked list, which stores instruction names.
 * - Frees the memory associated with the directive_list linked list, which stores directive names.
 */
void deallocate_memory() {
    /* Reset the flag indicating linked lists are not initialized */
    linked_list_initialized = 0;
    /* Free memory used for storing instruction names */
    free_string_linked_list(&instruction_list);
    /* Free memory used for storing directive names */
    free_string_linked_list(&directive_list);
}

