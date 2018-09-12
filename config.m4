dnl $Id$
dnl config.m4 for extension xaop

PHP_ARG_WITH(xaop, for xaop support,
Make sure that the comment is aligned:
[  --with-xaop             Include xaop support])

if test "$PHP_XAOP" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-xaop -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/xaop.h"  # you most likely want to change this
  dnl if test -r $PHP_XAOP/$SEARCH_FOR; then # path given as parameter
  dnl   XAOP_DIR=$PHP_XAOP
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for xaop files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       XAOP_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$XAOP_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the xaop distribution])
  dnl fi

  dnl # --with-xaop -> add include path
  dnl PHP_ADD_INCLUDE($XAOP_DIR/include)

  dnl # --with-xaop -> check for lib and symbol presence
  dnl LIBNAME=xaop # you may want to change this
  dnl LIBSYMBOL=xaop # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $XAOP_DIR/$PHP_LIBDIR, XAOP_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_XAOPLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong xaop lib version or lib not found])
  dnl ],[
  dnl   -L$XAOP_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(XAOP_SHARED_LIBADD)

  PHP_NEW_EXTENSION(xaop,
   xaop.c     \
   classes.c  \
   kernel/aop/exec.c  \
   kernel/aop/xaop.c  \
   kernel/annotation/annotation.c  \
   kernel/loader/loader.c  \
   kernel/common/helper_lib.c  \
   kernel/db/adapter.c  \
   kernel/mvc/app.c  \
   kernel/mvc/controller.c  \
   kernel/mvc/model.c  \
   kernel/mvc/request.c  \
   kernel/mvc/session.c  \
   kernel/mvc/view.c  \
   kernel/common/anno_parsing.c, 
   $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
