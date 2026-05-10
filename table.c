#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
table* main_table;
void create_table() {
    main_table = calloc(1, sizeof(table));
    main_table->rows_capacity = 1;
    main_table->cols_capacity = 1;
    main_table->table = calloc(main_table->rows_capacity, sizeof(vector_row));
}
void add_col(table* table, int col) {
    if (col >= table->cols_count) {
        table->cols_count = col + 1;
        if (col >= table->cols_capacity) {
            int old_cols_capacity = table->cols_capacity;
            while (table->cols_capacity <= table->cols_count) {
                table->cols_capacity *= 2;
            }
            for(int i = 0; i < table->rows_count; i++) {
                if (table->table[i].row != NULL) {
                    table->table[i].row = realloc(table->table[i].row, table->cols_capacity * sizeof(cell*));
                    for(int j = old_cols_capacity; j < table->cols_capacity; j++) {
                        table->table[i].row[j] = NULL;
                    }
                }
            }
        }
    }
}
void add_row(table* table, int row) {
    if (row >= table->rows_count) {
        table->rows_count = row + 1;
    }
    if (row >= table->rows_capacity) {
        int old_rows_capacity = table->rows_capacity;
        while (table->rows_capacity <= table->rows_count) {
            table->rows_capacity *= 2;
        }
        table->table = realloc(table->table, table->rows_capacity * sizeof(vector_row));
        for(int i = old_rows_capacity; i < table->rows_capacity; i++) {
            table->table[i].row = NULL;
        }
    }
    if (table->table[row].row == NULL) {
        table->table[row].row = calloc(table->cols_capacity, sizeof(cell*));
    }
}
cell* add_elem(table* table, int row, int col) {
    add_row(table, row);
    add_col(table, col);
    if (table->table[row].row[col] == NULL) {
        table->table[row].row[col] = calloc(1, sizeof(cell));
    }
    return table->table[row].row[col];
}
void add_number(table* table, int row, int col, double new_val) {
    cell* cell_ptr = add_elem(table, row, col);
    cell_ptr->type = NUMBER;
    cell_ptr->value.number = new_val;
}
void add_string(table* table, int row, int col, char* new_string) {
    cell* cell_ptr = add_elem(table, row, col);
    cell_ptr->type = STRING;
    cell_ptr->value.string = malloc((strlen(new_string) + 1) * sizeof(char));
    strcpy(cell_ptr->value.string, new_string);
}
void add_formula(table* table, int row, int col, char* new_formula) {
    cell* cell_ptr = add_elem(table, row, col);
    cell_ptr->type = FORMULA;
    cell_ptr->value.formula = malloc((strlen(new_formula) + 1) * sizeof(char));
    strcpy(cell_ptr->value.formula, new_formula);
}
void free_cell(table* table, int i, int j) {
    if (table->table[i].row[j] == NULL) {
        return;
    }
    if (table->table[i].row[j]->type == STRING) {
        free(table->table[i].row[j]->value.string);
    }
    else if (table->table[i].row[j]->type == FORMULA) {
            free(table->table[i].row[j]->value.formula);
            if (table->table[i].row[j]->calculated_string != NULL) {
                free(table->table[i].row[j]->calculated_string);
            }
    }
    free(table->table[i].row[j]);
}
void free_row(table* table, int row) {
    if (table->table[row].row == NULL) {
        return;
    }
    for(int i = 0; i < table->cols_capacity; i++) {
        free_cell(table, row, i);
    }
    free(table->table[row].row);
}
void free_table(table* table) {
    for(int i = 0; i < table->rows_capacity; i++) {
        free_row(table, i);
    }
    free(table->table);
    free(table);
}