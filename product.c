#include "product.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int loadProducts(const char* filename, Product catalog[]){
    FILE* file = fopen(filename, "r");
    if(file == NULL){
        printf("ERROR! Could not open file %s\n!", filename);
        return 0;
    }
    int count = 0;
    char line[200];
    char currentCategory[30] = "";

    while(fgets(line, sizeof(line), file)){
        line[strcspn(line, "\n")] = 0;

        if(strlen(line) == 0) continue;

        if(strchr(line, ',') == NULL){
            strcpy(currentCategory, line);
        } else {
            char name[50];
            float price;
            int stock;

            if (sscanf(line, "%49[^,], %f, %d", name, &price, &stock) == 3) {
                strcpy(catalog[count].name, name);
                strcpy(catalog[count].category, currentCategory);
                catalog[count].price = price;
                catalog[count].stock = stock;
                count++;
            }

        }
    }

    fclose(file);
    printf("Loaded %d products from file\n", count);
    return count;
}

void saveProducts(const char* filename, Product catalog[], int count){
    FILE* file = fopen(filename, "w");

    if (file == NULL) {
        printf("ERROR! Could not save to file %s\n", filename);
        return;
    }

    char lastCategory[30] = "";

    for(int i = 0; i < count; i++){
        if(strcmp(catalog[i].category, lastCategory) != 0){
            if(strlen(lastCategory) != 0){
                fprintf(file, "\n");
            }
            fprintf(file, "%s\n", catalog[i].category);
            strcpy(lastCategory, catalog[i].category);
        }
        fprintf(file, "%s, %.2f, %d\n", catalog[i].name, catalog[i].price, catalog[i].stock);

    }
    fclose(file);
    printf("Stock saved to file.\n");

}

void updateStock(Product catalog[], int index, int quantity){
    if(catalog[index].stock == -1){
        return; //unlimited stock therefore do nothing
    }
    catalog[index].stock -= quantity;
    if(catalog[index].stock < 0){
        catalog[index].stock = 0;
    }
}

void displayProducts(Product catalog[], int count) {
    char lastCategory[30] = "";

    for (int i = 0; i < count; i++) {
        if (strcmp(catalog[i].category, lastCategory) != 0) {
            printf("\n--- %s ---\n", catalog[i].category);
            strcpy(lastCategory, catalog[i].category);
        }

        if (catalog[i].stock == 0) {
            printf("  [UNAVAILABLE] %s - %.2f EUR\n", catalog[i].name, catalog[i].price);
        } else if (catalog[i].stock == -1) {
            printf("  %s - %.2f EUR (unlimited)\n", catalog[i].name, catalog[i].price);
        } else {
            printf("  %s - %.2f EUR (stock: %d)\n", catalog[i].name, catalog[i].price, catalog[i].stock);
        }
    }
    printf("\n");
}
