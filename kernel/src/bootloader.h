#pragma once
#include "limine.h"



// Declare Limine requests as extern (no definitions here)

extern volatile struct limine_framebuffer_request framebuffer_request;
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;
extern volatile struct limine_mp_request smp_request;

void minimal_bash();
