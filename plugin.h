#ifdef _WIN32
    #include <windows.h>
    #define dlopen(a, b) LoadLibrary(a)
    #define dlsym(a, b) GetProcAddress(a, b)
    #define dlclose(a) FreeLibrary(a)
    #define RTLD_LAZY 0 
    #define EXT ".dll"
#else 
    #include <dlfcn.h>
    #define EXT ".so"
#endif
typedef double (*func_t) (const char* func_name, double* args, int args_count, int* flag);
extern int plugin_count;
extern char** plugin_name;
extern func_t* plugin_func;
extern void** handle;
void load_all_plugins();
void unload_all_plugins();
double search_func(const char* func_name, double* args, int args_count);