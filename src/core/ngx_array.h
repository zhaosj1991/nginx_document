
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_ARRAY_H_INCLUDED_
#define _NGX_ARRAY_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/*  
    ngx_array_t 理解为可以自动扩容的数组
    
    ngx_array_push 向其中添加元素。
    注意：push这个操作和平时用到的push用法有些不同，并不是把元素赋值然后
    加到数组中，而是返回一个可以填充元素的地址，然后我们就可以在这个地址
    里赋值，所以push操作并不需要元素的内容作为参数。
    
    ngx_array_push_n 向其中添加n个元素，返回n个元素的首地址，后续可以赋值。
*/


typedef struct {
    void        *elts;      //数组首地址
    ngx_uint_t   nelts;     //已存储元素个数，可以理解为size
    size_t       size;      //每个元素大小
    ngx_uint_t   nalloc;    //数组可容纳元素个数，可以理解为capacity
    ngx_pool_t  *pool;      //数组位于的内存池
} ngx_array_t;


ngx_array_t *ngx_array_create(ngx_pool_t *p, ngx_uint_t n, size_t size);
void ngx_array_destroy(ngx_array_t *a);
void *ngx_array_push(ngx_array_t *a);
void *ngx_array_push_n(ngx_array_t *a, ngx_uint_t n);


static ngx_inline ngx_int_t
ngx_array_init(ngx_array_t *array, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    /*
     * set "array->nelts" before "array->elts", otherwise MSVC thinks
     * that "array->nelts" may be used without having been initialized
     */

    array->nelts = 0;
    array->size = size;
    array->nalloc = n;
    array->pool = pool;

    array->elts = ngx_palloc(pool, n * size);
    if (array->elts == NULL) {
        return NGX_ERROR;
    }

    return NGX_OK;
}


#endif /* _NGX_ARRAY_H_INCLUDED_ */
