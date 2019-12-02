#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef JEMALLOC
/* Using jemalloc for memery management. */
#include <jemalloc.h>
#define malloc(x) jemalloc(x)
#define realloc(p, x) jerealloc(p, x)
#define my_free(p) jefree(p)

#elif TCMALLOC
/* Using tcmalloc for memery management. */
#include <tcmalloc.h>
#define malloc(x) tcmalloc(x)
#define realloc(p, x) tcrealloc(p, x)
#define free(p) tcfree(p)

#else
/* use default for memery management. */
#endif

/* 这个联合体的目的是得到在当前系统中，最大类型所占的字节大小 */
union alignment {
    size_t sz;
    void *ptr;
    double dbl;
};
/* We need to make sure that everything we return is on the right
   alignment to hold anything, including a double. */
#define ALIGNMENT sizeof(union alignment)

/**
 * 在已分配内存的旁边额外开辟一块ALIGNMENT大小的空间，用以存储分配出去的大小
 * 该内存结构，以及分配与释放的过程如下：
 * 
 * 1.|*p = sz
 * 2.|OUTPTR: p + Aliment -> p'
 * 
 * p(size)    p'(real data ptr)
 * +----------+------------------------+
 * | Aligment |         data           |
 * +----------+------------------------+
 * 
 * 3.|INPTR: p' - Aligment -> p
 * 4.|sz = *p
 */
#define OUTPTR(ptr) (((char*)ptr)+ALIGNMENT)
#define INPTR(ptr) (((char*)ptr)-ALIGNMENT)

static size_t total_allocated = 0;
static void *replacement_malloc(size_t sz)
{
    void *chunk = malloc(sz + ALIGNMENT);
    if (!chunk) return chunk;
    total_allocated += sz;
    *(size_t*)chunk = sz;
    return OUTPTR(chunk);
}
static void *replacement_realloc(void *ptr, size_t sz)
{
    size_t old_size = 0;
    if (ptr) {
        ptr = INPTR(ptr);
        old_size = *(size_t*)ptr;
    }
    ptr = realloc(ptr, sz + ALIGNMENT);
    if (!ptr)
        return NULL;
    *(size_t*)ptr = sz;
    total_allocated = total_allocated - old_size + sz;
    return OUTPTR(ptr);
}
static void replacement_free(void *ptr)
{
    ptr = INPTR(ptr);
    total_allocated -= *(size_t*)ptr;
    free(ptr);
}

#define my_malloc(x) replacement_malloc(x)
#define my_realloc(p, x) replacement_realloc(p, x)
#define my_free(p) replacement_free(p)

int main(int argc, char const *argv[])
{
    /* code */
    void *p = NULL;

    p = my_malloc(1024);
    printf("total_allocated=%u bytes\n", total_allocated);

    p = my_realloc(p, 2048);
    printf("total_allocated=%u bytes\n", total_allocated);

    my_free(p);
    printf("total_allocated=%u bytes\n", total_allocated);

    return 0;
}
