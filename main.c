#include <stdio.h>
#include "product.h"

int main()
{
    Product catalog[MAX_PRODUCTS];
    int productCount = 0;

    productCount = loadProducts("data/products.csv", catalog);

    displayProducts(catalog, productCount);
    printf("Updating stock of product 0 (Laptop_1) by -2...\n");



    saveProducts("data/products.csv", catalog, productCount);
    return 0;
}
