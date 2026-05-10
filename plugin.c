#include <stdio.h>
#include "plugin.h"
void load_all_plugins() {
    for(int i = 0; i < plugin_count; i++) {
        void* hDll = dlopen(plugin_name[i], RTLD_LAZY);
        func_t func = (func_t)dlsym(hDll, "get_func");
        plugin_func[i] = func;
        handle[i] = hDll;
    }
}
void unload_all_plugins() {
    for(int i = 0; i < plugin_count; i++) {
        dlclose(handle[i]);
    }
}
double search_func(const char* func_name, double* args, int args_count) {
    int flag = 0;
    for(int i = 0; i < plugin_count; i++) {
        double result = plugin_func[i](func_name, args, args_count, &flag);
        if (!flag) {
            continue;
        }
        else {
            return result;
        }
    }
    fprintf(stderr, "Missing function %s\n", func_name);
    return 0;
}