#include <math.h>
#include <string.h>
double average_value(double* args, int args_count) {
    double result = 0;
    for(int i = 0; i < args_count; i++) {
        result += args[i];
    }
    result /= args_count;
    return result;
}
double square_root(double* args, int args_count) {
    return sqrt(args[0]);
}
double get_func(const char* name, double* args, int args_count, int* flag) {
    if (strcmp(name, "AVG") == 0) {
        *flag = 1;
        return average_value(args, args_count);
    }
    else if (strcmp(name, "SQUAREROOT") == 0) {
        *flag = 1;
        return square_root(args, args_count);
    }
    return 0;
}