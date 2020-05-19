
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


//分配 ngx_list_t 内存（里面就包含了一个头节点）并初始化
ngx_list_t *
ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    ngx_list_t  *list;

    list = ngx_palloc(pool, sizeof(ngx_list_t));
    if (list == NULL) {
        return NULL;
    }

    //初始化ngx_list_t，需要给头节点中的数组分配实际的存储空间，
    //也就是 n*size 个字节
    if (ngx_list_init(list, pool, n, size) != NGX_OK) {
        return NULL;
    }

    return list;
}


/*
    ngx_list_push会返回链表元素中数组的一个可填充的位置，
    这个返回值后续我们可以为其赋值。
*/
void *
ngx_list_push(ngx_list_t *l)
{
    void             *elt;
    ngx_list_part_t  *last;

    last = l->last;

    //last节点所用的数组个数 == 节点中数组中元素个数，则表示这个数组被用完了
    //自然需要新申请一个节点，并链接到上一个节点，这个新节点作为last节点。
    if (last->nelts == l->nalloc) {

        /* the last part is full, allocate a new list part */

        last = ngx_palloc(l->pool, sizeof(ngx_list_part_t));
        if (last == NULL) {
            return NULL;
        }

        last->elts = ngx_palloc(l->pool, l->nalloc * l->size);
        if (last->elts == NULL) {
            return NULL;
        }

        last->nelts = 0;
        last->next = NULL;

        l->last->next = last;
        l->last = last;
    }

    //此处last节点中的数组一定还有空余，将可填充数据的地址返回，已使用数组元素个数增一
    elt = (char *) last->elts + l->size * last->nelts;
    last->nelts++;

    return elt;
}
