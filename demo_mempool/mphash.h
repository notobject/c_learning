#ifndef __MPHASH__
#define __MPHASH__

#include <stdint.h>
#include <stddef.h>

typedef void (*mp_hash_table_free_t)(void *);
typedef uint32_t (*mp_hash_table_hash_t)(const void *);
typedef int (*mp_hash_table_cmp_t)(const void *, const void *);
typedef int (*mp_hash_table_walk_t)(void * /* ctx */, void * /* data */);

typedef struct mp_hash_entry_s {
	struct mp_hash_entry_s *prev;
	struct mp_hash_entry_s *next;
	void *data;
	uint32_t key;
	uint32_t reversed;
} mp_hash_entry_t;

typedef struct mp_hash_table_s {
	uint32_t num_buckets;
	uint32_t num_elements;
	uint32_t mask;

	mp_hash_entry_t **table_buckets;

	mp_hash_entry_t *bucket_nodes;
	mp_hash_entry_t *free_buckets;

	mp_hash_table_hash_t hash;
	mp_hash_table_cmp_t cmp;
	mp_hash_table_free_t free;
} mp_hash_table_t;

uint32_t mp_hash(const void *, size_t);

mp_hash_table_t *mp_hash_table_create(mp_hash_table_hash_t hashfunc,
		mp_hash_table_cmp_t cmpfunc, mp_hash_table_free_t freefunc,
		uint32_t buckets_size);

void mp_hash_table_free(mp_hash_table_t *ht);

int	mp_hash_table_insert(mp_hash_table_t *ht, void *data);
int	mp_hash_table_delete(mp_hash_table_t *ht, const void *data);
void *mp_hash_table_find(mp_hash_table_t *ht, const void *data);
int mp_hash_table_walk(mp_hash_table_t *ht, mp_hash_table_walk_t walkfunc, void *ctx);

#endif


