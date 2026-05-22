#include <stdio.h>
#include "plugin.h"
void load_all_plugins() {
    for(int i = 0; i < plugin_count; i++) {
        void* hDll = dlopen(plugin_name[i], RTLD_LAZY);
        if (hDll == NULL) {
            fprintf(stderr, "Failed to load %s\n", plugin_name[i]);
            handle[i] = NULL;
            plugin_func[i] = NULL;
            continue;
        }
        func_t func = (func_t)dlsym(hDll, "get_func");
        if (func == NULL) {
            fprintf(stderr, "Can't find \"get_func\" in %s\n", plugin_name[i]);
            handle[i] = NULL;
            plugin_func[i] = NULL;
            dlclose(hDll);
            continue;
        }
        plugin_func[i] = func;
        handle[i] = hDll;
    }
}
void unload_all_plugins() {
    for(int i = 0; i < plugin_count; i++) {
        if (handle[i] != NULL) {
            dlclose(handle[i]);
        }
    }
}
double search_func(const char* func_name, double* args, int args_count) {
    int flag = 0;
    for(int i = 0; i < plugin_count; i++) {
        if (plugin_func[i] == NULL) {
            continue;
        }
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