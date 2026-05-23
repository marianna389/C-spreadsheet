#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "table.h"
#include "parse.h"
#include "calculate.h"
#include "plugin.h"
double parse_expr(char** expr_ptr);
double calculate_cell(int row, int col) {
    if (main_table->table[row].row[col]->type == EMPTY) {
        return 0;
    }
    if (main_table->table[row].row[col]->type == NUMBER) {
        return main_table->table[row].row[col]->value.number;
    }
    if (main_table->table[row].row[col]->type == STRING) {
        fprintf(stderr, "You are using a string in an arithmetic expression\n");
        return 0;
    }
    if (main_table->table[row].row[col]->visited) {
        fprintf(stderr, "There is a cycle\n");
        return NAN;
    }
    if (main_table->table[row].row[col]->calculated) {
        return main_table->table[row].row[col]->calculated_value;
    }
    main_table->table[row].row[col]->visited = 1;
    char* ptr = main_table->table[row].row[col]->value.formula;
    double result = parse_expr(&ptr);
    main_table->table[row].row[col]->calculated = 1;
    main_table->table[row].row[col]->calculated_value = result;
    main_table->table[row].row[col]->visited = 0;
    return result;
}
double FIND(char** expr_ptr);
double SUM(char** expr_ptr);
double MIN(char** expr_ptr);
double MAX(char** expr_ptr);
double parse_atom(char** expr_ptr);
double parse_monome(char** expr_ptr);
double parse_expr(char** expr_ptr) {
    double val1 = parse_monome(expr_ptr);
    while ((**expr_ptr) != '\0' && (**expr_ptr == '+' || **expr_ptr == '-')) {
        char oper = **expr_ptr;
        (*expr_ptr)++;
        double val2 = parse_monome(expr_ptr);
        if (isnan(val1) || isnan(val2)) return NAN;
        if (oper == '+') {
            val1 += val2;
        }
        else {
            val1 -= val2;
        }
    }
    return val1;
}
double parse_monome(char** expr_ptr) {
    double val1 = parse_atom(expr_ptr);
    while ((**expr_ptr) != '\0' && (**expr_ptr == '*' || **expr_ptr == '/')) {
        char oper = **expr_ptr;
        (*expr_ptr)++;
        double val2 = parse_atom(expr_ptr);
        if (isnan(val1) || isnan(val2)) return NAN;
        if (oper == '*') {
            val1 *= val2;
        }
        else {
            val1 /= val2;
        }
    }
    return val1;
}
double parse_atom(char** expr_ptr) {
    char c = **expr_ptr;
    if (c != '\0' && c == '(') {
        (*expr_ptr)++;
        double res = parse_expr(expr_ptr);
        (*expr_ptr)++;
        return res;
    }
    else if (c != '\0' && isalpha(c)) {
        char* start_ptr = *expr_ptr;
        char func[15];
        int n = 0;
        while (isalpha(**expr_ptr)) {
            func[n++] = **expr_ptr;
            (*expr_ptr)++;
        }
        func[n] = '\0';
        if (**expr_ptr == '(') {
            (*expr_ptr)++;
            if (strcmp(func, "FIND") == 0) {
                return FIND(expr_ptr);
            }
            if (strcmp(func, "SUM") == 0) {
                return SUM(expr_ptr);
            }
            else if (strcmp(func, "MIN") == 0) {
                return MIN(expr_ptr);
            }
            else if (strcmp(func, "MAX") == 0) {
                return MAX(expr_ptr);
            }
            else {
                double args[10];
                int args_count = 0;
                while(**expr_ptr != '\0') {
                    args[args_count++] = parse_atom(expr_ptr);
                    (*expr_ptr)++;
                }
                return search_func(func, args, args_count);
            }
        }
        else {
            *expr_ptr = start_ptr;
            int col = parse_cell_adress_col(expr_ptr);
            int row = parse_cell_adress_row(expr_ptr);
            if (col < 0 || row < 0 || col >= main_table->cols_count || row >= main_table->rows_count)  {
                fprintf(stderr, "Reference to non-existent cell\n");
                return 0;
            }
            return calculate_cell(row, col);
        }
    }
    else if (c != '\0' && c == '-'){
        (*expr_ptr)++;
        double res = parse_atom(expr_ptr);
        return -res;
    }
    else if (c != '\0' && isdigit(c)){
        char* end_ptr; 
        double number = strtod(*expr_ptr, &end_ptr);
        *expr_ptr = end_ptr;
        return number;
    }
    if (**expr_ptr != '\0') {
        (*expr_ptr)++;
    }
    fprintf(stderr, "Syntax error\n");
    return 0;
}
double FIND(char** expr_ptr) {
    vector* str1 = calloc(1, sizeof(vector));
    vector* str2 = calloc(1, sizeof(vector));
    char* substr = parse_concat_arg(expr_ptr, str1);
    (*expr_ptr)++;
    char* Str = parse_concat_arg(expr_ptr, str2);
    (*expr_ptr)++;
    if (strcmp(Str, "CYCLE") == 0 || strcmp(substr, "CYCLE") == 0) {
        return NAN;
    }
    char* strstr_ptr = strstr(Str, substr);
    double position;
    if (strstr_ptr == NULL) {
        position = -1;
    }
    else {
        position = strstr_ptr - Str + 1;
    }
    if (str1->str != NULL) {
        free(str1->str);
    }
    free(str1);
    if (str2->str != NULL) {
        free(str2->str);
    }
    free(str2);
    return position;
}
double SUM(char** expr_ptr) {
    int col1 = parse_cell_adress_col(expr_ptr);
    int row1 = parse_cell_adress_row(expr_ptr);
    if (col1 < 0 || row1 < 0 || col1 >= main_table->cols_count || row1 >= main_table->rows_count)  {
        fprintf(stderr, "Reference to non-existent cell\n");
        return 0;
    }
    (*expr_ptr)++;
    int col2 = parse_cell_adress_col(expr_ptr);
    int row2 = parse_cell_adress_row(expr_ptr);
    if (col2 < 0 || row2 < 0 || col2 >= main_table->cols_count || row2 >= main_table->rows_count)  {
        fprintf(stderr, "Reference to non-existent cell\n");
        return 0;
    }
    double sum = 0;
    for(int i = row1; i <= row2; i++) {
        for(int j = col1; j <= col2; j++) {
            if (main_table->table[i].row[j]->type == STRING || (main_table->table[i].row[j]->type == FORMULA && strstr(main_table->table[i].row[j]->value.formula, "CONCAT") != NULL)) {
                continue;
            }
            else {
                double val = calculate_cell(i, j);
                if (isnan(val)) return NAN;
                sum += val;
            }
        }
    }
    (*expr_ptr)++;
    return sum;
}
double MIN(char** expr_ptr) {
    int col1 = parse_cell_adress_col(expr_ptr);
    int row1 = parse_cell_adress_row(expr_ptr);
    if (col1 < 0 || row1 < 0 || col1 >= main_table->cols_count || row1 >= main_table->rows_count)  {
        fprintf(stderr, "Reference to non-existent cell\n");
        return 0;
    }
    (*expr_ptr)++;
    int col2 = parse_cell_adress_col(expr_ptr);
    int row2 = parse_cell_adress_row(expr_ptr);
    if (col2 < 0 || row2 < 0 || col2 >= main_table->cols_count || row2 >= main_table->rows_count)  {
        fprintf(stderr, "Reference to non-existent cell\n");
        return 0;
    }
    double min = INFINITY;
    for(int i = row1; i <= row2; i++) {
        for(int j = col1; j <= col2; j++) {
            if (main_table->table[i].row[j]->type == STRING || (main_table->table[i].row[j]->type == FORMULA && strstr(main_table->table[i].row[j]->value.formula, "CONCAT") != NULL)) {
                continue;
            }
            else {
                double tmp = calculate_cell(i, j);
                if (isnan(tmp)) return NAN;
                if (tmp < min) {
                    min = tmp;
                }
            }
        }
    }
    (*expr_ptr)++;
    return min;
}
double MAX(char** expr_ptr) {
    int col1 = parse_cell_adress_col(expr_ptr);
    int row1 = parse_cell_adress_row(expr_ptr);
    if (col1 < 0 || row1 < 0 || col1 >= main_table->cols_count || row1 >= main_table->rows_count)  {
        fprintf(stderr, "Reference to non-existent cell\n");
        return 0;
    }
    (*expr_ptr)++;
    int col2 = parse_cell_adress_col(expr_ptr);
    int row2 = parse_cell_adress_row(expr_ptr);
    if (col2 < 0 || row2 < 0 || col2 >= main_table->cols_count || row2 >= main_table->rows_count)  {
        fprintf(stderr, "Reference to non-existent cell\n");
        return 0;
    }
    double max = -INFINITY;
    for(int i = row1; i <= row2; i++) {
        for(int j = col1; j <= col2; j++) {
            if (main_table->table[i].row[j]->type == STRING || (main_table->table[i].row[j]->type == FORMULA && strstr(main_table->table[i].row[j]->value.formula, "CONCAT") != NULL)) {
                continue;
            }
            else {
                double tmp = calculate_cell(i, j);
                if (isnan(tmp)) return NAN;
                if (tmp > max) {
                    max = tmp;
                }
            }
        }
    }
    (*expr_ptr)++;
    return max;
}
void cycle(int row, int col) {
    main_table->table[row].row[col]->calculated_string = malloc(6 * sizeof(char));
    strcpy(main_table->table[row].row[col]->calculated_string, "CYCLE");
}
char* parse_concat_arg(char** expr_ptr, vector* str);
void parse_expr_concat(char** expr_ptr, int row, int col);
char* calculate_concat_cell(int row, int col) {
    if (main_table->table[row].row[col]->visited) {
        fprintf(stderr, "There is a cycle\n");
        cycle(row, col);
        main_table->table[row].row[col]->calculated = 1;
        main_table->table[row].row[col]->visited = 0;
        return main_table->table[row].row[col]->calculated_string;
    }
    if (main_table->table[row].row[col]->calculated) {
        return main_table->table[row].row[col]->calculated_string;
    }
    main_table->table[row].row[col]->visited = 1;
    char* ptr = main_table->table[row].row[col]->value.formula;
    parse_expr_concat(&ptr, row, col);
    main_table->table[row].row[col]->calculated = 1;
    main_table->table[row].row[col]->visited = 0;
    return main_table->table[row].row[col]->calculated_string;
}
void parse_expr_concat(char** expr_ptr, int row, int col) {
    while(isalpha(**expr_ptr)) {
        (*expr_ptr)++;
    }
    (*expr_ptr)++;
    vector* str1 = calloc(1, sizeof(vector));
    vector* str2 = calloc(1, sizeof(vector));
    char* arg1 = parse_concat_arg(expr_ptr, str1);
    (*expr_ptr)++;
    char* arg2 = parse_concat_arg(expr_ptr, str2);
    (*expr_ptr)++;
    if (strcmp(arg1, "CYCLE") == 0 || strcmp(arg2, "CYCLE") == 0) {
        cycle(row, col);
    }
    else {
        main_table->table[row].row[col]->calculated_string = malloc((strlen(arg1) + strlen(arg2) + 1) * sizeof(char));
        strcpy(main_table->table[row].row[col]->calculated_string, arg1);
        strcat(main_table->table[row].row[col]->calculated_string, arg2);
    }
    if (str1->str != NULL) {
        free(str1->str);
    }
    free(str1);
    if (str2->str != NULL) {
        free(str2->str);
    }
    free(str2);
}
char* parse_concat_arg(char** expr_ptr, vector* str) {
    int col, row;
    if (isalpha(**expr_ptr)) {
        col = parse_cell_adress_col(expr_ptr);
        row = parse_cell_adress_row(expr_ptr);
        if (col < 0 || row < 0 || col >= main_table->cols_count || row >= main_table->rows_count)  {
            fprintf(stderr, "Reference to non-existent cell\n");
            return "";
        }
    }
    else if (**expr_ptr == '"') {
        (*expr_ptr)++;
        while(**expr_ptr != '"') {
            vector_push(str, **expr_ptr);
            (*expr_ptr)++;
        }
        (*expr_ptr)++;
        vector_push(str, '\0');
    }
    else {
        fprintf(stderr, "Incorrect argument in function 'CONCAT' or 'FIND'\n");
        while (**expr_ptr != ',' && **expr_ptr != ')') {
            (*expr_ptr)++;
        }
        return "";
    }
    char* arg;
    if (str->str != NULL) {
        arg = str->str;
    }
    else {
        if (main_table->table[row].row[col]->type == STRING) {
            arg = main_table->table[row].row[col]->value.string;
        }
        else if (main_table->table[row].row[col]->type == FORMULA && strstr(main_table->table[row].row[col]->value.formula, "CONCAT") != NULL) {
            char* ptr = main_table->table[row].row[col]->value.formula;
            arg = calculate_concat_cell(row, col);
        }
        else {
            fprintf(stderr, "Incorrect argument in function 'CONCAT' or 'FIND'\n");
            return "";
        }
    }
    return arg;
}