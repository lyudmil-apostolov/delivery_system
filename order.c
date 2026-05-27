#include "order.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Internal counter for unique IDs ───────────────────────────────── */
static int orderCounter = 0;

int nextOrderId(void) {
    return ++orderCounter;
}

/* ── Queue operations ───────────────────────────────────────────────── */

void initQueue(Queue* q) {
    q->front = NULL;
    q->rear  = NULL;
    q->size  = 0;
}

int isEmpty(const Queue* q) {
    return (q->front == NULL);
}

void enqueue(Queue* q, Order* order) {
    order->next = NULL;
    if (q->rear == NULL) {
        /* queue was empty */
        q->front = order;
        q->rear  = order;
    } else {
        q->rear->next = order;
        q->rear       = order;
    }
    q->size++;
}

Order* dequeue(Queue* q) {
    if (isEmpty(q)) {
        return NULL;
    }
    Order* removed = q->front;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;  /* queue is now empty */
    }
    removed->next = NULL;
    q->size--;
    return removed;
}

void printQueue(const Queue* q, const char* label) {
    printf("\n=== Queue: %s (%d order(s)) ===\n", label, q->size);
    if (isEmpty(q)) {
        printf("  (empty)\n");
        return;
    }
    Order* cur = q->front;
    int position = 1;
    while (cur != NULL) {
        printf("  [%d] Order #%d | Address: %s | Total: %.2f EUR\n",
               position++, cur->id, cur->address, cur->total);
        cur = cur->next;
    }
}
