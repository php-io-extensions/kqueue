PHP_ARG_ENABLE([kqueue],
  [whether to enable kqueue support],
  [AS_HELP_STRING([--enable-kqueue], [Enable kqueue(2) bindings])],
  [no])

if test "$PHP_KQUEUE" != "no"; then
  AC_CHECK_HEADERS([sys/event.h], [], [AC_MSG_ERROR([kqueue requires <sys/event.h>])])
  AC_CHECK_FUNCS([kqueue kevent kevent64], [], [AC_MSG_ERROR([kqueue requires kqueue(), kevent() and kevent64()])])
  PHP_NEW_EXTENSION([kqueue], [kqueue.c], [$ext_shared],, [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
fi
