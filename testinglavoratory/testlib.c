extern int serial_io_printf(const char *fmt, ...);
 
/* ---- Exported symbols ---- */
 
__attribute__((visibility("default")))
int add(int a, int b) {
    return a + b;
}
 
__attribute__((visibility("default")))
void greet(const char *name) {
    serial_io_printf("Hello from the .so! Name = %s\n", name);
}
 
__attribute__((visibility("default")))
int sum_array(const int *arr, int n) {
    int total = 0;
    for (int i = 0; i < n; i++)
        total += arr[i];
    return total;
}
 
/*
 * A global variable — exercises R_X86_64_RELATIVE relocation.
 * The loader must patch this with (base + addend).
 */
__attribute__((visibility("default")))
int global_answer = 42;