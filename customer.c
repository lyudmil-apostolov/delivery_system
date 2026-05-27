#include "customer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Helper: flush stdin after scanf ────────────────────────────────── */
static void flushInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* ── Helper: collect all unique category names ──────────────────────── */
static int getCategories(Product catalog[], int productCount,
                          char categories[][30]) {
    int catCount = 0;
    for (int i = 0; i < productCount; i++) {
        int found = 0;
        for (int j = 0; j < catCount; j++) {
            if (strcmp(catalog[i].category, categories[j]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(categories[catCount++], catalog[i].category);
        }
    }
    return catCount;
}

/* ── Display available categories ───────────────────────────────────── */
static void displayCategories(char categories[][30], int catCount) {
    printf("\n=== Product Categories ===\n");
    for (int i = 0; i < catCount; i++) {
        printf("  %d. %s\n", i + 1, categories[i]);
    }
    printf("  0. Back\n");
}

/* ── Display products for a given category ──────────────────────────── */
static void displayCategory(Product catalog[], int productCount,
                             const char* category) {
    printf("\n=== %s ===\n", category);
    int found = 0;
    for (int i = 0; i < productCount; i++) {
        if (strcmp(catalog[i].category, category) == 0) {
            if (catalog[i].stock == 0) {
                printf("  [%-3d] %-30s  %.2f EUR  [UNAVAILABLE]\n",
                       i + 1, catalog[i].name, catalog[i].price);
            } else if (catalog[i].stock == -1) {
                printf("  [%-3d] %-30s  %.2f EUR  (unlimited)\n",
                       i + 1, catalog[i].name, catalog[i].price);
            } else {
                printf("  [%-3d] %-30s  %.2f EUR  (stock: %d)\n",
                       i + 1, catalog[i].name, catalog[i].price,
                       catalog[i].stock);
            }
            found = 1;
        }
    }
    if (!found) {
        printf("  (no products in this category)\n");
    }
}

/* ── Display the current cart ───────────────────────────────────────── */
static void displayCart(CartItem cart[], int cartSize,
                         Product catalog[]) {
    printf("\n=== Your Cart ===\n");
    if (cartSize == 0) {
        printf("  (empty)\n");
        return;
    }
    float total = 0.0f;
    for (int i = 0; i < cartSize; i++) {
        int pi = cart[i].productIndex;
        float lineTotal = catalog[pi].price * cart[i].quantity;
        printf("  %d. %-30s  x%d  = %.2f EUR\n",
               i + 1, catalog[pi].name, cart[i].quantity, lineTotal);
        total += lineTotal;
    }
    printf("  --------------------------------\n");
    printf("  TOTAL: %.2f EUR\n", total);
}

/* ── Add a product to the cart ──────────────────────────────────────── */
static void addToCart(CartItem cart[], int* cartSize,
                       Product catalog[], int productCount) {
    int productNum;
    printf("\nEnter product number to add (1-%d): ", productCount);
    if (scanf("%d", &productNum) != 1) { flushInput(); return; }
    flushInput();

    if (productNum < 1 || productNum > productCount) {
        printf("Invalid product number.\n");
        return;
    }

    int pi = productNum - 1; /* convert to 0-based index */

    if (catalog[pi].stock == 0) {
        printf("Sorry, \"%s\" is unavailable.\n", catalog[pi].name);
        return;
    }

    int qty;
    printf("Enter quantity: ");
    if (scanf("%d", &qty) != 1 || qty <= 0) {
        printf("Invalid quantity.\n");
        flushInput();
        return;
    }
    flushInput();

    /* Check against stock (unless unlimited) */
    if (catalog[pi].stock != -1) {
        /* Find already-in-cart quantity for this product */
        int alreadyInCart = 0;
        for (int i = 0; i < *cartSize; i++) {
            if (cart[i].productIndex == pi) {
                alreadyInCart = cart[i].quantity;
                break;
            }
        }
        if (alreadyInCart + qty > catalog[pi].stock) {
            printf("Not enough stock. Available: %d (already in cart: %d).\n",
                   catalog[pi].stock, alreadyInCart);
            return;
        }
    }

    /* Merge with existing cart entry for the same product, if any */
    for (int i = 0; i < *cartSize; i++) {
        if (cart[i].productIndex == pi) {
            cart[i].quantity += qty;
            printf("Updated cart: \"%s\" x%d\n",
                   catalog[pi].name, cart[i].quantity);
            return;
        }
    }

    /* New cart entry */
    if (*cartSize >= MAX_CART_ITEMS) {
        printf("Cart is full (max %d items).\n", MAX_CART_ITEMS);
        return;
    }
    cart[*cartSize].productIndex = pi;
    cart[*cartSize].quantity     = qty;
    (*cartSize)++;
    printf("Added \"%s\" x%d to cart.\n", catalog[pi].name, qty);
}

/* ── Remove an item from the cart ───────────────────────────────────── */
static void removeFromCart(CartItem cart[], int* cartSize) {
    if (*cartSize == 0) {
        printf("Cart is empty.\n");
        return;
    }
    int choice;
    printf("Enter cart item number to remove (1-%d): ", *cartSize);
    if (scanf("%d", &choice) != 1) { flushInput(); return; }
    flushInput();

    if (choice < 1 || choice > *cartSize) {
        printf("Invalid selection.\n");
        return;
    }
    /* Shift remaining items left */
    for (int i = choice - 1; i < *cartSize - 1; i++) {
        cart[i] = cart[i + 1];
    }
    (*cartSize)--;
    printf("Item removed from cart.\n");
}

/* ── Compute cart total ─────────────────────────────────────────────── */
static float cartTotal(CartItem cart[], int cartSize, Product catalog[]) {
    float total = 0.0f;
    for (int i = 0; i < cartSize; i++) {
        total += catalog[cart[i].productIndex].price * cart[i].quantity;
    }
    return total;
}

/* ── Validate the order and enqueue it ──────────────────────────────── */
static void validateOrder(CartItem cart[], int cartSize,
                            Product catalog[], Queue* q1, Queue* q2) {
    if (cartSize == 0) {
        printf("Cannot place an order: your cart is empty.\n");
        return;
    }

    /* Choose priority */
    int priority;
    printf("\nChoose delivery priority:\n");
    printf("  1. High priority\n");
    printf("  2. Normal priority\n");
    printf("Your choice: ");
    if (scanf("%d", &priority) != 1 || (priority != 1 && priority != 2)) {
        printf("Invalid priority. Defaulting to normal (2).\n");
        priority = 2;
    }
    flushInput();

    /* Enter delivery address */
    char address[100];
    printf("Enter delivery address: ");
    if (fgets(address, sizeof(address), stdin) == NULL) {
        printf("Error reading address.\n");
        return;
    }
    address[strcspn(address, "\n")] = '\0'; /* strip newline */

    if (strlen(address) == 0) {
        printf("Address cannot be empty.\n");
        return;
    }

    /* Build the Order */
    Order* order = (Order*)malloc(sizeof(Order));
    if (order == NULL) {
        printf("Memory allocation error.\n");
        return;
    }
    order->id       = nextOrderId();
    order->priority = priority;
    order->total    = cartTotal(cart, cartSize, catalog);
    strncpy(order->address, address, sizeof(order->address) - 1);
    order->address[sizeof(order->address) - 1] = '\0';
    order->itemCount = cartSize;
    for (int i = 0; i < cartSize; i++) {
        order->items[i] = cart[i];
    }
    order->next = NULL;

    /* Deduct stock from the in-memory catalog */
    for (int i = 0; i < cartSize; i++) {
        int pi = cart[i].productIndex;
        if (catalog[pi].stock != -1) {
            catalog[pi].stock -= cart[i].quantity;
            if (catalog[pi].stock < 0) catalog[pi].stock = 0;
        }
    }

    /* Enqueue into the correct priority queue */
    if (priority == 1) {
        enqueue(q1, order);
    } else {
        enqueue(q2, order);
    }

    printf("\n✓ Order #%d placed successfully!\n", order->id);
    printf("  Priority  : %s\n", priority == 1 ? "HIGH" : "Normal");
    printf("  Address   : %s\n", order->address);
    printf("  Total     : %.2f EUR\n", order->total);
}

/* ── Cart management sub-menu ───────────────────────────────────────── */
static void cartMenu(CartItem cart[], int* cartSize,
                      Product catalog[], int productCount,
                      Queue* q1, Queue* q2) {
    int choice;
    do {
        printf("\n=== Cart Menu ===\n");
        printf("  1. View cart\n");
        printf("  2. Add a product\n");
        printf("  3. Remove an item\n");
        printf("  4. Validate order\n");
        printf("  0. Back to main customer menu\n");
        printf("Your choice: ");

        if (scanf("%d", &choice) != 1) {
            flushInput();
            choice = -1;
            continue;
        }
        flushInput();

        switch (choice) {
            case 1:
                displayCart(cart, *cartSize, catalog);
                break;
            case 2:
                addToCart(cart, cartSize, catalog, productCount);
                break;
            case 3:
                removeFromCart(cart, cartSize);
                break;
            case 4:
                validateOrder(cart, *cartSize, catalog, q1, q2);
                if (*cartSize > 0) {
                    /* Cart is cleared after successful order */
                    *cartSize = 0;
                    printf("Cart cleared.\n");
                }
                break;
            case 0:
                break;
            default:
                printf("Invalid option. Please try again.\n");
        }
    } while (choice != 0);
}

/* ── Browse category sub-menu ───────────────────────────────────────── */
static void browseCategoryMenu(Product catalog[], int productCount,
                                 const char* category,
                                 CartItem cart[], int* cartSize,
                                 Queue* q1, Queue* q2) {
    int choice;
    do {
        displayCategory(catalog, productCount, category);
        printf("\n  1. Add a product to cart\n");
        printf("  2. View cart\n");
        printf("  3. Go to cart menu\n");
        printf("  0. Back to categories\n");
        printf("Your choice: ");

        if (scanf("%d", &choice) != 1) {
            flushInput();
            choice = -1;
            continue;
        }
        flushInput();

        switch (choice) {
            case 1:
                addToCart(cart, cartSize, catalog, productCount);
                break;
            case 2:
                displayCart(cart, *cartSize, catalog);
                break;
            case 3:
                cartMenu(cart, cartSize, catalog, productCount, q1, q2);
                break;
            case 0:
                break;
            default:
                printf("Invalid option.\n");
        }
    } while (choice != 0);
}

/* ── Main customer entry point ──────────────────────────────────────── */
void runCustomerMode(Product catalog[], int productCount,
                     Queue* q1, Queue* q2) {
    CartItem cart[MAX_CART_ITEMS];
    int cartSize = 0;

    char categories[MAX_PRODUCTS][30];
    int catCount = getCategories(catalog, productCount, categories);

    int choice;
    do {
        printf("\n=== Customer Mode ===\n");
        printf("  1. Browse products by category\n");
        printf("  2. View cart\n");
        printf("  3. Go to cart menu (add / remove / validate)\n");
        printf("  0. Back to main menu\n");
        printf("Your choice: ");

        if (scanf("%d", &choice) != 1) {
            flushInput();
            choice = -1;
            continue;
        }
        flushInput();

        switch (choice) {
            case 1: {
                int catChoice;
                do {
                    displayCategories(categories, catCount);
                    printf("Your choice: ");
                    if (scanf("%d", &catChoice) != 1) {
                        flushInput();
                        catChoice = -1;
                        continue;
                    }
                    flushInput();

                    if (catChoice == 0) break;
                    if (catChoice < 1 || catChoice > catCount) {
                        printf("Invalid choice.\n");
                        continue;
                    }
                    browseCategoryMenu(catalog, productCount,
                                       categories[catChoice - 1],
                                       cart, &cartSize, q1, q2);
                } while (catChoice != 0);
                break;
            }
            case 2:
                displayCart(cart, cartSize, catalog);
                break;
            case 3:
                cartMenu(cart, &cartSize, catalog, productCount, q1, q2);
                break;
            case 0:
                break;
            default:
                printf("Invalid option. Please try again.\n");
        }
    } while (choice != 0);
}
