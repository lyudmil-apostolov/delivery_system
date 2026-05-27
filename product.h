#ifndef PRODUCT_H
#define PRODUCT_H
#define MAX_PRODUCTS 100


typedef struct {
    char name[50];
    char category[30];
    float price;
    int stock;

} Product;

int loadProducts(const char* filename, Product catalog[]);
void saveProducts(const char* filename, Product catalog[], int count);
void updateStock(Product catalog[], int index, int quantity);
void displayProducts(Product catalog[], int count);


#endif
