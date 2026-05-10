#pragma once
double calculate_cell(int row, int col);
double parse_atom(char** expr_ptr);
double parse_monome(char** expr_ptr);
double parse_expr(char** expr_ptr);
char* calculate_concat_cell(int row, int col);
void parse_expr_concat(char** expr_ptr, int row, int col);
char* parse_concat_arg(char** expr_ptr, vector* str);