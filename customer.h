#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "product.h"
#include "order.h"

/*
 * Run the full customer workflow:
 *   - browse categories / products
 *   - manage cart
 *   - choose priority + enter address
 *   - validate → enqueue into the appropriate priority queue
 *
 * Parameters:
 *   catalog      – the in-memory product array
 *   productCount – number of products loaded
 *   q1           – pointer to the priority-1 (high) queue
 *   q2           – pointer to the priority-2 (normal) queue
 */
void runCustomerMode(Product catalog[], int productCount,
                     Queue* q1, Queue* q2);

#endif /* CUSTOMER_H */
