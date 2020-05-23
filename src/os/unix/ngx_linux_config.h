
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_LINUX_CONFIG_H_INCLUDED_
#define _NGX_LINUX_CONFIG_H_INCLUDED_


#ifndef _GNU_SOURCE
#define _GNU_SOURCE             /* pread(), pwrite(), gethostname() */
#endif

#define _FILE_OFFSET_BITS  64

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stddef.h>             /* offsetof() */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <glob.h>
#include <sys/vfs.h>            /* statfs() */

#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sched.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>        /* TCP_NODELAY, TCP_CORK */
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>

#include <time.h>               /* tzset() */
#include <malloc.h>             /* memalign() */
#include <limits.h>             /* IOV_MAX */
#include <sys/ioctl.h>
#include <crypt.h>
#include <sys/utsname.h>        /* uname() */

#include <dlfcn.h>


#include <ngx_auto_config.h>


#if (NGX_HAVE_POSIX_SEM)
#include <semaphore.h>
#endif


#if (NGX_HAVE_SYS_PRCTL_H)
#include <sys/prctl.h>
#endif


#if (NGX_HAVE_SENDFILE64)
#include <sys/sendfile.h>
#else
extern ssize_t sendfile(int s, int fd, int32_t *offset, size_t size);
#define NGX_SENDFILE_LIMIT  0x80000000
#endif


#if (NGX_HAVE_POLL)
#include <poll.h>
#endif


#if (NGX_HAVE_EPOLL)
#include <sys/epoll.h>
#endif


#if (NGX_HAVE_SYS_EVENTFD_H)
#include <sys/eventfd.h>
#endif
#include <sys/syscall.h>
#if (NGX_HAVE_FILE_AIO)
#include <linux/aio_abi.h>
typedef struct iocb  ngx_aiocb_t;
#endif


#if (NGX_HAVE_CAPABILITIES)
#include <linux/capability.h>
#endif


#define NGX_LISTEN_BACKLOG        511


#ifndef NGX_HAVE_SO_SNDLOWAT
/* setsockopt(SO_SNDLOWAT) returns ENOPROTOOPT */
#define NGX_HAVE_SO_SNDLOWAT         0
#endif


#ifndef NGX_HAVE_INHERITED_NONBLOCK
#define NGX_HAVE_INHERITED_NONBLOCK  0
#endif


#define NGX_HAVE_OS_SPECIFIC_INIT    1
#define ngx_debug_init()

/*
    environ属于linux系统定义的变量，通过 man environ 以下可以看到介绍。
    通过printf循环打印environ[i]就可以看到，其实environ保存了当前终端
    的所有环境变量，linux的env命令就是打印了这个数组的内容。

   The  variable  environ  points  to  an array of pointers to strings called the "environment".  The last pointer in this
   array has the value NULL.  (This variable must be declared in the user program, but is  declared  in  the  header  file
   <unistd.h>  if  the _GNU_SOURCE feature test macro is defined.)  This array of strings is made available to the process
   by the exec(3) call that started the process.
*/
extern char **environ;


#endif /* _NGX_LINUX_CONFIG_H_INCLUDED_ */
