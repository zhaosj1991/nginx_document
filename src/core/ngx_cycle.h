
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CYCLE_H_INCLUDED_
#define _NGX_CYCLE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


#ifndef NGX_CYCLE_POOL_SIZE
#define NGX_CYCLE_POOL_SIZE     NGX_DEFAULT_POOL_SIZE
#endif


#define NGX_DEBUG_POINTS_STOP   1
#define NGX_DEBUG_POINTS_ABORT  2


typedef struct ngx_shm_zone_s  ngx_shm_zone_t;

typedef ngx_int_t (*ngx_shm_zone_init_pt) (ngx_shm_zone_t *zone, void *data);

struct ngx_shm_zone_s {
    void                     *data;
    ngx_shm_t                 shm;
    ngx_shm_zone_init_pt      init;
    void                     *tag;
    void                     *sync;
    ngx_uint_t                noreuse;  /* unsigned  noreuse:1; */
};


struct ngx_cycle_s {
    void                  ****conf_ctx;         //所有模块存储配置项的结构体指针
    ngx_pool_t               *pool;

    ngx_log_t                *log;              //日志模块中提供了生成基本ngx_log_t日志对象的功能
    ngx_log_t                 new_log;          //调用ngx_init_cycle方法后，会用new_log的地址覆盖上面的log指针

    ngx_uint_t                log_use_stderr;  /* unsigned  log_use_stderr:1; */

    //fiels保存所有ngx_connection_t的指针组成的数组，files_n就是指针的总数，而文件句柄的值用来访问files数组成员
    ngx_connection_t        **files;
    ngx_connection_t         *free_connections; //连接池中可用连接总数
    ngx_uint_t                free_connection_n;//空闲连接池中连接的总数

    ngx_module_t            **modules;
    ngx_uint_t                modules_n;
    ngx_uint_t                modules_used;    /* unsigned  modules_used:1; */

    ngx_queue_t               reusable_connections_queue;
    ngx_uint_t                reusable_connections_n;

    ngx_array_t               listening;    //存储ngx_listening_t成员
    
    //保存着Nginx所有要操作的目录，如果目录不存在，则会试图创建，而创建目录失败将会导致Nginx启动失败。
    ngx_array_t               paths;        

    ngx_array_t               config_dump;
    ngx_rbtree_t              config_dump_rbtree;
    ngx_rbtree_node_t         config_dump_sentinel;

    ngx_list_t                open_files;
    ngx_list_t                shared_memory;

    ngx_uint_t                connection_n;
    ngx_uint_t                files_n;      //表示files数组中ngx_connection_t指针的总数

    ngx_connection_t         *connections;  //指向当前进程中的所有连接对象，每个连接对象对应一个写事件和一个读事件
    ngx_event_t              *read_events;  //指向当前进程中的所有写事件对象，connection_n同时表示所有读事件的总数
    ngx_event_t              *write_events; //指向当前进程中的所有写事件对象，connection_n同时表示所有写事件的总数

    /*
        旧的ngx_cycle_t对象用于引用上一个ngx_cycle_t对象中的成员，例如ngx_init_cycle方法，
        在启动初期，需要建立一个临时的ngx_cycle_t对象保存一些变量，在调用ngx_init_cycle方法时，
        就可以把旧的ngx_cycle_t的对象传进去，而这时old_clcle对象
        就会保存这个前期的ngx_clcle_t对象。
    */
    ngx_cycle_t              *old_cycle;

    ngx_str_t                 conf_file;    //配置文件相对于安装目录的路径名称
    ngx_str_t                 conf_param;   //Nginx处理配置文件时需要特殊处理的在命令行携带的参数，一般是-g选项携带的参数
    ngx_str_t                 conf_prefix;  //Nginx配置文件所在的路径
    ngx_str_t                 prefix;       //Nginx安装目录的路径
    ngx_str_t                 lock_file;    //用于进程间同步的文件锁名称
    ngx_str_t                 hostname;     //使用gethostname系统调用得到的主机名
};


typedef struct {
    ngx_flag_t                daemon;
    ngx_flag_t                master;

    ngx_msec_t                timer_resolution;
    ngx_msec_t                shutdown_timeout;

    ngx_int_t                 worker_processes;
    ngx_int_t                 debug_points;

    ngx_int_t                 rlimit_nofile;
    off_t                     rlimit_core;

    int                       priority;

    ngx_uint_t                cpu_affinity_auto;
    ngx_uint_t                cpu_affinity_n;
    ngx_cpuset_t             *cpu_affinity;

    char                     *username;
    ngx_uid_t                 user;
    ngx_gid_t                 group;

    ngx_str_t                 working_directory;
    ngx_str_t                 lock_file;

    ngx_str_t                 pid;
    ngx_str_t                 oldpid;

    ngx_array_t               env;
    char                    **environment;

    ngx_uint_t                transparent;  /* unsigned  transparent:1; */
} ngx_core_conf_t;


#define ngx_is_init_cycle(cycle)  (cycle->conf_ctx == NULL)


ngx_cycle_t *ngx_init_cycle(ngx_cycle_t *old_cycle);
ngx_int_t ngx_create_pidfile(ngx_str_t *name, ngx_log_t *log);
void ngx_delete_pidfile(ngx_cycle_t *cycle);
ngx_int_t ngx_signal_process(ngx_cycle_t *cycle, char *sig);
void ngx_reopen_files(ngx_cycle_t *cycle, ngx_uid_t user);
char **ngx_set_environment(ngx_cycle_t *cycle, ngx_uint_t *last);
ngx_pid_t ngx_exec_new_binary(ngx_cycle_t *cycle, char *const *argv);
ngx_cpuset_t *ngx_get_cpu_affinity(ngx_uint_t n);
ngx_shm_zone_t *ngx_shared_memory_add(ngx_conf_t *cf, ngx_str_t *name,
    size_t size, void *tag);
void ngx_set_shutdown_timer(ngx_cycle_t *cycle);


extern volatile ngx_cycle_t  *ngx_cycle;
extern ngx_array_t            ngx_old_cycles;
extern ngx_module_t           ngx_core_module;
extern ngx_uint_t             ngx_test_config;
extern ngx_uint_t             ngx_dump_config;
extern ngx_uint_t             ngx_quiet_mode;


#endif /* _NGX_CYCLE_H_INCLUDED_ */
