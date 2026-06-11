#ifndef ADMIN_H
#define ADMIN_H

#include "order.h"

/* admin.h  –  Administrator module interface
 *
 * Delivery processing + statistics
 * File history + report export + session log
 * CLI menu with password protection */

/* ── Admin password ───────────────────────── */
#define ADMIN_PASSWORD  "utt"
#define MAX_LOGIN_TRIES  3

/* Prompts for password, then shows the admin menu */
void runAdminMode(Queue* q1, Queue* q2);

#endif /* ADMIN_H */
