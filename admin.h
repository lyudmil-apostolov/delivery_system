#ifndef ADMIN_H
#define ADMIN_H

#include "order.h"
#include "product.h"

/* admin.h  –  Administrator module interface
 *
 * Delivery processing + statistics
 * File history + report export + session log
 * Product management (restock, add, discontinue)
 * CLI menu with password protection */

/* ── Admin password ───────────────────────── */
#define ADMIN_PASSWORD  "utt"
#define MAX_LOGIN_TRIES  3

/* Prompts for password, then shows the admin menu.
 * catalog / productCount are passed in so that:
 *   - stock is restored when an order is cancelled
 *   - admin can restock, add, or discontinue products */
void runAdminMode(Queue* q1, Queue* q2,
                  Product catalog[], int* productCount);

#endif /* ADMIN_H */
