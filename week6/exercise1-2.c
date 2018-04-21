#include <stdlib.h>
#define N     409600000
#define M        100000
#define PAGESIZE 4096

int main(int argc, char **argv) {

    int *x = malloc(N * sizeof(int));
    if (!x) return 1;

    for (int i = 0; i < M; i++) x[i * PAGESIZE] = 0;

    return 0;
}