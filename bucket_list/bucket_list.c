#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "bucket_list.h"
#include "../util.h"

#define BUCKET_LIST_ITER 64
#define BUCKET_ITER 64


static inline struct bucketHeader init_bucket(bucketList *lst){
    struct bucketHeader bckt;

    bckt.len = 0;
    bckt.cap = 0;
    bckt.elem = NULL;

    return bckt;
}

static inline void resize_bucket(bucketList *lst, struct bucketHeader *bckt){
    
    if(bckt->elem == NULL){
        lst->len++;
        bckt->elem = check_malloc(BUCKET_ITER * lst->elem_size, "malloc in resize_bucket");


    }else
        bckt->elem = check_realloc(bckt->elem, (bckt->cap + BUCKET_ITER) * lst->elem_size,
                "realloc in resize_bucket");

    bckt->cap += BUCKET_ITER;
}


static inline void resize_bucket_list(bucketList *lst, size_t amt_to_add){
    lst->_buckets = check_realloc(
            lst->_buckets,
            sizeof(struct bucketHeader)  * (amt_to_add + lst->bucket_cap),
            "realloc in resize_bucket_list");

    for(int i = lst->bucket_cap; i < lst->bucket_cap + amt_to_add; i++)
        lst->_buckets[i] = init_bucket(lst);

    lst->bucket_cap += amt_to_add;
}

bucketList bucketList_make(size_t elem_size, size_t bucket_size){
    bucketList list;
    list.elem_size = elem_size;

    list._buckets = check_malloc(
            sizeof(struct bucketHeader) * BUCKET_LIST_ITER, "malloc in bucketList_make");

    for(int i = 0; i < BUCKET_LIST_ITER; i++){
        list._buckets[i] = init_bucket(&list);
    }

    list.bucket_cap = BUCKET_LIST_ITER;
    list.len = 0;

    return list;
}

void bucketList_free(bucketList *lst){
    for(int i = 0; i < lst->bucket_cap; i++){
        if(lst->_buckets[i].elem != NULL){
            free(lst->_buckets[i].elem);
        }
    }

    free(lst->_buckets);

}

struct bucketHeader bucketList_get(bucketList *lst, size_t bucket_no){
    return lst->_buckets[bucket_no];
}

void bucketList_insert(
        bucketList *lst, size_t bucket_no, void *elem){
    
    if(bucket_no >= lst->bucket_cap){
        resize_bucket_list(lst, 
                max(bucket_no - lst->bucket_cap + 1, BUCKET_LIST_ITER));    
    }

    struct bucketHeader *bckt = lst->_buckets;
    
    bckt += bucket_no;

    if(bckt->cap - bckt->len < 1){
       resize_bucket(lst, bckt);
    }

    char *elem_ptr = (char *) (bckt->elem);
    elem_ptr += (bckt->len * lst->elem_size);
    memcpy(elem_ptr, elem, lst->elem_size);

    bckt->len += 1;
}

void bucketList_add_buckets(bucketList *lst, int amt){
    if(amt < 0)
       amt = BUCKET_LIST_ITER;

    resize_bucket_list(lst, amt);
}


