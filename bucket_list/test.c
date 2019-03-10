#include "bucket_list.h"
#include "../util.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){

    if(argc < 4){
        printf("Usage: test <max_buckets> <max_bucket_size> <num_tests>\n");
        exit(1);
    }
    size_t max_buckets = strtol(argv[1], NULL, 10);
    size_t max_bucket_size = strtol(argv[2], NULL, 10);
    size_t num_tests = strtol(argv[3], NULL, 10);

    bucketList list = bucketList_make(sizeof(int), 8);

    bucketList_add_buckets(&list, max_buckets - list.bucket_cap);

    size_t *bucket_sizes = check_malloc(sizeof(size_t) * max_buckets, "malloc 3");

    int **bucket_list = check_malloc(
            sizeof(int*) * max_buckets,
            "malloc top bucket_list");

    for(int i = 0; i < max_buckets; i++){
        bucket_list[i] = check_malloc(sizeof(int) * max_bucket_size, "malloc bucket");
        bucket_sizes[i] = 0;
    }
    
    for(int i = 0; i < num_tests; i++){
        printf("%d\n", i);
        size_t bucket = rand() % max_buckets;
        if(bucket_sizes[bucket] < max_bucket_size){
            int elem = rand();
            bucketList_insert(&list, bucket, &elem);
            bucket_list[bucket][bucket_sizes[bucket]] = elem;
            bucket_sizes[bucket]++;
        }
    }

    bool valid = true;

    for(int i = 0; i < max_buckets; i++){
        struct bucketHeader lst = bucketList_get(&list, i);

        int *elem = (int*) lst.elem;

        for(int j = 0; j < lst.len; j++){
            if(elem[j] != bucket_list[i][j]){
                valid = false;
                break;
            }
        }

    }

    if(valid){
        printf("passed");
    }else
        printf("failed");
}
