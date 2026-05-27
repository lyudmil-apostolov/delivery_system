#ifndef ORDER_H
#define ORDER_H

#include "product.h"

/* ── Cart item ──────────────────────────────────────────────────────── */
typedef struct {
    int   productIndex;   /* index in the catalog array */
    int   quantity;
} CartItem;

#define MAX_CART_ITEMS 50

/* ── Order (node of the FIFO linked list) ───────────────────────────── */
typedef struct Order {
    int    id;
    char   address[100];
    float  total;
    int    priority;      /* 1 = high, 2 = normal */

    /* snapshot of items so the delivery log can record them */
    CartItem items[MAX_CART_ITEMS];
    int      itemCount;

    struct Order* next;
} Order;

/* ── Queue (FIFO linked list) ───────────────────────────────────────── */
typedef struct {
    Order* front;
    Order* rear;
    int    size;
} Queue;

/* ── Function prototypes ────────────────────────────────────────────── */

/* Initialise an empty queue */
void initQueue(Queue* q);

/* Returns 1 if the queue is empty, 0 otherwise */
int  isEmpty(const Queue* q);

/* Append a new order to the rear of the queue */
void enqueue(Queue* q, Order* order);

/* Remove and return the front order (caller must free it) */
Order* dequeue(Queue* q);

/* Print all orders currently in a queue (for debugging / admin view) */
void printQueue(const Queue* q, const char* label);

/* Generate the next unique, strictly-increasing order ID */
int nextOrderId(void);

#endif /* ORDER_H */
