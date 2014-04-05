#ifndef __TEST__H__
#define __TEST__H__

#define NP_CHECK(ptr) \
    { \
        if (NULL == (ptr)) { \
            fprintf(stderr, "%s:%d NULL POINTER: %s n", \
                __FILE__, __LINE__, #ptr); \
            exit(-1); \
        } \
    } \

/* Test structure */
typedef struct test_s {
    int priority;
} Test;

Test *test_new(int priority);

void test_delete(Test *t);

int test_compare(const void *d1, const void *d2);

#endif