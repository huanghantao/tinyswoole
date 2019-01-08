PHP_ARG_ENABLE(tinyswoole, whether to enable tinyswoole support,
dnl Make sure that the comment is aligned:
[  --enable-tinyswoole           Enable tinyswoole support])

if test "$PHP_TINYSWOOLE" != "no"; then
  PHP_SUBST(TINYSWOOLE_SHARED_LIBADD)
  source_file=" \
  tinyswoole.c \
  tinyswoole_server.c \
  src/log.c \
  src/socket.c \
  src/network/server.c \
  src/network/reactor_thread.c \
  src/network/process_pool.c \
  src/network/worker.c \
  src/reactor/epoll.c \
  src/reactor/base.c \
  src/pipe/unix_socket.c"
  PHP_NEW_EXTENSION(tinyswoole, $source_file, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
