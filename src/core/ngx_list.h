
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_LIST_H_INCLUDED_
#define _NGX_LIST_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct ngx_list_part_s  ngx_list_part_t;

struct ngx_list_part_s {
    void             *elts;     //数组的头指针
    ngx_uint_t        nelts;    //已存储数组元素的个数
    ngx_list_part_t  *next;     //指向下一个ngx_list_part_t节点
};


/* 
    ngx_list_t 并不是单纯的链表，它的数据被具体的定义为了数组。
    数组中存储的才是平常定义链表中的void* data数据，所以称之为
    数组链表更加容易理解些
*/
typedef struct {
    ngx_list_part_t  *last;     //永远指向链表的最后一个节点
    ngx_list_part_t   part;     //链表头节点
    size_t            size;     //节点中数组的单个元素占用的内存大小
    ngx_uint_t        nalloc;   //每个节点中数组元素个数
    ngx_pool_t       *pool;     //链表所位于的内存池
} ngx_list_t;


ngx_list_t *ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size);

static ngx_inline ngx_int_t
ngx_list_init(ngx_list_t *list, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    list->part.elts = ngx_palloc(pool, n * size);
    if (list->part.elts == NULL) {
        return NGX_ERROR;
    }

    list->part.nelts = 0;
    list->part.next = NULL;
    list->last = &list->part;
    list->size = size;
    list->nalloc = n;
    list->pool = pool;

    return NGX_OK;
}


/*
 *
 *  the iteration through the list:
 *
 *  part = &list.part;
 *  data = part->elts;
 *
 *  for (i = 0 ;; i++) {
 *
 *      if (i >= part->nelts) {
 *          if (part->next == NULL) {
 *              break;
 *          }
 *
 *          part = part->next;
 *          data = part->elts;
 *          i = 0;
 *      }
 *
 *      ...  data[i] ...
 *
 *  }
 */


void *ngx_list_push(ngx_list_t *list);


#endif /* _NGX_LIST_H_INCLUDED_ */
