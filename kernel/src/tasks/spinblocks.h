#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>

typedef struct {
    volatile uint8_t locked;
} spinlock_t;

static inline void spinlock_init(spinlock_t *lock) {
    lock->locked = 0;
}

// Save CPU flags and disable interrupts
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

// Restore CPU flags
static inline void irq_restore(uint64_t flags) {
    __asm__ volatile(
        "push %0\n\t"
        "popfq"
        :
        : "r"(flags)
        : "memory", "cc"
    );
}

// Spin until we acquire the lock
static inline void spin_lock(spinlock_t *lock) {
    while (__atomic_test_and_set(&lock->locked, __ATOMIC_ACQUIRE)) {
        __asm__ volatile("pause");
    }
}

// Release the lock
static inline void spin_unlock(spinlock_t *lock) {
    __atomic_clear(&lock->locked, __ATOMIC_RELEASE);
}

// Spinlock with IRQ save/restore
static inline void spin_lock_irqsave(spinlock_t *lock, uint64_t *flags) {
    *flags = irq_save();
    spin_lock(lock);
}

static inline void spin_unlock_irqrestore(spinlock_t *lock, uint64_t *flags) {
    spin_unlock(lock);
    irq_restore(*flags);
}

#endif