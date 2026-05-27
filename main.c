#include <stdio.h>
#include <stdlib.h>
#include "product.h"
#include "order.h"
#include "customer.h"

/* ── Minimal flush helper ───────────────────────────────────────────── */
static void flushInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ── Stub: Administrator mode (section 2.4 – to be completed) ───────── */
static void runAdminMode(Queue* q1, Queue* q2) {
    printf("\n=== Administrator Mode ===\n");
    printf("  (Admin functionality not yet implemented – see section 2.4)\n");
    printf("  Current queue sizes: Priority-1=%d, Priority-2=%d\n",
           q1->size, q2->size);
    printQueue(q1, "Priority-1 (High)");
    printQueue(q2, "Priority-2 (Normal)");
}

/* ── Main ───────────────────────────────────────────────────────────── */
int main(void) {
    /* ── 2.1 / 2.2: Load product catalog ─────────────────────────── */
    Product catalog[MAX_PRODUCTS];
    int productCount = loadProducts("data/products.csv", catalog);
    if (productCount == 0) {
        fprintf(stderr, "No products loaded. Check data/products.csv.\n");
        return 1;
    }

    /* ── 2.5: Initialise the two priority queues ──────────────────── */
    Queue q1, q2;
    initQueue(&q1);
    initQueue(&q2);

    /* ── Main menu loop ───────────────────────────────────────────── */
    int choice;
    do {
        printf("\n========================================\n");
        printf("  E-Commerce Delivery Management System\n");
        printf("========================================\n");
        printf("  1. Customer mode\n");
        printf("  2. Administrator mode\n");
        printf("  3. Exit\n");
        printf("Your choice: ");

        if (scanf("%d", &choice) != 1) {
            flushInput();
            choice = -1;
        } else {
            flushInput();
        }

        switch (choice) {
            case 1:
                runCustomerMode(catalog, productCount, &q1, &q2);
                break;
            case 2:
                runAdminMode(&q1, &q2);
                break;
            case 3:
                printf("\nSaving stock to file...\n");
                saveProducts("data/products.csv", catalog, productCount);
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please enter 1, 2, or 3.\n");
        }
    } while (choice != 3);

    return 0;
}
