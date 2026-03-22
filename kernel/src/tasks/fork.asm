section .text
    global task_fork

    extern task_fork_impl

task_fork:
    ; [rsp]    = return address back into the caller (forkTestMain)
    ; rsp+8... = caller's stack frame
    ; No fucking  shitty ahh  prologue has run yet - this bullshit is a naked langra trampoline

    mov rdi, rsp            ; arg1 = live_rsp (caller rsp pointing at the fucking  return addr)
    mov rsi, [rsp]          ; arg2 = return_addr = [rsp]

    ; go to the fucking impl u motherfucker 
    jmp task_fork_impl