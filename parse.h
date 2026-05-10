#pragma once
typedef struct vector_t {
    char* str;
    int size;
    int capacity;
} vector;
void vector_push(vector* vector, char new_symbol);
void parse(FILE* file);
int parse_cell_adress_col(char** expr_ptr);
int parse_cell_adress_row(char** expr_ptr);
