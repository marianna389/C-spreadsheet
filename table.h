#pragma once
#define EMPTY 0
#define NUMBER 1
#define STRING 2
#define FORMULA 3
typedef struct cell_t {
    int type;
    union {
        char* string;
        char* formula;
        double number;
    } value;
    int visited;
    int calculated;
    double calculated_value;
    char* calculated_string;
} cell;
typedef struct vector_row_t {
    cell** row; 
} vector_row;
typedef struct vector_table_t {
    vector_row* table;
    int rows_count;
    int rows_capacity;
    int cols_count;
    int cols_capacity;
} table;
extern table* main_table;
void create_table();
void add_col(table* table, int col);
void add_row(table* table, int row);
cell* add_elem(table* table, int row, int col);
void add_number(table* table, int row, int col, double new_val);
void add_string(table* table, int row, int col, char* new_string);
void add_formula(table* table, int row, int col, char* new_formula);
void free_cell(table* table, int i, int j);
void free_row(table* table, int row);
void free_table(table* table);