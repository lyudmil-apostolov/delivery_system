#ifndef UTILS_H
#define UTILS_H

/* utils.h – Shared utility helpers used by every module.
 *
 * Having a single definition eliminates the three identical
 * static copies that previously lived in main.c, customer.c,
 * and admin.c. */

/* Discard all characters in stdin up to and including the next
   newline (or EOF).  Call this after every scanf() to prevent
   stale input from corrupting the next read. */
void flushInput(void);

#endif /* UTILS_H */
