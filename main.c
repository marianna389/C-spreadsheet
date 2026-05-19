#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "parse.h"
#include "table.h"
#include "calculate.h"
#include "plugin.h"
int plugin_count;
char** plugin_name;
func_t* plugin_func;
void** handle;
void print_string(FILE* output, char* str) {
    fprintf(output, "\"");
    int s = 0;
    while (str[s] != '\0') {
        if (str[s] == '"') {
            fprintf(output, "\"");
            fprintf(output, "%c", str[s]);
        }
        else {
            fprintf(output, "%c", str[s]);
        }
        s++;
    }
    fprintf(output, "\"");
}
void help(char* name) {
    printf("Usage: %s [options] input_file output_file\n", name);
    printf("Receives data in csv format, calculates formulas and produces a ready-made csv table.\n");
    printf("Options:\n");
    printf("  --plugins (next enter the following data: plugin_count, plugin names (the quantity must be equal to plugin_count))\n");
    printf("            You can connect your own plugins and use new formulas.\n");
    printf("  --help  Display this information.\n");
    printf("Supported functions:\n");
    printf("SUM, MIN, MAX, FIND, CONCAT\n");
}
int main(int argc, char* argv[]) {
    char input_file[50];
    char output_file[50];
    if (argc == 1) {
        printf("Failed to start the program\n");
        printf("For more information use: %s --help", argv[0]);
        return 0;
    }
    else if (strcmp(argv[1], "--help") == 0) {
        help(argv[0]);
        return 0;
    }
    else if (strcmp(argv[1], "--plugins") == 0) {
        sscanf(argv[2], "%d", &plugin_count);
        if (argc != plugin_count + 5) {
            printf("Failed to start the program\n");
            printf("For more information use: %s --help", argv[0]);
            return 0;
        }
        plugin_name = malloc(plugin_count * sizeof(char*));
        for(int i = 0; i < plugin_count; i++) {
            plugin_name[i] = malloc(50 * sizeof(char));
            snprintf(plugin_name[i], 50, "./%s%s", argv[i + 3], EXT);
        }
        plugin_func = malloc(plugin_count * sizeof(func_t));
        handle = malloc(plugin_count * sizeof(void*));
        load_all_plugins();
        strcpy(input_file, argv[argc - 2]);
        strcpy(output_file, argv[argc - 1]);
    }
    else if (argc == 3) {
        strcpy(input_file, argv[1]);
        strcpy(output_file, argv[2]);
    }
    else {
        printf("Failed to start the program\n");
        printf("For more information use: %s --help", argv[0]);
        return 0;
    }
    FILE* input = fopen(input_file, "rt");
    FILE* output = fopen(output_file, "wt");
    create_table();
    parse(input);
    for(int i = 0; i < main_table->rows_count; i++) {
        for(int j = 0; j < main_table->cols_count; j++) {
            if (main_table->table[i].row[j] == NULL) {
               main_table->table[i].row[j] = calloc(1, sizeof(cell));
            }
        }
    }
    for(int i = 0; i < main_table->rows_count; i++) {
        for(int j = 0; j < main_table->cols_count; j++) {
            if (main_table->table[i].row[j]->type == NUMBER) {
                fprintf(output, "%g", main_table->table[i].row[j]->value.number);
            }
            else if (main_table->table[i].row[j]->type == STRING) {
                print_string(output, main_table->table[i].row[j]->value.string);
            }
            else if (main_table->table[i].row[j]->type == FORMULA) {
                if (strstr(main_table->table[i].row[j]->value.formula, "CONCAT") != NULL) {
                    char* result = calculate_concat_cell(i, j);
                    print_string(output, result);
                }
                else {
                    double result = calculate_cell(i, j);
                    if (isnan(result)) {
                        fprintf(output, "\"CYCLE\"");
                    }
                    else {
                        fprintf(output, "%g", result);
                    }
                }
            }
            if (j != main_table->cols_count - 1) {
                fprintf(output, ",");
            }
        }
        fprintf(output, "\n");
    }
    free_table(main_table);
    unload_all_plugins();
    free(handle);
    free(plugin_func);
    for(int i = 0; i < plugin_count; i++) {
        free(plugin_name[i]);
    }
    free(plugin_name);
    fclose(input);
    fclose(output);
    return 0;
}