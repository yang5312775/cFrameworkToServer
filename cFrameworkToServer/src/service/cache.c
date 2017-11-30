#include"cache.h"


//token»º´æ
dict * tokenCache = NULL;
pthread_spinlock_t  token_spin_lock;
//êÇ³Æ»º´æ
dict * nicknameCache = NULL;
pthread_spinlock_t  nickname_spin_lock;
//¹«¹²»º´æ
dict * commonCache = NULL;
pthread_spinlock_t  common_spin_lock;


void createCache(void)
{
	pthread_spin_init(&token_spin_lock, PTHREAD_PROCESS_PRIVATE);
	tokenCache =  dictCreateEx();
	pthread_spin_init(&nickname_spin_lock, PTHREAD_PROCESS_PRIVATE);
	nicknameCache = dictCreateEx();
	pthread_spin_init(&common_spin_lock, PTHREAD_PROCESS_PRIVATE);
	commonCache = dictCreateEx();
}

static int selectCache(int cache, dict ** d, pthread_spinlock_t ** lock)
{
	switch (cache)
	{
	case CACHE_TOKEN:
		*d = tokenCache;
		*lock = &token_spin_lock;
		return 0;
	case CACHE_NICK:
		*d = nicknameCache;
		*lock = &nickname_spin_lock;
		return 0;
	case CACHE_COMM:
		*d = commonCache;
		*lock = &common_spin_lock;
		return 0;
	}
	return ERR_UNDEFINED_CACHE;
}

int cacheInsert(int cache, char * key, void * obj)
{
	int ret = 0;
	dict * d = NULL;
	pthread_spinlock_t * lock = NULL;
	selectCache(cache, &d, &lock);
	pthread_spin_lock(lock);
	ret = dictAdd(d, key, obj);
	pthread_spin_unlock(lock);
	return ret;
}

void * cacheQuery(int cache , char * key)
{
	void * ret = NULL;
	dict * d = NULL;
	pthread_spinlock_t * lock = NULL;
	selectCache(cache, &d, &lock);
	pthread_spin_lock(lock);
	ret = dictFetchValue(d, key);
	pthread_spin_unlock(lock);
	return ret;
}

int cacheModify(int cache , char * key, void * obj)
{
	int ret = 0;
	dict * d = NULL;
	pthread_spinlock_t * lock = NULL;
	selectCache(cache, &d, &lock);
	pthread_spin_lock(lock);
	ret = dictReplace(d, key, obj);
	pthread_spin_unlock(lock);
	return ret;
}

int cacheDelete(int cache, char * key)
{
	int ret = 0;
	dict * d = NULL;
	pthread_spinlock_t * lock = NULL;
	selectCache(cache, &d, &lock);
	pthread_spin_lock(lock);
	ret = dictDelete(d, key);
	pthread_spin_unlock(lock);
	return ret;
}