#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "table.h"
#include "parse.h"
void vector_push(vector* vector, char new_symbol) {
    if (vector->capacity == 0) {
        vector->capacity = 1;
        vector->str = malloc(sizeof(char) * vector->capacity);
    }
    else if (vector->size == vector->capacity) {
        vector->capacity *= 2;
        vector->str = realloc(vector->str, sizeof(char) * vector->capacity);
    }
    vector->str[vector->size++] = new_symbol;
}
void parse(FILE* file) {
    char symbol; 
    int row_index = 0;
    int col_index;
    int start = 1;
    while((symbol = fgetc(file)) != EOF) {
        if (start) {
            col_index = 0;
            start = 0;
            if (symbol == ',') {
                add_elem(main_table, row_index, col_index);
                col_index++;
            }
        }
        if (symbol == ',') {
            char old_symbol = symbol;
            symbol = fgetc(file);
            if (symbol == ',') {
                add_elem(main_table, row_index, col_index);
                col_index++;
            }
            if (symbol == '\n' || symbol == EOF) {
                add_elem(main_table, row_index, col_index);
                col_index++;
            }
            ungetc(symbol, file);
            symbol = old_symbol;
        }
        if (symbol == '\n') {
            start = 1;
            row_index++;
        }
        if (symbol == '"') {
            vector* string = calloc(1, sizeof(vector));
            symbol = fgetc(file);
            int formula = 0;
            if (symbol == '=') {
                symbol = fgetc(file);
                formula = 1;
            }
            while (1) {
                if (symbol == '"') {
                    char old_symbol = symbol;
                    symbol = fgetc(file);
                    if (symbol == '"') {
                        vector_push(string, '"');
                        symbol = fgetc(file);
                    }
                    else {
                        ungetc(symbol, file);
                        symbol = old_symbol;
                        break;
                    }
                }
                else {
                    vector_push(string, symbol);
                    symbol = fgetc(file);
                }
            }
            vector_push(string, '\0');
            if (formula) {
                add_formula(main_table, row_index, col_index, string->str);
            }
            else {
                add_string(main_table, row_index, col_index, string->str);
            }
            col_index++;
            free(string->str);
            free(string);
        }
        if (symbol == '=') {
            vector* formula = calloc(1, sizeof(vector));
            symbol = fgetc(file);
            while (symbol != ',' && symbol != '\n' && symbol != EOF) {
                vector_push(formula, symbol);
                symbol = fgetc(file);
            }
            ungetc(symbol, file);
            vector_push(formula, '\0');
            add_formula(main_table, row_index, col_index, formula->str);
            col_index++;
            free(formula->str);
            free(formula);
        }
        if (symbol == '-') {
            vector* negative_number = calloc(1, sizeof(vector));
            symbol = fgetc(file);
            while (symbol != ',' && symbol != '\n' && symbol != EOF) {
                vector_push(negative_number, symbol);
                symbol = fgetc(file);
            }
            ungetc(symbol, file);
            vector_push(negative_number, '\0');
            double number;
            sscanf(negative_number->str, "%lf", &number);
            number *= -1;
            add_number(main_table, row_index, col_index, number);
            col_index++;
            free(negative_number->str);
            free(negative_number);
        }
        if (isdigit(symbol)) {
            vector* positive_number = calloc(1, sizeof(vector));
            while (symbol != ',' && symbol != '\n' && symbol != EOF) {
                vector_push(positive_number, symbol);
                symbol = fgetc(file);
            }
            ungetc(symbol, file);
            vector_push(positive_number, '\0');
            double number;
            sscanf(positive_number->str, "%lf", &number);
            add_number(main_table, row_index, col_index, number);
            col_index++;
            free(positive_number->str);
            free(positive_number);
        }
    }
}
int parse_cell_adress_col(char** expr_ptr) {
    int col = 0;
    while (isalpha(**expr_ptr)) {
        col = col * 26 + (**expr_ptr - 'A' + 1);
        (*expr_ptr)++;
    }
    return col - 1;
}
int parse_cell_adress_row(char** expr_ptr) {
    int row = 0;
    while (isdigit(**expr_ptr)) {
        row = row * 10 + (**expr_ptr - '0');
        (*expr_ptr)++;
    }
    return row - 1;
}