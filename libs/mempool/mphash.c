#include "include/mphash.h"
#include <stdlib.h>
#include <unistd.h>

/*
 * perl -e 'foreach $i (0..255) {$r = 0; foreach $j (0 .. 7 ) { if (($i & ( 1<< $j)) != 0) { $r |= (1 << (7 - $j));}} print $r, ", ";if (($i & 7) == 7) {print "\n";}}'
 */
static const uint8_t reversed_byte[256] = {
	0,  128, 64, 192, 32, 160, 96,  224,
	16, 144, 80, 208, 48, 176, 112, 240,
	8,  136, 72, 200, 40, 168, 104, 232,
	24, 152, 88, 216, 56, 184, 120, 248,
	4,  132, 68, 196, 36, 164, 100, 228,
	20, 148, 84, 212, 52, 180, 116, 244,
	12, 140, 76, 204, 44, 172, 108, 236,
	28, 156, 92, 220, 60, 188, 124, 252,
	2,  130, 66, 194, 34, 162, 98,  226,
	18, 146, 82, 210, 50, 178, 114, 242,
	10, 138, 74, 202, 42, 170, 106, 234,
	26, 154, 90, 218, 58, 186, 122, 250,
	6,  134, 70, 198, 38, 166, 102, 230,
	22, 150, 86, 214, 54, 182, 118, 246,
	14, 142, 78, 206, 46, 174, 110, 238,
	30, 158, 94, 222, 62, 190, 126, 254,
	1,  129, 65, 193, 33, 161, 97,  225,
	17, 145, 81, 209, 49, 177, 113, 241,
	9,  137, 73, 201, 41, 169, 105, 233,
	25, 153, 89, 217, 57, 185, 121, 249,
	5,  133, 69, 197, 37, 165, 101, 229,
	21, 149, 85, 213, 53, 181, 117, 245,
	13, 141, 77, 205, 45, 173, 109, 237,
	29, 157, 93, 221, 61, 189, 125, 253,
	3,  131, 67, 195, 35, 163, 99,  227,
	19, 147, 83, 211, 51, 179, 115, 243,
	11, 139, 75, 203, 43, 171, 107, 235,
	27, 155, 91, 219, 59, 187, 123, 251,
	7,  135, 71, 199, 39, 167, 103, 231,
	23, 151, 87, 215, 55, 183, 119, 247,
	15, 143, 79, 207, 47, 175, 111, 239,
	31, 159, 95, 223, 63, 191, 127, 255
};

static uint32_t reverse(uint32_t key)
{
	return ((reversed_byte[key & 0xff] << 24) |
		(reversed_byte[(key >> 8) & 0xff] << 16) |
		(reversed_byte[(key >> 16) & 0xff] << 8) |
		(reversed_byte[(key >> 24) & 0xff]));
}

static inline int is_power_of_2(uint32_t n)
{
	return n && !(n & (n - 1));
}

#define FNV_MAGIC_INIT (0x811c9dc5)
#define FNV_MAGIC_PRIME (0x01000193)

uint32_t mp_hash(const void *data, size_t size)
{
	const uint8_t *p = data;
	const uint8_t *q = p + size;
	uint32_t hash = FNV_MAGIC_INIT;

	/*
	 *	FNV-1 hash each octet in the buffer
	 */
	while (p != q) {
		/*
		 *	Multiple by 32-bit magic FNV prime, mod 2^32
		 */
		hash *= FNV_MAGIC_PRIME;

		/*
		 *	XOR the 8-bit quantity into the bottom of
		 *	the hash.
		 */
		hash ^= (uint32_t) (*p++);
	}

	return hash;
}

mp_hash_table_t *mp_hash_table_create(mp_hash_table_hash_t hashfunc,
		mp_hash_table_cmp_t cmpfunc, mp_hash_table_free_t freefunc,
		uint32_t buckets_size)
{
	if (buckets_size == 0) {
		return NULL;
	}

	if (!is_power_of_2(buckets_size)) {
		uint32_t size = 1;
		while (size < buckets_size) {
			size = size << 1;
		}
		buckets_size = size;
	}

	mp_hash_table_t *ht = calloc(1, sizeof(mp_hash_table_t));
	if (!ht) {
		return NULL;
	}

	ht->num_buckets = buckets_size;
	ht->mask = buckets_size - 1;
	ht->num_elements = 0;

	ht->table_buckets = calloc(ht->num_buckets, sizeof(*ht->table_buckets));
	if (!ht->table_buckets) {
		free(ht);
		return NULL;
	}

	ht->bucket_nodes = calloc(ht->num_buckets, sizeof(*ht->bucket_nodes));
	if (!ht->bucket_nodes) {
		free(ht->table_buckets);
		free(ht);
		return NULL;
	}

	int i = 0;
	for (i = 0; i < ht->num_buckets - 1; i++) {
		ht->bucket_nodes[i].next = ht->bucket_nodes + i + 1;
		ht->bucket_nodes[i + 1].prev = ht->bucket_nodes + i;
	}

	ht->free_buckets = ht->bucket_nodes;

	ht->hash = hashfunc;
	ht->cmp = cmpfunc;
	ht->free = freefunc;

	return ht;
}

void mp_hash_table_free(mp_hash_table_t *ht)
{
	if (!ht) {
		return;
	}

	if (ht->free) {
		int i = 0;
		for (i = 0; i < ht->num_buckets; i++) {
			mp_hash_entry_t *entry = ht->table_buckets[i];
			while (entry) {
				ht->free(entry->data);
				entry = entry->next;
			}
		}
	}

	ht->num_elements = 0;
	ht->mask = 0;
	ht->num_buckets = 0;
	ht->free_buckets = NULL;

	free(ht->table_buckets);
	ht->table_buckets = NULL;
	free(ht->bucket_nodes);
	ht->bucket_nodes = NULL;
	free(ht);
}

static inline mp_hash_entry_t *mp_get_free_entry(mp_hash_table_t *ht)
{
	if (!ht->free_buckets) {
		return NULL;
	}

	mp_hash_entry_t *entry = ht->free_buckets;
	ht->free_buckets = ht->free_buckets->next;
	if (ht->free_buckets) {
		ht->free_buckets->prev = NULL;
	}

	entry->data = entry->prev = entry->next = NULL;
	entry->key = 0;

	return entry;
}

static inline void mp_put_free_entry(mp_hash_table_t *ht, mp_hash_entry_t *entry)
{
	entry->key = entry->reversed = 0;
	entry->data = NULL;
	entry->prev = NULL;
	entry->next = ht->free_buckets;
	if (ht->free_buckets) {
		ht->free_buckets->prev = entry;
	}
	ht->free_buckets = entry;
}

static inline int mp_list_insert(mp_hash_table_t *ht, mp_hash_entry_t **head, mp_hash_entry_t *entry)
{
	mp_hash_entry_t *cur = *head;
	mp_hash_entry_t *last = NULL;

	if (!*head) {
		*head = entry;
		return 0;
	}

	while (cur) {
		if (cur->reversed > entry->reversed) {
			break;
		}
		last = cur;

		if (cur->reversed == entry->reversed) {
			if (ht->cmp && ht->cmp(entry->data, cur->data)) {
				cur = cur->next;
				continue;
			}

			return -1;
		}

		cur = cur->next;
	}

	if (last) {
		entry->prev = last;
		if (last->next) {
			last->next->prev = entry;
			entry->next = last->next;
		}
		last->next = entry;
	} else {
		entry->next = *head;
		(*head)->prev = entry;
		*head = entry;
	}

	return 0;
}

static inline mp_hash_entry_t *mp_list_find(mp_hash_table_t *ht,
		mp_hash_entry_t *head, const void *data, uint32_t reversed)
{
	mp_hash_entry_t *cur = head;

	while (cur) {
		if (cur->reversed > reversed) {
			return NULL;
		}

		if (cur->reversed == reversed) {
			if (ht->cmp && ht->cmp(data, cur->data)) {
				cur = cur->next;
				continue;
			}

			break;
		}

		cur = cur->next;
	}

	return cur;
}

static inline mp_hash_entry_t *mp_list_delete(mp_hash_table_t *ht,
		mp_hash_entry_t **head, const void *data, uint32_t reversed)
{
	mp_hash_entry_t *cur = mp_list_find(ht, *head, data, reversed);
	if (cur) {
		if (cur->prev) {
			cur->prev->next = cur->next;
		} else {
			*head = cur->next;
			if (*head) {
				(*head)->prev = NULL;
			}
		}

		if (cur->next) {
			cur->next->prev = cur->prev;
		}

		cur->prev = cur->next = NULL;
	}

	return cur;
}

int mp_hash_table_insert(mp_hash_table_t *ht, void *data)
{
	if (!ht || !data) {
		return -1;
	}

	if (ht->num_elements >= ht->num_buckets) {
		return -1;
	}

	uint32_t key = ht->hash(data);
	uint32_t pos = key % ht->mask;
	uint32_t reversed = reverse(key);

	mp_hash_entry_t *entry = mp_get_free_entry(ht);
	if (!entry) {
		return -1;
	}

	entry->data = data;
	entry->key = key;
	entry->reversed = reversed;

	if (mp_list_insert(ht, &(ht->table_buckets[pos]), entry) != 0) {
		mp_put_free_entry(ht, entry);
		return -1;
	}

	ht->num_elements++;

	return 0;
}

int mp_hash_table_delete(mp_hash_table_t *ht, const void *data)
{
	if (!ht || !data) {
		return -1;
	}

	uint32_t key = ht->hash(data);
	uint32_t pos = key % ht->mask;
	uint32_t reversed = reverse(key);

	mp_hash_entry_t *entry = mp_list_delete(ht, &(ht->table_buckets[pos]), data, reversed);
	if (!entry) {
		return -1;
	}

	if (ht->free) {
		ht->free(entry->data);
	}

	mp_put_free_entry(ht, entry);
	ht->num_elements--;

	return 0;
}

void *mp_hash_table_find(mp_hash_table_t *ht, const void *data)
{
	if (!ht || !data) {
		return NULL;
	}

	uint32_t key = ht->hash(data);
	uint32_t pos = key % ht->mask;
	uint32_t reversed = reverse(key);

	mp_hash_entry_t *entry = mp_list_find(ht, ht->table_buckets[pos], data, reversed);
	if (!entry) {
		return NULL;
	}

	return entry->data;
}

int mp_hash_table_walk(mp_hash_table_t *ht, mp_hash_table_walk_t walkfunc, void *ctx)
{
	if (!ht || !walkfunc) {
		return -1;
	}

	int i = 0, iret = 0;
	for (i = 0; i < ht->num_buckets; i++) {
		mp_hash_entry_t *entry = ht->table_buckets[i];
		while (entry) {
			iret = walkfunc(ctx, entry->data);
			if (iret != 0) {
				return iret;
			}

			entry = entry->next;
		}
	}

	return 0;
}

#ifdef __DEBUG
#include <stdio.h>

uint32_t ihash(const void *data)
{
	return (uint32_t) *((int *) data);
}

int icmp(const void *a, const void *b)
{
	return ((*((int *) a)) - *((int *) b));
}

int iwalk(void *ctx, void *data)
{
	printf("%d\n", *((int *) data));
	return 0;
}

int main()
{
	mp_hash_table_t *ht = mp_hash_table_create(ihash, icmp, NULL, 16);
	int ival = 30;
	mp_hash_table_insert(ht, &ival);
	int ival2 = 15;
	mp_hash_table_insert(ht, &ival2);
	int ival3 = 0;
	mp_hash_table_insert(ht, &ival3);

	mp_hash_table_walk(ht, iwalk, NULL);

	void *result = mp_hash_table_find(ht, &ival2);
	mp_hash_table_delete(ht, &ival2);

	mp_hash_table_walk(ht, iwalk, NULL);

	result = mp_hash_table_find(ht, &ival2);

	mp_hash_table_free(ht);

	return 0;
}
#endif


