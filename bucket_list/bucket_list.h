#include <stddef.h>

struct bucketHeader{
    size_t used;
    size_t cap;
    void *elem;
};

typedef struct bucketList{
    size_t elem_size;
    
    size_t bucket_size;

    size_t bucket_used;
    size_t bucket_cap;
    struct bucketHeader *_buckets;
}bucketList;


bucketList bucketList_make(size_t elem_size, size_t bucket_size);

void bucketList_free(bucketList *lst);

struct bucketHeader bucketList_get(bucketList *lst, size_t bucket_no);

void bucketList_insert(bucketList *lst, size_t bucket_no, void *elem);

void bucketList_add_buckets(bucketList *lst, int amt);
