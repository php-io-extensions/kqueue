#ifndef PHP_KQUEUE_H
#define PHP_KQUEUE_H

extern zend_module_entry kqueue_module_entry;
#define phpext_kqueue_ptr &kqueue_module_entry

#define PHP_KQUEUE_VERSION "0.10.0"

ZEND_BEGIN_MODULE_GLOBALS(kqueue)
	int last_errno;
ZEND_END_MODULE_GLOBALS(kqueue)

ZEND_EXTERN_MODULE_GLOBALS(kqueue)
#define KQUEUE_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(kqueue, v)

#if defined(ZTS) && defined(COMPILE_DL_KQUEUE)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif /* PHP_KQUEUE_H */
