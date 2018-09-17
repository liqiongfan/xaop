PHP_ARG_ENABLE(xaop, whether to enable xaop support,
Make sure that the comment is aligned:
[  --enable-xaop           Enable xaop support])

if test "$PHP_XAOP" != "no"; then
  dnl Write more examples of tests here...

  PHP_NEW_EXTENSION(xaop, xaop.c kernel/exec.c kernel/helper.c kernel/parsing.c kernel/classes.c \
  kernel/annotations/annotation.c \
  kernel/xaop.c \
  kernel/interface/annotation.c,
  $ext_shared,,
  -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
