#include "admin.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ── Time helpers ───────────────────────────────────────────────────── */

/* get today's filename like 10-06-2026.txt */
static void getTodayFilename(char* result, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(result, size, "%d-%m-%Y.txt", t);
}

/* get current time as HH:MM:SS */
static void getCurrentTime(char* result, size_t size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(result, size, "%H:%M:%S", t);
}

/* ── Logging ────────────────────────────────────────────────────────── */

/* write every admin action to admin_log.txt with timestamp */
static void logAction(const char* msg) {
    FILE* log = fopen("logs/admin_log.txt", "a");
    if (log == NULL) return;

    char timeNow[20];
    char dateStr[30];
    getCurrentTime(timeNow, sizeof(timeNow));
    getTodayFilename(dateStr, sizeof(dateStr));
    dateStr[strlen(dateStr) - 4] = '\0'; /* strip ".txt" */

    fprintf(log, "[%s %s] %s\n", dateStr, timeNow, msg);
    fclose(log);
}

/* ── History / report helpers ───────────────────────────────────────── */

/* save delivered order to history/DD-MM-YYYY.txt */
static void saveToHistory(const Order* o) {
    char fname[30];
    getTodayFilename(fname, sizeof(fname));
    char fullPath[60];
    snprintf(fullPath, sizeof(fullPath), "history/%s", fname);
    FILE* f = fopen(fullPath, "a");
    if (f == NULL) {
        printf("  [Warning] Could not open history file.\n");
        return;
    }
    fprintf(f, "Order #%d | Address: %s | Total: %.2f EUR\n",
            o->id, o->address, o->total);
    fclose(f);
    printf("  [Saved] Order #%d written to %s\n", o->id, fname);
}

/* ── Delivery processing ────────────────────────────────────────────── */

/* dequeue one order, record it, and free it */
static void deliverOne(Queue* q, const char* name,
                        int* count, float* revenue) {
    Order* o = dequeue(q);
    if (o == NULL) {
        printf("  %s queue is empty.\n", name);
        return;
    }

    printf("\n  Delivered: Order #%d\n", o->id);
    printf("    Priority : %s\n", o->priority == 1 ? "HIGH" : "Normal");
    printf("    Address  : %s\n", o->address);
    printf("    Total    : %.2f EUR\n", o->total);

    (*count)++;
    (*revenue) += o->total;

    saveToHistory(o);

    char entry[150];
    snprintf(entry, sizeof(entry), "Delivered Order #%d | %s | %.2f EUR",
             o->id, o->address, o->total);
    logAction(entry);

    free(o);
}

/* process 2 from high priority then 1 from normal */
static void processDeliveries(Queue* q1, Queue* q2,
                               int* count, float* revenue) {
    if (isEmpty(q1) && isEmpty(q2)) {
        printf("\n  Both queues are empty.\n");
        return;
    }

    printf("\n--- Processing Deliveries ---\n");

    if (isEmpty(q1)) {
        printf("  High priority queue empty. Taking from normal...\n");
        deliverOne(q2, "Normal", count, revenue);
    } else if (isEmpty(q2)) {
        printf("  Normal queue empty. Taking from high priority...\n");
        deliverOne(q1, "High", count, revenue);
    } else {
        /* standard rule: 2 high priority, then 1 normal */
        printf("  Step 1 - High priority:\n");
        deliverOne(q1, "High", count, revenue);

        if (!isEmpty(q1)) {
            printf("  Step 2 - High priority:\n");
            deliverOne(q1, "High", count, revenue);
        } else {
            printf("  High priority queue now empty, skipping step 2.\n");
        }

        printf("  Step 3 - Normal priority:\n");
        deliverOne(q2, "Normal", count, revenue);
    }

    logAction("Processed delivery cycle");
}

/* ── Statistics ─────────────────────────────────────────────────────── */

static void showStats(int count, float revenue,
                       const Queue* q1, const Queue* q2) {
    printf("\n===== Statistics =====\n");
    printf("  Deliveries done  : %d\n", count);
    printf("  Total revenue    : %.2f EUR\n", revenue);

    if (count > 0)
        printf("  Average per order: %.2f EUR\n", revenue / (float)count);
    else
        printf("  Average per order: N/A\n");

    printf("  Waiting (High)   : %d\n", q1->size);
    printf("  Waiting (Normal) : %d\n", q2->size);
    printf("======================\n");

    logAction("Viewed statistics");
}

/* ── History / log viewers ──────────────────────────────────────────── */

/* print today's history file to screen */
static void viewHistory(void) {
    char fname[30];
    getTodayFilename(fname, sizeof(fname));

    char fullPath[60];
    snprintf(fullPath, sizeof(fullPath), "history/%s", fname);
    FILE* f = fopen(fullPath, "r");
    if (f == NULL) {
        printf("\n  No history found for today (%s).\n", fullPath);
        return;
    }

    printf("\n=== History: %s ===\n", fname);
    char line[256];
    int i = 1;
    while (fgets(line, sizeof(line), f)) {
        printf("  %d. %s", i++, line);
    }
    if (i == 1) printf("  (empty)\n");
    fclose(f);

    logAction("Viewed history file");
}

/* export a summary report file for today */
static void exportReport(int count, float revenue) {
    char dateStr[20];
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(dateStr, sizeof(dateStr), "%d-%m-%Y", t);

    char reportName[40];
    snprintf(reportName, sizeof(reportName), "reports/report_%s.txt", dateStr);

    char histName[30];
    getTodayFilename(histName, sizeof(histName));

    char histPath[60];
    snprintf(histPath, sizeof(histPath), "history/%s", histName);
    FILE* hist = fopen(histPath, "r");
    FILE* rep  = fopen(reportName, "w");

    if (rep == NULL) {
        printf("  Could not create report file.\n");
        if (hist) fclose(hist);
        return;
    }

    char timeNow[20];
    getCurrentTime(timeNow, sizeof(timeNow));

    fprintf(rep, "========================================\n");
    fprintf(rep, "  E-Commerce Delivery - Daily Report\n");
    fprintf(rep, "  Date      : %s\n", dateStr);
    fprintf(rep, "  Generated : %s\n", timeNow);
    fprintf(rep, "========================================\n\n");

    if (hist == NULL) {
        fprintf(rep, "  No delivery history file found for today.\n\n");
    } else {
        fprintf(rep, "--- Orders Delivered ---\n");
        char line[256];
        int i = 0;
        while (fgets(line, sizeof(line), hist)) {
            fprintf(rep, "  %s", line);
            i++;
        }
        if (i == 0) fprintf(rep, "  (none)\n");
        fclose(hist);
        fprintf(rep, "\n");
    }

    fprintf(rep, "--- Summary ---\n");
    fprintf(rep, "  Total deliveries : %d\n", count);
    fprintf(rep, "  Total revenue    : %.2f EUR\n", revenue);
    if (count > 0)
        fprintf(rep, "  Average order    : %.2f EUR\n", revenue / (float)count);
    fprintf(rep, "\n========================================\n");
    fprintf(rep, "  End of Report\n");
    fprintf(rep, "========================================\n");
    fclose(rep);

    printf("\n  Report saved to: %s\n", reportName);
    logAction("Exported daily report");
}

/* show the admin activity log on screen */
static void viewLog(void) {
    FILE* f = fopen("logs/admin_log.txt", "r");
    if (f == NULL) {
        printf("\n  No log file found yet.\n");
        return;
    }
    printf("\n=== Admin Log ===\n");
    char line[256];
    int i = 1;
    while (fgets(line, sizeof(line), f)) {
        printf("  %d. %s", i++, line);
    }
    if (i == 1) printf("  (empty)\n");
    fclose(f);
}

/* ── Order cancellation (with stock restoration) ────────────────────── */

/* Search for order ID in a queue; if found, restore its stock to the
   catalogue and free it.  Returns 1 if found, 0 otherwise. */
static int removeFromQueue(Queue* q, int id, const char* name,
                            Product catalog[]) {
    if (isEmpty(q)) return 0;

    /* Drain into a temp queue, free only the matching node */
    Queue temp;
    temp.front = NULL;
    temp.rear  = NULL;
    temp.size  = 0;

    int found = 0;
    while (!isEmpty(q)) {
        Order* o = dequeue(q);
        if (o->id == id && !found) {
            printf("\n  Cancelled: Order #%d from %s queue\n", o->id, name);
            printf("    Address : %s\n", o->address);
            printf("    Total   : %.2f EUR\n", o->total);

            /* Restore stock for every item in the cancelled order */
            for (int i = 0; i < o->itemCount; i++) {
                int pi = o->items[i].productIndex;
                if (catalog[pi].stock != -1) {      /* skip unlimited items */
                    catalog[pi].stock += o->items[i].quantity;
                }
            }
            printf("    Stock restored for %d item type(s).\n", o->itemCount);

            free(o);
            found = 1;
        } else {
            enqueue(&temp, o);
        }
    }

    /* Put all remaining orders back */
    while (!isEmpty(&temp)) {
        Order* o = dequeue(&temp);
        enqueue(q, o);
    }

    return found;
}

static void cancelOrder(Queue* q1, Queue* q2, Product catalog[]) {
    printQueue(q1, "High Priority");
    printQueue(q2, "Normal Priority");

    if (isEmpty(q1) && isEmpty(q2)) {
        printf("\n  No pending orders to cancel.\n");
        return;
    }

    int id;
    printf("\nEnter Order ID to cancel: ");
    if (scanf("%d", &id) != 1) {
        flushInput();
        printf("  Invalid input.\n");
        return;
    }
    flushInput();

    int found = removeFromQueue(q1, id, "High",   catalog);
    if (!found)
        found  = removeFromQueue(q2, id, "Normal", catalog);

    if (!found) {
        printf("  Order #%d not found.\n", id);
    } else {
        char entry[80];
        snprintf(entry, sizeof(entry), "Cancelled Order #%d", id);
        logAction(entry);
    }
}

/* ── Product management ─────────────────────────────────────────────── */

/* Increase the stock of an existing product */
static void restockProduct(Product catalog[], int productCount) {
    if (productCount == 0) {
        printf("  No products in catalogue.\n");
        return;
    }
    displayProducts(catalog, productCount);

    int num;
    printf("Enter product number to restock (1-%d): ", productCount);
    if (scanf("%d", &num) != 1) { flushInput(); return; }
    flushInput();

    if (num < 1 || num > productCount) {
        printf("  Invalid product number.\n");
        return;
    }

    int pi = num - 1;
    if (catalog[pi].stock == -1) {
        printf("  \"%s\" has unlimited stock — no restock needed.\n",
               catalog[pi].name);
        return;
    }

    printf("  Current stock of \"%s\": %d\n",
           catalog[pi].name, catalog[pi].stock);
    printf("  Quantity to add: ");
    int qty;
    if (scanf("%d", &qty) != 1 || qty <= 0) {
        printf("  Invalid quantity.\n");
        flushInput();
        return;
    }
    flushInput();

    catalog[pi].stock += qty;
    printf("  \"%s\" restocked. New stock: %d\n",
           catalog[pi].name, catalog[pi].stock);

    char entry[120];
    snprintf(entry, sizeof(entry),
             "Restocked \"%s\" +%d (total: %d)",
             catalog[pi].name, qty, catalog[pi].stock);
    logAction(entry);
}

/* Add a brand-new product to the in-memory catalogue */
static void addProduct(Product catalog[], int* productCount) {
    if (*productCount >= MAX_PRODUCTS) {
        printf("  Catalogue is full (max %d products).\n", MAX_PRODUCTS);
        return;
    }

    Product p;

    printf("  Product name   : ");
    if (fgets(p.name, sizeof(p.name), stdin) == NULL) return;
    p.name[strcspn(p.name, "\n")] = '\0';
    if (strlen(p.name) == 0) {
        printf("  Name cannot be empty.\n");
        return;
    }

    printf("  Category       : ");
    if (fgets(p.category, sizeof(p.category), stdin) == NULL) return;
    p.category[strcspn(p.category, "\n")] = '\0';
    if (strlen(p.category) == 0) {
        printf("  Category cannot be empty.\n");
        return;
    }

    printf("  Price (EUR)    : ");
    if (scanf("%f", &p.price) != 1 || p.price < 0.0f) {
        printf("  Invalid price.\n");
        flushInput();
        return;
    }
    flushInput();

    printf("  Initial stock (-1 = unlimited): ");
    if (scanf("%d", &p.stock) != 1 || p.stock < -1) {
        printf("  Invalid stock value.\n");
        flushInput();
        return;
    }
    flushInput();

    catalog[*productCount] = p;
    (*productCount)++;

    printf("  Product \"%s\" added (catalogue now has %d items).\n",
           p.name, *productCount);

    char entry[120];
    snprintf(entry, sizeof(entry),
             "Added product \"%s\" (%.2f EUR, stock: %d)",
             p.name, p.price, p.stock);
    logAction(entry);
}

/* Mark a product as discontinued (sets stock to 0) */
static void discontinueProduct(Product catalog[], int productCount) {
    if (productCount == 0) {
        printf("  No products in catalogue.\n");
        return;
    }
    displayProducts(catalog, productCount);

    int num;
    printf("Enter product number to discontinue (1-%d): ", productCount);
    if (scanf("%d", &num) != 1) { flushInput(); return; }
    flushInput();

    if (num < 1 || num > productCount) {
        printf("  Invalid product number.\n");
        return;
    }

    int pi = num - 1;
    char ch;
    printf("  Discontinue \"%s\"? (y/n): ", catalog[pi].name);
    if (scanf(" %c", &ch) != 1) { flushInput(); return; }
    flushInput();

    if (ch != 'y' && ch != 'Y') {
        printf("  Cancelled.\n");
        return;
    }

    catalog[pi].stock = 0;
    printf("  \"%s\" is now discontinued (stock set to 0).\n",
           catalog[pi].name);

    char entry[100];
    snprintf(entry, sizeof(entry),
             "Discontinued product \"%s\"", catalog[pi].name);
    logAction(entry);
}

/* ── Authentication ─────────────────────────────────────────────────── */

static int login(void) {
    printf("\n=== Admin Login ===\n");

    for (int i = 1; i <= MAX_LOGIN_TRIES; i++) {
        char input[50];
        printf("  Password (%d/%d): ", i, MAX_LOGIN_TRIES);
        if (fgets(input, sizeof(input), stdin) == NULL) continue;
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, ADMIN_PASSWORD) == 0) {
            printf("  Access granted.\n");
            logAction("Admin logged in");
            return 1;
        }
        printf("  Wrong password.\n");
    }

    printf("\n  Too many attempts. Access denied.\n");
    logAction("Login failed - access denied");
    return 0;
}

/* ── Main admin entry point ─────────────────────────────────────────── */

void runAdminMode(Queue* q1, Queue* q2,
                  Product catalog[], int* productCount) {
    if (!login()) return;

    static int   totalDone = 0;
    static float totalRev  = 0.0f;

    int choice;
    do {
        printf("\n=== Administrator Menu ===\n");
        printf("  High priority pending  : %d\n", q1->size);
        printf("  Normal priority pending: %d\n", q2->size);
        printf("\n");
        printf("  1.  Process deliveries\n");
        printf("  2.  View queues\n");
        printf("  3.  View statistics\n");
        printf("  4.  View today's history\n");
        printf("  5.  Cancel an order\n");
        printf("  6.  Export daily report\n");
        printf("  7.  View admin log\n");
        printf("  8.  Restock a product\n");
        printf("  9.  Add a new product\n");
        printf("  10. Discontinue a product\n");
        printf("  0.  Back to main menu\n");
        printf("Your choice: ");

        if (scanf("%d", &choice) != 1) {
            flushInput();
            choice = -1;
            continue;
        }
        flushInput();

        switch (choice) {
            case 1:  processDeliveries(q1, q2, &totalDone, &totalRev);  break;
            case 2:
                printQueue(q1, "High Priority");
                printQueue(q2, "Normal Priority");
                logAction("Viewed queues");
                break;
            case 3:  showStats(totalDone, totalRev, q1, q2);            break;
            case 4:  viewHistory();                                       break;
            case 5:  cancelOrder(q1, q2, catalog);                       break;
            case 6:  exportReport(totalDone, totalRev);                  break;
            case 7:  viewLog();                                           break;
            case 8:  restockProduct(catalog, *productCount);             break;
            case 9:  addProduct(catalog, productCount);                  break;
            case 10: discontinueProduct(catalog, *productCount);         break;
            case 0:
                printf("Returning to main menu...\n");
                logAction("Admin logged out");
                break;
            default:
                printf("Invalid option.\n");
        }
    } while (choice != 0);
}
