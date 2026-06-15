#include "utils.h"
#include <stdio.h>

void flushInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
