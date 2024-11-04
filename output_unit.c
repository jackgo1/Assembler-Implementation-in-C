#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <libgen.h>
#include "output_unit.h"
#include "linked_list.h"

/*
 * Outputs a section of memory in Base64 format to a given file.
 *
 * This function takes a section of memory represented as an array of unsigned integers,
 * converts the data into Base64 format, and writes it to the specified output file.
 *
 * @param obj_file A pointer to the FILE object where the Base64-encoded data will be written.
 * @param section_of_memory A pointer to the start of the memory section to be encoded.
 * @param size The size of the memory section in number of unsigned integers.
 */
static void output_section_memory(FILE *obj_file, const unsigned int *section_of_memory, long size) {
    size_t i;
    const char *const chars_b64 = BASE64;

    for (i = 0; i < size; i++) {
        unsigned int most_significant_6_bits = (section_of_memory[i] >> 6) & 0x3F;
        unsigned int least_significant_6_bits = section_of_memory[i] & 0x3F;

        if (most_significant_6_bits >= 0 && most_significant_6_bits < 64 &&
            least_significant_6_bits >= 0 && least_significant_6_bits < 64) {
            /* Write the Base64-encoded characters to the output file */
            fprintf(obj_file, "%c%c", chars_b64[most_significant_6_bits], chars_b64[least_significant_6_bits]);
            fprintf(obj_file, "\n");
        } else {
            fprintf(stderr, "Invalid index for chars_b64 array: %u %u\n", most_significant_6_bits, least_significant_6_bits);
            
        }
    }
}

/*
 * Outputs symbol entries to an .ent file.
 *
 * This function takes the name of the .ent file and a linked list of symbols.
 * It iterates through the symbol linked list and writes
 * symbols marked as entry code or entry data to the specified .ent file.
 *
 * @param ent_name_of_file The name of the .ent file to be created.
 * @param table_of_symbols A pointer to the linked list containing the symbols.
 */
static void output_ent_file(const char * ent_name_of_file, SymbolLinkedList *table_of_symbols) {
   FILE * ent_file;
   SymbolNode *current_node;
   struct symbol *current_symbol;

   /* Open the .ent file for writing */
   ent_file = fopen(ent_name_of_file, "w");
   if (ent_file) {
      
      
      current_node = table_of_symbols->head; 
        while(current_node != NULL) {  
            current_symbol = (struct symbol *)current_node->symbol_data; 
            if(current_symbol) {
                /* Check if the symbol is of type symbol_entry_code or symbol_entry_data */
                if(current_symbol->type_of_symbol == symbol_entry_code || current_symbol->type_of_symbol == symbol_entry_data){
                    /* Write the symbol name and address to the .ent file */
                    fprintf(ent_file, "%s\t%u\n", current_symbol->name_of_symbol, current_symbol->address_of_symbol);
                }
            }
            current_node = current_node->next;  
        }
        fclose(ent_file);
   }
}

/*
 * Outputs certain externs to an .ext file.
 *
 * This function takes the name of the .ext file and a linked list of certain externs.
 * It iterates through the certain extern linked list and writes the names and addresses
 * of certain externs to the specified .ext file.
 *
 * @param ext_name_of_file The name of the .ext file to be created.
 * @param name_and_addresses_certain_extern A pointer to the linked list containing certain externs.
 */
static void output_ext_file(const char *ext_name_of_file, CertainExternLinkedList *name_and_addresses_certain_extern) {
    FILE *ext_file;
    struct certain_extern *current_extern;
    CertainExternNode *current_node_ext;
    
   
    if (name_and_addresses_certain_extern->head->next == NULL)
    {
        return;
    }
    /* Open the .ext file for writing */
    ext_file = fopen(ext_name_of_file, "w");
    if (ext_file) {
        current_node_ext = name_and_addresses_certain_extern->head;

        while (current_node_ext != NULL) {
            current_extern = (struct certain_extern *)current_node_ext->data;
            if (current_extern) {
                /* Write the certain extern name and address to the .ext file */
                fprintf(ext_file, "%s\t%ld\n", current_extern->name_of_extern, current_extern->address_of_extern);
            }
            current_node_ext = current_node_ext->next; 
        }

        fclose(ext_file);
    }
}


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
void output(char * name_of_were_to_output, const struct object_file * obj_file){
    FILE * ob_file;
    char * ob_name_of_file;
    char * ext_name_of_file;
    char * ent_name_of_file;
    size_t length_name_of_were_to_output;

    length_name_of_were_to_output = strlen(name_of_were_to_output);

    if (obj_file->number_of_entries >= 1) {
        ent_name_of_file = malloc(length_name_of_were_to_output + strlen(FILE_EXTENSION_ENT) + 1);
        if (ent_name_of_file == NULL) {
            fprintf(stderr, "wasn't able to allocate memory for ent_name_of_file\n"); 
            exit(1);
        }

        ent_name_of_file = strcat(strcpy(ent_name_of_file, name_of_were_to_output), FILE_EXTENSION_ENT);
        output_ent_file(ent_name_of_file , obj_file->table_of_symbols);
        free(ent_name_of_file);
    }

    if (get_amount_of_elements_in_certain_extern_linked_list(obj_file->name_and_addresses_certain_extern) >= 1) { 
        ext_name_of_file = malloc(length_name_of_were_to_output + strlen(FILE_EXTENSION_EXT) + 1);
        if (ext_name_of_file == NULL) {
            fprintf(stderr, "wasn't able to allocate memory for ext_name_of_file\n");
            exit(1);
        }

        ext_name_of_file = strcat(strcpy(ext_name_of_file, name_of_were_to_output), FILE_EXTENSION_EXT);
        output_ext_file(ext_name_of_file, obj_file->name_and_addresses_certain_extern);
        free(ext_name_of_file);
    }

    ob_name_of_file = malloc(length_name_of_were_to_output + strlen(FILE_EXTENSION_OB) + 1);
    if (ob_name_of_file == NULL) {
        fprintf(stderr, "wasn't able to allocate memory for ob_name_of_file\n");
        exit(1);
        
    }

    ob_name_of_file = strcat(strcpy(ob_name_of_file, name_of_were_to_output), FILE_EXTENSION_OB);
    ob_file = fopen(ob_name_of_file, "w");

    if (ob_file == NULL) {
        fprintf(stderr, "wasn't able to open file: %s\n", ob_name_of_file);
        exit(1);
    }

    fprintf(ob_file, "%lu %lu\n", (unsigned long)obj_file->IC, (unsigned long)obj_file->DC);
    if (obj_file->IC <= MEMORY_SIZE && obj_file->DC <= MEMORY_SIZE) {
           output_section_memory(ob_file, (const unsigned int*)obj_file->code_image, obj_file->IC);
           output_section_memory(ob_file, (const unsigned int*)obj_file->data_image, obj_file->DC);

        
    } else {
          fprintf(stderr, "Invalid size for code_image or data_image\n");
          exit(1);
    }

    fclose(ob_file);
    free(ob_name_of_file);
    
    
}



