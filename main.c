#include <stdio.h>
#include <stdlib.h>
#include "product.h"
#include "order.h"
#include "customer.h"
#include "admin.h"

static void flushInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(void) {
    Product catalog[MAX_PRODUCTS];
    int productCount = loadProducts("data/products.csv", catalog);
    if (productCount == 0) {
        fprintf(stderr, "No products loaded. Check data/products.csv.\n");
        return 1;
    }

    Queue q1, q2;
    initQueue(&q1);
    initQueue(&q2);

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
    printf("\nPress Enter to close...\n");
    getchar();
    return 0;
}
