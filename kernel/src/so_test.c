#include "so_loader.h"
#include "gfx/serial_io.h"
#include "libs/string.h"
#include "mem/heap.h"
 
/* -------------------------------------------------------------------------
 * Step 1: Define your kernel export table.
 *
 * List every function your test .so will call back into the kernel.
 * Add more as needed — the NULL sentinel must always be last.
 * -------------------------------------------------------------------------*/

 
/* -------------------------------------------------------------------------
 * Step 2: Function pointer types matching what test_lib.so exports.
 * -------------------------------------------------------------------------*/
typedef int  (*fn_add_t)(int a, int b);
typedef void (*fn_greet_t)(const char *name);
typedef int  (*fn_sum_array_t)(const int *arr, int n);
 
/* -------------------------------------------------------------------------
 * Step 3: The actual test
 * -------------------------------------------------------------------------*/
void so_test_run(void) {
    serial_io_printf("\n=== SO loader test ===\n");
 
    so_module_t mod;
    so_error_t  err = so_load_file("/test_lib.so", kernel_exports, &mod);
 
    if (err != SO_OK) {
        serial_io_printf("SO test FAILED to load: %s\n", so_strerror(err));
        return;
    }
 
    /* --- Test 1: simple integer add --- */
    fn_add_t add_fn = (fn_add_t)so_get_symbol(&mod, "add");
    if (add_fn) {
        int result = add_fn(3, 4);
        serial_io_printf("add(3, 4) = %d  %s\n", result,
                         result == 7 ? "[PASS]" : "[FAIL]");
    } else {
        serial_io_printf("Symbol 'add' not found  [SKIP]\n");
    }
 
    /* --- Test 2: callback into kernel (so calls serial_io_printf) --- */
    fn_greet_t greet_fn = (fn_greet_t)so_get_symbol(&mod, "greet");
    if (greet_fn) {
        serial_io_printf("Calling greet() - it will call back into kernel:\n");
        greet_fn("doorsos");
        serial_io_printf("greet() returned  [PASS]\n");
    } else {
        serial_io_printf("Symbol 'greet' not found  [SKIP]\n");
    }
 
    /* --- Test 3: array summation (exercises stack + local data) --- */
    fn_sum_array_t sum_fn = (fn_sum_array_t)so_get_symbol(&mod, "sum_array");
    if (sum_fn) {
        static int arr[] = { 1, 2, 3, 4, 5 };
        int sum = sum_fn(arr, 5);
        serial_io_printf("sum_array({1..5}) = %d  %s\n", sum,
                         sum == 15 ? "[PASS]" : "[FAIL]");
    } else {
        serial_io_printf("Symbol 'sum_array' not found  [SKIP]\n");
    }
 
    so_unload(&mod);
    serial_io_printf("=== SO loader test complete ===\n\n");
}