#include <stdlib.h>
#include <stdio.h>

#include "test.h"

Test *test_new(int priority) {
    Test *t = NULL;
    t = malloc(sizeof(*t));
    NP_CHECK(t);
    t->priority = priority;
    return (t);
}

void test_delete(Test *t) {
    if (NULL != t) {
        free(t);
    }
}

/* Used two compare two elements */
int test_compare(const void *d1, const void *d2) {
    return ((Test*)d1)->priority - ((Test*)d2)->priority;
}