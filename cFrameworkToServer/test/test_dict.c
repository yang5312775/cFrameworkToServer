#include<dict.h>


#define start_benchmark() start = timeInMilliseconds()
#define end_benchmark(msg) do { \
    elapsed = timeInMilliseconds()-start; \
    printf(msg ": %ld items in %lld ms\n", count, elapsed); \
} while(0);
//
// 测试 dict.h 接口的使用
//
void test_dict(void) {
	long long start, elapsed;
	long count = 0;
	start_benchmark();

	
	printf("test dict start!!!\n");
	dict * d = dictCreateEx();
	dictEntry * entry = NULL;
	assert(d != NULL);
	dictAdd(d , "abc" , "hahahaha");
	dictAdd(d, "abcd", "qwer");
	dictAdd(d, "abce", "asdf");
	dictAdd(d, "abcf", "zxcv");
	dictAdd(d, "abcg", "yuio");
	entry = dictFind(d , "abc");

	dictIterator * iter = NULL;
	iter = dictGetIterator(d);
	entry = 1;
	while (entry != NULL)
	{
		entry = dictNext(iter);
	}
	printf("end!!\n");
	end_benchmark("Inserting");

}
# if 0
/* ------------------------------- Debugging ---------------------------------*/

#define DICT_STATS_VECTLEN 50
size_t _dictGetStatsHt(char *buf, size_t bufsize, dictht *ht, int tableid) {
	unsigned long i, slots = 0, chainlen, maxchainlen = 0;
	unsigned long totchainlen = 0;
	unsigned long clvector[DICT_STATS_VECTLEN];
	size_t l = 0;

	if (ht->used == 0) {
		return snprintf(buf, bufsize,
			"No stats available for empty dictionaries\n");
	}

	/* Compute stats. */
	for (i = 0; i < DICT_STATS_VECTLEN; i++) clvector[i] = 0;
	for (i = 0; i < ht->size; i++) {
		dictEntry *he;

		if (ht->table[i] == NULL) {
			clvector[0]++;
			continue;
		}
		slots++;
		/* For each hash entry on this slot... */
		chainlen = 0;
		he = ht->table[i];
		while (he) {
			chainlen++;
			he = he->next;
		}
		clvector[(chainlen < DICT_STATS_VECTLEN) ? chainlen : (DICT_STATS_VECTLEN - 1)]++;
		if (chainlen > maxchainlen) maxchainlen = chainlen;
		totchainlen += chainlen;
	}

	/* Generate human readable stats. */
	l += snprintf(buf + l, bufsize - l,
		"Hash table %d stats (%s):\n"
		" table size: %ld\n"
		" number of elements: %ld\n"
		" different slots: %ld\n"
		" max chain length: %ld\n"
		" avg chain length (counted): %.02f\n"
		" avg chain length (computed): %.02f\n"
		" Chain length distribution:\n",
		tableid, (tableid == 0) ? "main hash table" : "rehashing target",
		ht->size, ht->used, slots, maxchainlen,
		(float)totchainlen / slots, (float)ht->used / slots);

	for (i = 0; i < DICT_STATS_VECTLEN - 1; i++) {
		if (clvector[i] == 0) continue;
		if (l >= bufsize) break;
		l += snprintf(buf + l, bufsize - l,
			"   %s%ld: %ld (%.02f%%)\n",
			(i == DICT_STATS_VECTLEN - 1) ? ">= " : "",
			i, clvector[i], ((float)clvector[i] / ht->size) * 100);
	}

	/* Unlike snprintf(), teturn the number of characters actually written. */
	if (bufsize) buf[bufsize - 1] = '\0';
	return strlen(buf);
}

void dictGetStats(char *buf, size_t bufsize, dict *d) {
	size_t l;
	char *orig_buf = buf;
	size_t orig_bufsize = bufsize;

	l = _dictGetStatsHt(buf, bufsize, &d->ht[0], 0);
	buf += l;
	bufsize -= l;
	if (dictIsRehashing(d) && bufsize > 0) {
		_dictGetStatsHt(buf, bufsize, &d->ht[1], 1);
	}
	/* Make sure there is a NULL term at the end. */
	if (orig_bufsize) orig_buf[orig_bufsize - 1] = '\0';
}

/* ------------------------------- Benchmark ---------------------------------*/

#ifndef DICT_BENCHMARK_MAIN

#include "sds.h"

uint64_t hashCallback(const void *key) {
	return dictGenHashFunction((unsigned char*)key, sdslen((char*)key));
}

int compareCallback(void *privdata, const void *key1, const void *key2) {
	int l1, l2;
	DICT_NOTUSED(privdata);

	l1 = sdslen((sds)key1);
	l2 = sdslen((sds)key2);
	if (l1 != l2) return 0;
	return memcmp(key1, key2, l1) == 0;
}

void freeCallback(void *privdata, void *val) {
	DICT_NOTUSED(privdata);

	sdsfree(val);
}

dictType BenchmarkDictType = {
	hashCallback,
	NULL,
	NULL,
	compareCallback,
	freeCallback,
	NULL
};

#define start_benchmark() start = timeInMilliseconds()
#define end_benchmark(msg) do { \
    elapsed = timeInMilliseconds()-start; \
    printf(msg ": %ld items in %lld ms\n", count, elapsed); \
} while(0);

/* dict-benchmark [count] */
int main(int argc, char **argv) {
	long j;
	long long start, elapsed;
	dict *dict = dictCreate(&BenchmarkDictType, NULL);
	long count = 0;

	if (argc == 2) {
		count = strtol(argv[1], NULL, 10);
	}
	else {
		count = 5000000;
	}

	start_benchmark();
	for (j = 0; j < count; j++) {
		int retval = dictAdd(dict, sdsfromlonglong(j), (void*)j);
		assert(retval == DICT_OK);
	}
	end_benchmark("Inserting");
	assert((long)dictSize(dict) == count);

	/* Wait for rehashing. */
	while (dictIsRehashing(dict)) {
		dictRehashMilliseconds(dict, 100);
	}

	start_benchmark();
	for (j = 0; j < count; j++) {
		sds key = sdsfromlonglong(j);
		dictEntry *de = dictFind(dict, key);
		assert(de != NULL);
		sdsfree(key);
	}
	end_benchmark("Linear access of existing elements");

	start_benchmark();
	for (j = 0; j < count; j++) {
		sds key = sdsfromlonglong(j);
		dictEntry *de = dictFind(dict, key);
		assert(de != NULL);
		sdsfree(key);
	}
	end_benchmark("Linear access of existing elements (2nd round)");

	start_benchmark();
	for (j = 0; j < count; j++) {
		sds key = sdsfromlonglong(rand() % count);
		dictEntry *de = dictFind(dict, key);
		assert(de != NULL);
		sdsfree(key);
	}
	end_benchmark("Random access of existing elements");

	start_benchmark();
	for (j = 0; j < count; j++) {
		sds key = sdsfromlonglong(rand() % count);
		key[0] = 'X';
		dictEntry *de = dictFind(dict, key);
		assert(de == NULL);
		sdsfree(key);
	}
	end_benchmark("Accessing missing");

	start_benchmark();
	for (j = 0; j < count; j++) {
		sds key = sdsfromlonglong(j);
		int retval = dictDelete(dict, key);
		assert(retval == DICT_OK);
		key[0] += 17; /* Change first number to letter. */
		retval = dictAdd(dict, key, (void*)j);
		assert(retval == DICT_OK);
	}
	end_benchmark("Removing and adding");
}
#endif
#endif