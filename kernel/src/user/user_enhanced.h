#pragma once
#include <stdint.h>
#include <stdbool.h>

void user_init(void);
int user_create_task(const char* name, void (*func)(void));
void user_list_tasks(void);
bool user_run_task(const char* name);

// Built-in tasks
void task_counter(void);
void task_hello(void);
void task_stress(void);
