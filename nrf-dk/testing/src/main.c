#include "main.h"

int main() {
    printDebug("Starting main operation.");
    performMainOperation();
    return 0;
}

void performMainOperation() {
    int result = computeSum(5, 3);
    printf("Result: %d\n", result);
}
