#include <string.h>
double square(double* args, int args_count) {
    return (args[0] * args[0]);
}
double get_func(const char* name, double* args, int args_count, int* flag) {
    if (strcmp(name, "SQUARE") == 0) {
        *flag = 1;
        return square(args, args_count);
    }
    return 0;
}