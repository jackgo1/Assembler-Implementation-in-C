#ifndef __LEXER_H_
#define __LEXER_H_

#include "common.h"
#include "linked_list.h"

#define MIN_NUM_OF_REGISTER 0
#define MAX_NUM_OF_REGISTER 7
#define MAX_NUMBER 511
#define MIN_NUMBER -512
#define MAX_NUMBER_DATA 80
#define LABEL_MAX_LENGTH 31

/* Struct representing the abstract syntax tree (AST) for a parsed logical line. */
struct mmn14_ast {
    char syntax_error[250]; /* Error message for syntax-related issues */
    char name_of_label[LABEL_MAX_LENGTH + 1]; /* Name of the label, if present */
    enum {
        mmn14_ast_instruction, /* Represents an instruction in the logical line */
        mmn14_ast_directive,  /* Represents a directive in the logical line */
        mmn14_ast_syntax_error /* Represents a syntax error in the logical line */
    }mmn14_ast_options;
    union {
        struct {
            enum {
                
                mmn14_ast_directive_extern, /* Represents the .extern directive. */
                mmn14_ast_directive_entry, /* Represents the .entry directive. */
                mmn14_ast_directive_string, /* Represents the .string directive. */
                mmn14_ast_directive_data /* Represents the .data directive. */
            } mmn14_ast_directive_opt;
            union {
                char *name_of_label; /* Name of a label associated with the directive */
                char *string; /* String data associated with the .string directive */
                struct {
                    int data[MAX_NUMBER_DATA]; /* Integer data associated with the .data directive */
                    int num_of_numbers; /* Number of integers in the data array */
                } data; /* Struct holding data for the .data directive */
            } directive_operand;
        } mmn14_ast_directive;
        struct {
            enum {
                mmn14_ast_instruction_mov = 0,
                mmn14_ast_instruction_cmp = 1,
                mmn14_ast_instruction_add = 2,
                mmn14_ast_instruction_sub = 3,
                mmn14_ast_instruction_lea = 6,

                mmn14_ast_instruction_not = 4,
                mmn14_ast_instruction_clr = 5,
                mmn14_ast_instruction_inc = 7,
                mmn14_ast_instruction_dec = 8,
                mmn14_ast_instruction_jmp = 9,
                mmn14_ast_instruction_bne = 10,
                mmn14_ast_instruction_red = 11,
                mmn14_ast_instruction_prn = 12,
                mmn14_ast_instruction_jsr = 13,

                mmn14_ast_instruction_rts = 14,
                mmn14_ast_instruction_stop = 15
            } mmn14_ast_instruction_opt;
            enum {
                mmn14_ast_operand_opt_no_operand = 0, /* No operand for the instruction */
                mmn14_ast_operand_opt_constant_number = 1, /* Operand is a constant number */
                mmn14_ast_operand_opt_operand_label = 3, /* Operand is an operand label */
                mmn14_ast_operand_opt_operand_register = 5 /* Operand is an operand register */
            } mmn14_ast_instruction_operand_opt[2]; 
            union {
                int constent_number; /* Constant number operand */
                int register_number; /* Operand register number */
                char *label; /* Operand label */
            } mmn14_ast_instruction_operands[2]; 
        } mmn14_ast_instruction;
    } directive_or_instruction;
};



typedef struct mmn14_ast mmn14_ast;



/*
 * Generate a mmn14_ast structure to represent the parsed logical line.
 * This function processes a logical line, identifies labels, instructions, and directives,
 * and constructs a mmn14_ast structure to encapsulate the parsed information.
 *
 * @param logical_line The logical line being parsed.
 * @return A mmn14_ast structure capturing the parsed logical line.
 */
mmn14_ast get_ast_lexer(char *logical_line);

#endif
