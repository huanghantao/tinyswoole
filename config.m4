PHP_ARG_ENABLE(tinyswoole, whether to enable tinyswoole support,
dnl Make sure that the comment is aligned:
[  --enable-tinyswoole           Enable tinyswoole support])

if test "$PHP_TINYSWOOLE" != "no"; then
  PHP_SUBST(TINYSWOOLE_SHARED_LIBADD)
  source_file="tinyswoole.c tinyswoole_client.c src/client.c src/server.c"
  PHP_NEW_EXTENSION(tinyswoole, $source_file, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
