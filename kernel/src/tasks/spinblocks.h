#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>
#include <stdatomic.h>

typedef struct {
    atomic_flag flag;
} spinlock_t;

static inline void spinlock_init(spinlock_t *lock) {
    atomic_flag_clear_explicit(&lock->flag, memory_order_relaxed);
}

static inline uint64_t irq_save(void) {
    uint64_t flags;
    __asm__ volatile(
        "pushfq\n\t"
        "pop %0\n\t"
        "cli"
        : "=r"(flags)
        :
        : "memory"
    );
    return flags;
}

static inline void irq_restore(uint64_t flags) {
    __asm__ volatile(
        "push %0\n\t"
        "popfq"
        :
        : "r"(flags)
        : "memory", "cc"
    );
}

static inline void spin_lock(spinlock_t *lock) {
    while (atomic_flag_test_and_set_explicit(
               &lock->flag,
               memory_order_acquire)) {
        __asm__ volatile("pause");
    }
}

static inline void spin_unlock(spinlock_t *lock) {
    atomic_flag_clear_explicit(&lock->flag, memory_order_release);
}

static inline void spin_lock_irqsave(spinlock_t *lock, uint64_t *flags) {
    *flags = irq_save();
    spin_lock(lock);
}

static inline void spin_unlock_irqrestore(spinlock_t *lock, uint64_t *flags) {
    spin_unlock(lock);
    irq_restore(*flags);
}

#endif